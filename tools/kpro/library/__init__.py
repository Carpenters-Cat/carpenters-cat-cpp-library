from __future__ import annotations

import re
from dataclasses import dataclass
from pathlib import Path

import yaml

from kpro.config import Config, read_toml
from kpro.core import KproError, within

ID_PATTERN = r"[a-z][a-z0-9_]*(?:/[a-z][a-z0-9_]*)+"


def validate_id(entry_id: str) -> str:
    if not re.fullmatch(ID_PATTERN, entry_id):
        raise KproError(f"Invalid library id: {entry_id}. Use e.g. data_structure/union_find.")
    return entry_id


def front_matter(path: Path) -> tuple[dict, str]:
    try:
        text = path.read_text(encoding="utf-8")
    except OSError as exc:
        raise KproError(f"Cannot read documentation {path}: {exc}") from exc
    match = re.match(r"\A---\r?\n(.*?)\r?\n---(?:\r?\n|$)(.*)\Z", text, re.S)
    if not match:
        raise KproError(f"Missing YAML front matter: {path}")
    try:
        metadata = yaml.safe_load(match[1])
    except yaml.YAMLError as exc:
        raise KproError(f"Invalid YAML front matter in {path}: {exc}") from exc
    if not isinstance(metadata, dict):
        raise KproError(f"Front matter must be a mapping: {path}")
    return metadata, match[2]


def tag_registry(config: Config) -> set[str]:
    data = read_toml(config.path("docs_root") / "tags.toml")
    tags = data.get("tags")
    if (
        set(data) != {"tags"}
        or not isinstance(tags, list)
        or not all(
            isinstance(tag, str) and re.fullmatch(r"[a-z0-9-]+(?:/[a-z0-9-]+)*", tag)
            for tag in tags
        )
    ):
        raise KproError(
            'docs/tags.toml must contain tags = ["graph", ...] using lowercase hierarchical tags.'
        )
    if len(tags) != len(set(tags)):
        raise KproError("Duplicate tags in docs/tags.toml.")
    return set(tags)


def validate_tags(tags: object, allowed: set[str], path: Path) -> list[str]:
    if not isinstance(tags, list) or not all(isinstance(tag, str) for tag in tags):
        raise KproError(f"tags must be a list of strings: {path}")
    unknown = set(tags) - allowed
    if unknown:
        raise KproError(
            f"Unknown tags in {path}: {', '.join(sorted(unknown))}. Add intentional new tags to docs/tags.toml."
        )
    return tags


@dataclass(frozen=True)
class Entry:
    id: str
    source: Path
    doc: Path
    metadata: dict
    body: str
    checks: dict[str, list[Path]]


def load_entry(config: Config, entry_id: str) -> Entry:
    validate_id(entry_id)
    doc = config.path("docs_root") / "library" / f"{entry_id}.md"
    metadata, body = front_matter(doc)
    required = {"title", "source", "status", "tags", "aliases"}
    allowed = required | {"complexity", "requires", "pitfalls", "related", "verification"}
    if missing := required - set(metadata):
        raise KproError(f"Missing metadata in {doc}: {', '.join(sorted(missing))}")
    if unknown := set(metadata) - allowed:
        raise KproError(f"Unknown metadata in {doc}: {', '.join(sorted(unknown))}")
    for field in ("title", "source", "status"):
        if not isinstance(metadata[field], str) or not metadata[field].strip():
            raise KproError(f"{field} must be a nonempty string: {doc}")
    if metadata["status"] not in {"experimental", "stable", "deprecated"}:
        raise KproError(
            f"Invalid status in {doc}; use experimental, stable, or deprecated. Verification is derived."
        )
    validate_tags(metadata["tags"], tag_registry(config), doc)
    for key in ("aliases", "requires", "pitfalls", "related"):
        if key in metadata and (
            not isinstance(metadata[key], list)
            or not all(isinstance(item, str) and item.strip() for item in metadata[key])
        ):
            raise KproError(f"{key} must be a list of nonempty strings: {doc}")
    if "complexity" in metadata and (
        not isinstance(metadata["complexity"], dict)
        or not all(
            isinstance(k, str) and isinstance(v, str) for k, v in metadata["complexity"].items()
        )
    ):
        raise KproError(f"complexity must map operation names to strings: {doc}")
    source = within(config.root, metadata["source"])
    expected = (
        config.path("library_include_root")
        / config.section("bundle")["library_prefix"]
        / f"{entry_id}.hpp"
    )
    if source != expected or not source.is_file():
        raise KproError(
            f"source for {entry_id} must reference an existing canonical header: {expected.relative_to(config.root)}"
        )
    for related in metadata.get("related", []):
        validate_id(related)
        if not (config.path("docs_root") / "library" / f"{related}.md").is_file():
            raise KproError(f"Missing related entry {related} referenced by {entry_id}.")
    policy = metadata.get("verification", {})
    if not isinstance(policy, dict) or set(policy) - {"unit", "stress", "online"}:
        raise KproError(f"verification must contain only unit, stress, online lists: {doc}")
    checks = {}
    for kind in ("unit", "stress", "online"):
        listed = policy.get(kind, [])
        if not isinstance(listed, list) or not all(isinstance(item, str) for item in listed):
            raise KproError(f"verification.{kind} must be a list of paths: {doc}")
        paths = [within(config.root, item) for item in listed]
        if not listed:
            candidate = config.path("verify_root") / kind / f"{entry_id}.test.cpp"
            paths = [candidate] if candidate.is_file() else []
        for path in paths:
            if (
                not path.is_relative_to(config.path("verify_root") / kind)
                or path.suffix != ".cpp"
                or not path.is_file()
            ):
                raise KproError(f"Missing/invalid {kind} verification source: {path}")
        checks[kind] = paths
    if metadata["status"] == "stable":
        if not metadata["tags"] or not metadata["aliases"] or not any(checks.values()):
            raise KproError(
                f"Stable entry {entry_id} needs tags, aliases, and at least one configured verification test."
            )
        if "TODO" in body or "TODO" in yaml.safe_dump(metadata):
            raise KproError(f"Fill all TODO markers before marking {entry_id} stable.")
    return Entry(entry_id, source, doc, metadata, body, checks)


def entries(config: Config) -> list[Entry]:
    folder = config.path("docs_root") / "library"
    return [
        load_entry(config, path.relative_to(folder).with_suffix("").as_posix())
        for path in sorted(folder.rglob("*.md"))
    ]


def create(config: Config, entry_id: str, force: bool = False) -> list[Path]:
    validate_id(entry_id)
    source = (
        config.path("library_include_root")
        / config.section("bundle")["library_prefix"]
        / f"{entry_id}.hpp"
    )
    doc = config.path("docs_root") / "library" / f"{entry_id}.md"
    test = config.path("verify_root") / "unit" / f"{entry_id}.test.cpp"
    paths = [source, doc, test]
    for path in paths:
        within(config.root, path.relative_to(config.root).as_posix())
        if path.exists() and not force:
            raise KproError(f"Refusing to overwrite {path}. Choose another id or pass --force.")
    metadata = {
        "title": entry_id.split("/")[-1].replace("_", " ").title(),
        "source": source.relative_to(config.root).as_posix(),
        "status": "experimental",
        "tags": [],
        "aliases": ["TODO: aliases"],
        "requires": ["TODO: preconditions"],
        "complexity": {"query": "TODO: complexity"},
        "pitfalls": ["TODO: pitfalls"],
        "verification": {"unit": [test.relative_to(config.root).as_posix()]},
    }
    contents = [
        "#pragma once\n\nnamespace cp {\n// TODO: implementation\n}\n",
        "---\n"
        + yaml.safe_dump(metadata, allow_unicode=True, sort_keys=False)
        + "---\n\n# "
        + metadata["title"]
        + "\n\nTODO: summary\n\n<!-- TODO: tags in front matter (docs/tags.toml) -->\n\n## When to use\n\nTODO\n\n## API\n\nTODO\n\n## Examples\n\nTODO\n",
        f"// competitive-verifier: STANDALONE\n#include <{config.section('bundle')['library_prefix']}{entry_id}.hpp>\n\nint main() {{\n    // TODO: meaningful unit tests; fail until implemented.\n    return 1;\n}}\n",
    ]
    for path, content in zip(paths, contents, strict=True):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8")
    return paths
