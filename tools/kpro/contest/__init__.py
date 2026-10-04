from __future__ import annotations

import json
import re
import shutil
import tempfile
from datetime import datetime
from pathlib import Path

from kpro.config import Config, read_toml
from kpro.core import KproError, git, head, within, write_text


def toml_text(data: dict) -> str:
    # This subset covers contest/lock metadata without a TOML writer dependency.
    scalar = []
    sections = []
    for key, value in data.items():
        if isinstance(value, dict):
            sections += [f"\n[{key}]\n" + toml_text(value)]
        else:
            literal = (
                str(value).lower()
                if isinstance(value, bool)
                else json.dumps(value, ensure_ascii=False)
            )
            scalar.append(f"{key} = {literal}\n")
    return "".join(scalar + sections)


def lock_path(config: Config) -> Path:
    path = git(config.root, "rev-parse", "--git-path", "kpro/contest-lock.toml")
    return Path(path) if Path(path).is_absolute() else config.root / path


def validate_metadata(path: Path) -> dict:
    data = read_toml(path)
    required = {"contest", "platform", "url", "base_commit", "started_at", "branch", "cpp"}
    if required - set(data):
        raise KproError(
            f"Missing contest metadata in {path}: {', '.join(sorted(required - set(data)))}"
        )
    if set(data) - (required | {"snapshot_tag"}):
        raise KproError(f"Unknown contest metadata keys in {path}.")
    for key in required - {"cpp"}:
        if not isinstance(data[key], str) or not data[key]:
            raise KproError(f"contest.toml {key} must be a nonempty string.")
    if not re.fullmatch(r"[a-z0-9_-]+", data["contest"]) or not re.fullmatch(
        r"[0-9a-f]{40}|[0-9a-f]{64}", data["base_commit"]
    ):
        raise KproError(f"Invalid contest id or full base_commit hash: {path}")
    if data["branch"] != f"contest/{data['contest']}":
        raise KproError(f"Contest branch must be contest/{data['contest']}.")
    if "snapshot_tag" in data and data["snapshot_tag"] != f"contest-snapshot/{data['contest']}":
        raise KproError("Invalid snapshot_tag in contest.toml.")
    try:
        started = datetime.fromisoformat(data["started_at"])
        if started.tzinfo is None:
            raise ValueError("timezone required")
    except ValueError as exc:
        raise KproError(f"started_at must be an ISO timestamp with timezone: {path}") from exc
    cpp = data["cpp"]
    if (
        not isinstance(cpp, dict)
        or set(cpp) != {"compiler", "standard"}
        or not isinstance(cpp["compiler"], str)
        or not cpp["compiler"]
        or cpp["standard"] not in {"c++17", "c++20", "c++23"}
    ):
        raise KproError(f"Invalid contest [cpp] compiler/standard: {path}")
    return data


def active(config: Config) -> dict | None:
    path = lock_path(config)
    if not path.exists():
        return None
    data = read_toml(path)
    if (
        set(data) != {"contest", "started_at", "workspace", "base_commit", "branch", "disable_push"}
        or type(data.get("disable_push")) is not bool
        or not all(
            isinstance(item, str) and item for key, item in data.items() if key != "disable_push"
        )
    ):
        raise KproError(f"Malformed contest lock: {path}. Inspect it before recovering manually.")
    workspace = within(config.root, data["workspace"])
    metadata = validate_metadata(workspace / "contest.toml")
    for key in ("contest", "base_commit", "branch", "started_at"):
        if data[key] != metadata[key]:
            raise KproError(f"Contest lock disagrees with contest.toml on {key}.")
    return data


def start(config: Config, identifier: str, backend) -> dict:
    root = config.root
    if active(config):
        raise KproError("Contest mode is already active. Run `kpro contest end` first.")
    base = head(root)
    if not base:
        raise KproError("Create an initial Git commit before starting a contest.")
    branch = git(root, "branch", "--show-current")
    if branch != config.section("contest")["base_branch"]:
        raise KproError(
            f"Start a contest from {config.section('contest')['base_branch']}; current branch is {branch}."
        )
    if git(root, "status", "--porcelain"):
        raise KproError(
            "Cannot start contest: working tree is dirty. Commit or stash changes before retrying."
        )
    if not shutil.which(config.section("cpp")["compiler"]):
        raise KproError("Configured compiler is unavailable. Run `kpro doctor`.")
    if config.section("contest")["disable_push"]:
        configured = git(root, "config", "--get", "core.hooksPath", check=False)
        hooks = Path(configured) if configured else Path(".git/hooks")
        hooks = hooks if hooks.is_absolute() else root / hooks
        hook = hooks / "pre-push"
        if (
            hooks.resolve() != (root / ".githooks").resolve()
            or not hook.is_file()
            or not hook.stat().st_mode & 0o111
        ):
            raise KproError(
                "Install the push guard first: git config core.hooksPath .githooks; chmod +x .githooks/pre-push"
            )
    contest = backend.fetch_contest(identifier)
    contest_id = contest.id
    if not re.fullmatch(r"[a-z0-9_-]+", contest_id):
        raise KproError("Judge returned an invalid contest id.")
    now = datetime.now().astimezone()
    workspace = config.path("contest_root") / str(now.year) / contest_id
    contest_branch = f"contest/{contest_id}"
    tag = f"contest-snapshot/{contest_id}"
    if (
        workspace.exists()
        or git(root, "branch", "--list", contest_branch)
        or git(root, "tag", "--list", tag)
    ):
        raise KproError(f"Contest workspace, branch, or snapshot already exists for {contest_id}.")
    metadata = {
        "contest": contest_id,
        "platform": contest.platform,
        "url": contest.url,
        "base_commit": base,
        "started_at": now.isoformat(timespec="seconds"),
        "branch": contest_branch,
        "cpp": {key: config.section("cpp")[key] for key in ("compiler", "standard")},
    }
    if config.section("contest")["record_snapshot_tag"]:
        metadata["snapshot_tag"] = tag
    workspace.parent.mkdir(parents=True, exist_ok=True)
    (root / ".kpro").mkdir(exist_ok=True)
    made_tag = made_branch = made_workspace = False
    try:
        with tempfile.TemporaryDirectory(prefix=".kpro-contest-", dir=root / ".kpro") as folder:
            staging = Path(folder)
            if not contest.problems:
                raise KproError(
                    "Judge returned no problems; check the contest URL or login with oj."
                )
            seen = set()
            for problem in contest.problems:
                if not re.fullmatch(r"[a-z0-9_-]+", problem.id) or problem.id in seen:
                    raise KproError("Judge returned invalid/duplicate problem ids.")
                seen.add(problem.id)
                directory = staging / problem.id
                directory.mkdir()
                (directory / "main.cpp").write_text(
                    "#include <iostream>\n\nint main() {\n    std::ios::sync_with_stdio(false);\n    std::cin.tie(nullptr);\n}\n",
                    encoding="utf-8",
                )
                write_text(
                    directory / "problem.toml",
                    toml_text({"id": problem.id, "title": problem.title, "url": problem.url}),
                )
                backend.download_samples(problem, directory / "tests")
            write_text(staging / "contest.toml", toml_text(metadata))
            write_text(
                staging / "notes.md",
                f"---\ncontest: {contest_id}\ntags: []\n---\n\n# {contest_id}\n\n## Initial idea\n\n## Key observations\n\n## Failed approaches\n\n## Final approach\n\n## WA/TLE causes\n\n## Lessons\n\n## Reusable technique\n",
            )
            if config.section("contest")["record_snapshot_tag"]:
                git(root, "tag", tag, base)
                made_tag = True
            git(root, "switch", "-c", contest_branch)
            made_branch = True
            shutil.move(str(staging), str(workspace))
            made_workspace = True
            # Keep active lifecycle state even if disable_push=false; the hook reads policy.
            write_text(
                lock_path(config),
                toml_text(
                    {
                        key: metadata[key]
                        for key in ("contest", "started_at", "base_commit", "branch")
                    }
                    | {
                        "workspace": workspace.relative_to(root).as_posix(),
                        "disable_push": config.section("contest")["disable_push"],
                    }
                ),
            )
    except Exception:
        lock_path(config).unlink(missing_ok=True)
        if made_workspace:
            shutil.rmtree(workspace)
        if made_branch:
            git(root, "switch", branch)
            git(root, "branch", "-D", contest_branch)
        if made_tag:
            git(root, "tag", "-d", tag)
        raise
    return metadata | {"workspace": workspace.relative_to(root).as_posix()}


def end(config: Config) -> dict:
    data = active(config)
    if not data:
        raise KproError("No active contest.")
    lock_path(config).unlink()
    return data


def locate_problem(
    config: Config, problem: str, cwd: Path | None = None
) -> tuple[Path, dict | None]:
    current = (cwd or Path.cwd()).resolve()
    explicit = Path(problem)
    candidates = []
    if explicit.is_absolute() or "/" in problem or explicit.suffix == ".cpp":
        candidates += [
            explicit if explicit.is_absolute() else current / explicit,
            config.root / explicit,
        ]
    else:
        candidates += [current / problem / "main.cpp"]
        for parent in [current, *current.parents]:
            if parent == config.root.parent:
                break
            if (parent / "contest.toml").is_file():
                candidates.append(parent / problem / "main.cpp")
                break
        if state := active(config):
            candidates.append(within(config.root, state["workspace"]) / problem / "main.cpp")
    for path in candidates:
        if path.is_dir():
            path = path / "main.cpp"
        if not path.is_file():
            continue
        path = path.resolve()
        if not path.is_relative_to(config.root):
            raise KproError("Problem source must be inside the repository.")
        for parent in path.parents:
            if (parent / "contest.toml").is_file():
                return path, validate_metadata(parent / "contest.toml")
            if parent == config.root:
                break
        if path.is_relative_to(config.path("contest_root")):
            raise KproError(
                "Contest source has no contest.toml. Restore its snapshot metadata before bundling."
            )
        return path, None
    raise KproError(f"Cannot locate problem {problem}. Use a source path or start a contest first.")
