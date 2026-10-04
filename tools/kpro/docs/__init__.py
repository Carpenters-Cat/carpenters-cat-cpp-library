from __future__ import annotations

import fcntl
import hashlib
import json
import posixpath
import re
import shutil
import tempfile
from pathlib import Path
from urllib.parse import unquote, urlsplit

import yaml

from kpro.config import Config
from kpro.core import KproError, write_bytes, write_json, write_text
from kpro.library import Entry, entries, front_matter, tag_registry, validate_tags


def fingerprint(config: Config, entry: Entry) -> str:
    # Include all canonical headers: dependency edits cannot leave an old green result.
    paths = {
        entry.doc,
        config.root / "kpro.toml",
        config.root / "zensical.toml",
        config.path("docs_root") / "tags.toml",
        *config.path("library_include_root").rglob("*.hpp"),
        *[path for group in entry.checks.values() for path in group],
        *Path(__file__).parents[1].rglob("*.py"),
    }
    digest = hashlib.sha256()
    for path in sorted(paths):
        label = (
            path.relative_to(config.root).as_posix()
            if path.is_relative_to(config.root)
            else path.name
        )
        digest.update(label.encode() + b"\0" + path.read_bytes() + b"\0")
    lock = config.root / "uv.lock"
    if lock.is_file():
        digest.update(lock.read_bytes())
    return digest.hexdigest()


def verification_state(config: Config, entry: Entry) -> str:
    path = config.root / ".kpro" / "verify" / f"{entry.id}.json"
    if not path.is_file():
        return "未検証"
    try:
        record = json.loads(path.read_text(encoding="utf-8"))
        if record["fingerprint"] != fingerprint(config, entry):
            return "変更あり・再検証が必要"
        return "検証済み" if record["result"] == "pass" else "検証失敗"
    except (KeyError, ValueError, TypeError):
        return "検証記録が無効・再検証が必要"


def validate_links(text: str, original: Path, root: Path) -> None:
    # Ignore fenced/inline code so API examples are not interpreted as links.
    stripped = re.sub(r"```.*?```|`[^`]*`", "", text, flags=re.S)
    for target in re.findall(r"!?\[[^\]]*\]\(([^\s)]+)(?:\s+[^)]*)?\)", stripped):
        target = target.strip("<>")
        parsed = urlsplit(target)
        if parsed.scheme or target.startswith(("#", "//")):
            continue
        link = unquote(parsed.path)
        path = root / link.lstrip("/") if link.startswith("/") else original.parent / link
        if link and not path.exists():
            raise KproError(f"Broken local link in {original}: {target}")


def generate(config: Config) -> Path:
    cache = config.root / ".kpro" / "docs"
    cache.mkdir(parents=True, exist_ok=True)
    output = cache / "source"
    # CLI search, verification and the running server may generate simultaneously.
    with (cache / "generation.lock").open("a") as lock:
        fcntl.flock(lock.fileno(), fcntl.LOCK_EX)
        with tempfile.TemporaryDirectory(prefix="source-", dir=cache) as directory:
            staging = Path(directory)
            index = _generate(config, staging)
            expected = set()
            for path in sorted(staging.rglob("*")):
                if not path.is_file():
                    continue
                relative = path.relative_to(staging)
                expected.add(relative)
                target = output / relative
                content = path.read_bytes()
                if not target.is_file() or target.read_bytes() != content:
                    write_bytes(target, content)
            for path in output.rglob("*"):
                if path.is_file() and path.relative_to(output) not in expected:
                    path.unlink()
            write_json(cache / "search.json", index)
        fcntl.flock(lock.fileno(), fcntl.LOCK_UN)
    return output


def _generate(config: Config, output: Path) -> list[dict]:
    library = entries(config)
    docs_root = config.path("docs_root")
    allowed_tags = tag_registry(config)
    index = []
    for path in sorted(docs_root.rglob("*")):
        if not path.is_file() or path.suffix == ".toml":
            continue
        if path.suffix == ".md":
            text = path.read_text(encoding="utf-8")
            validate_links(text, path, docs_root)
            if not path.is_relative_to(docs_root / "library"):
                metadata, body = front_matter(path) if text.startswith("---\n") else ({}, text)
                if "tags" in metadata:
                    validate_tags(metadata["tags"], allowed_tags, path)
                index.append(
                    {
                        "id": path.relative_to(docs_root).as_posix(),
                        "title": metadata.get("title", path.stem),
                        "status": "note",
                        "tags": metadata.get("tags", []),
                        "aliases": [],
                        "body": body,
                        "metadata": metadata,
                        "url": path.relative_to(docs_root).as_posix(),
                    }
                )
        target = output / path.relative_to(docs_root)
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, target)
    for entry in library:
        metadata = entry.metadata
        appendix = (
            "\n\n## Metadata\n\n"
            + f"Status: **{metadata['status']}**\n\nAliases: {', '.join(metadata['aliases'])}\n\nTags: {', '.join(metadata['tags'])}\n"
        )
        for field, title in (
            ("requires", "Preconditions"),
            ("complexity", "Complexity"),
            ("pitfalls", "Pitfalls"),
            ("related", "Related entries"),
        ):
            if value := metadata.get(field):
                appendix += f"\n## {title}\n\n"
                if isinstance(value, dict):
                    appendix += "\n".join(f"- {key}: {item}" for key, item in value.items()) + "\n"
                elif field == "related":
                    appendix += (
                        "\n".join(
                            f"- [{item}]({posixpath.relpath(item + '.md', Path(entry.id).parent.as_posix())})"
                            for item in value
                        )
                        + "\n"
                    )
                else:
                    appendix += "\n".join(f"- {item}" for item in value) + "\n"
        appendix += f"\n## Verification status\n\n{verification_state(config, entry)}\n\n"
        appendix += "\n".join(
            f"- {kind}: `{path.relative_to(config.root)}`"
            for kind, paths in entry.checks.items()
            for path in paths
        )
        code = entry.source.read_text(encoding="utf-8")
        fence = "`" * max(
            3, max((len(match[0]) + 1 for match in re.finditer(r"`+", code)), default=3)
        )
        appendix += f"\n\n## Full source code\n\n{fence}cpp\n{code.rstrip()}\n{fence}\n"
        relative = entry.doc.relative_to(docs_root)
        # Source blocks exist only in ignored generated Markdown.
        write_text(
            output / relative,
            "---\n"
            + yaml.safe_dump(metadata, allow_unicode=True, sort_keys=False)
            + "---\n"
            + entry.body
            + appendix,
        )
        index.append(
            {
                "id": entry.id,
                "title": metadata["title"],
                "aliases": metadata["aliases"],
                "tags": metadata["tags"],
                "status": metadata["status"],
                "body": entry.body + appendix.split("## Full source code")[0],
                "metadata": metadata,
                "url": relative.as_posix(),
            }
        )
    contest_root = config.path("contest_root")
    for note in sorted(contest_root.rglob("*.md")):
        if "build" in note.relative_to(contest_root).parts:
            continue
        metadata, body = front_matter(note)
        tags = validate_tags(metadata.get("tags", []), allowed_tags, note)
        validate_links(body, note, config.root)
        relative = Path("notes/contests") / note.relative_to(contest_root)
        # Mirror adjacent problem assets/notes so portable relative links keep working.
        target = output / relative
        write_text(
            target,
            note.read_text(encoding="utf-8")
            + "\n\nTags: "
            + ", ".join(tags)
            + "\n\n"
            + "\n".join(f"{key}: {value}" for key, value in metadata.items() if key != "tags"),
        )
        index.append(
            {
                "id": "contest/" + note.relative_to(contest_root).as_posix(),
                "title": metadata.get(
                    "title",
                    f"{metadata.get('contest', note.parent.name)} {metadata.get('problem', '')}".strip(),
                ),
                "aliases": [],
                "tags": tags,
                "status": "note",
                "body": body,
                "metadata": metadata,
                "url": relative.as_posix(),
            }
        )
    tag_page = "# Tags\n\n"
    for tag in sorted(allowed_tags):
        matching = [
            item
            for item in index
            if any(value == tag or value.startswith(tag + "/") for value in item["tags"])
        ]
        if matching:
            tag_page += (
                f"## {tag}\n\n"
                + "\n".join(
                    f"- [{item['title']}]({item['url']}) ({item['status']})" for item in matching
                )
                + "\n\n"
            )
    write_text(output / "tags.md", tag_page)
    return index


def search(config: Config, query: str) -> list[dict]:
    if not query.strip():
        raise KproError("Provide a nonempty search query.")
    generate(config)
    data = json.loads((config.root / ".kpro" / "docs" / "search.json").read_text(encoding="utf-8"))
    terms = query.casefold().split()
    ranked = []
    for item in data:
        title = item["title"].casefold()
        aliases = [alias.casefold() for alias in item["aliases"]]
        tags = " ".join(item["tags"]).casefold()
        text = (
            item["body"] + " " + yaml.safe_dump(item["metadata"], allow_unicode=True)
        ).casefold()
        if all(
            any(term in field for field in (title, " ".join(aliases), tags, text)) for term in terms
        ):
            score = sum(
                100 * (term == title or term in aliases)
                + 20 * (term in title or any(term in alias for alias in aliases))
                + 10 * (term in tags)
                + (term in text)
                for term in terms
            )
            ranked.append((score, item))
    return [item for _, item in sorted(ranked, key=lambda pair: (-pair[0], pair[1]["id"]))]
