from __future__ import annotations

import argparse
import logging
import shutil
import sys

from kpro import __version__
from kpro.adapters.compiler import compile_cpp
from kpro.adapters.documentation import build, serve
from kpro.adapters.judge import backend
from kpro.bundler import bundle_file
from kpro.config import Config, load
from kpro.contest import active, end, locate_problem, start
from kpro.contest.runner import compile_problem, submit, test_samples
from kpro.core import KproError, git, run, write_text
from kpro.core import executable as find_executable
from kpro.docs import search
from kpro.library import create, entries, load_entry
from kpro.verify import check


def parser() -> argparse.ArgumentParser:
    result = argparse.ArgumentParser(
        prog="kpro", description="C++ library, verification, documentation and contest workflow"
    )
    result.add_argument("--version", action="version", version=f"kpro {__version__}")
    result.add_argument("--verbose", action="store_true")
    commands = result.add_subparsers(dest="command", required=True)
    commands.add_parser("doctor")
    lib = commands.add_parser("lib").add_subparsers(dest="action", required=True)
    new = lib.add_parser("new")
    new.add_argument("id")
    new.add_argument("--force", action="store_true")
    lib.add_parser("check").add_argument("id")
    verify = commands.add_parser("verify")
    verify.add_argument("id", nargs="?")
    docs = commands.add_parser("docs")
    docs.add_argument("action", nargs="?", choices=["build"])
    docs.add_argument("--host")
    docs.add_argument("--port", type=int)
    commands.add_parser("search").add_argument("query")
    contest = commands.add_parser("contest").add_subparsers(dest="action", required=True)
    contest.add_parser("start").add_argument("identifier")
    contest.add_parser("status")
    contest.add_parser("end")
    for name in ("run", "test", "bundle", "submit"):
        command = commands.add_parser(name)
        command.add_argument("problem")
        if name == "submit":
            command.add_argument("--force", action="store_true")
            command.add_argument("--language")
    return result


def doctor(config: Config) -> bool:
    failures = []

    def report(name: str, valid: bool, detail: str = ""):
        print(f"{'PASS' if valid else 'FAIL'} {name}" + (f": {detail}" if detail else ""))
        if not valid:
            failures.append(name)

    report("configuration", True, str(config.root / "kpro.toml"))
    report("Git", bool(shutil.which("git")))
    root = git(config.root, "rev-parse", "--show-toplevel", check=False)
    report("repository", bool(root), root or "Run git init")
    compiler = config.section("cpp")["compiler"]
    found = shutil.which(compiler)
    report("compiler", bool(found), found or "Install a C++ compiler or edit cpp.compiler")
    if found:
        version = run([compiler, "--version"], cwd=config.root, capture_output=True)
        report("compiler version", True, version.stdout.splitlines()[0])
        source = config.root / ".kpro" / "doctor" / "standard.cpp"
        minimum = {"c++17": 201703, "c++20": 202002, "c++23": 202100}[
            config.section("cpp")["standard"]
        ]
        write_text(
            source,
            f"#include <version>\n#if __cplusplus < {minimum}L\n#error Configured standard required\n#endif\nint main() {{}}\n",
        )
        try:
            compile_cpp(config, source, source.parent / "standard")
            report("C++ standard", True, config.section("cpp")["standard"])
        except KproError as exc:
            report("C++ standard", False, str(exc))
    dependencies = ["zensical"]
    if config.section("verify")["backend"] == "competitive-verifier":
        dependencies.append("competitive-verifier")
    if config.section("judge")["backend"] != "disabled":
        dependencies += ["oj", "oj-api"]
    for dependency in dependencies:
        report(dependency, bool(find_executable(dependency)), "Run uv sync if missing")
    for key in config.section("project"):
        directory = config.path(key)
        report(key, directory.is_dir(), str(directory))
    configured = git(config.root, "config", "--get", "core.hooksPath", check=False)
    report(
        "Git hooks",
        configured in {".githooks", str(config.root / ".githooks")}
        and (config.root / ".githooks" / "pre-push").is_file()
        and bool((config.root / ".githooks" / "pre-push").stat().st_mode & 0o111),
        "git config core.hooksPath .githooks",
    )
    if root:
        state = active(config)
        report("contest lock", True, state["contest"] if state else "inactive")
        if not git(config.root, "rev-parse", "--verify", "HEAD", check=False):
            print(
                "INFO No initial commit: library verification works; contest start needs a commit."
            )
    return not failures


def dispatch(config: Config, args: argparse.Namespace) -> int:
    match args.command:
        case "doctor":
            return 0 if doctor(config) else 1
        case "lib":
            if args.action == "new":
                for path in create(config, args.id, args.force):
                    print(f"Created {path.relative_to(config.root)}")
                return 0
            load_entry(config, args.id)
            return 0 if check(config, args.id) else 1
        case "verify":
            if args.id:
                selected = [load_entry(config, args.id)]
            else:
                selected = entries(config)
            if not selected:
                raise KproError(
                    "No library entries. Create one with `kpro lib new <category/name>`."
                )
            outcomes = [check(config, entry.id) for entry in selected]
            print(f"Verification: {sum(outcomes)}/{len(outcomes)} entries passed.")
            return 0 if all(outcomes) else 1
        case "docs":
            build(config) if args.action == "build" else serve(config, args.host, args.port)
        case "search":
            matches = search(config, args.query)
            for entry in matches:
                print(
                    f"{entry['id']} [{entry['status']}] {entry['title']}\n  tags: {', '.join(entry['tags'])}\n  docs: {entry['url']}"
                )
            if not matches:
                print("No matches.")
        case "contest":
            if args.action == "start":
                state = start(config, args.identifier, backend(config))
                push_state = (
                    "active"
                    if config.section("contest")["disable_push"]
                    else "disabled by configuration"
                )
                print(
                    f"Started {state['contest']}\nSnapshot: {state['base_commit']}\nWorkspace: {state['workspace']}\nBranch: {state['branch']}\nPush lock: {push_state}"
                )
            elif args.action == "status":
                state = active(config)
                print(
                    f"Contest: {state['contest']}\nSnapshot: {state['base_commit']}\nWorkspace: {state['workspace']}\nBranch: {state['branch']}\nStarted: {state['started_at']}"
                    if state
                    else "No active contest."
                )
            else:
                state = end(config)
                print(
                    f"Ended {state['contest']}. Push lock removed. Preserved {state['workspace']} and {state['branch']}."
                )
                if git(config.root, "status", "--porcelain"):
                    print("Uncommitted work remains; review contest notes before committing.")
        case "bundle":
            source, metadata = locate_problem(config, args.problem)
            if not metadata:
                print("Non-contest bundle: using working-tree library headers.")
            print(bundle_file(config, source, metadata).relative_to(config.root))
        case "run" | "test":
            source, executable = compile_problem(config, args.problem)
            if args.command == "run":
                return run([str(executable)], cwd=source.parent, check=False).returncode
            test_samples(config, executable, source.parent / "tests")
        case "submit":
            submitted = submit(
                config, args.problem, backend(config), force=args.force, language=args.language
            )
            print(f"Submitted {submitted.relative_to(config.root)}")
    return 0


def main(argv: list[str] | None = None) -> int:
    args = parser().parse_args(argv)
    logging.basicConfig(
        level=logging.DEBUG if args.verbose else logging.WARNING, format="%(levelname)s %(message)s"
    )
    try:
        return dispatch(load(), args)
    except KproError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 1
    except OSError as exc:
        print(f"ERROR: {exc}. Check file paths and permissions.", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        return 130
