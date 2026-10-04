from __future__ import annotations

import re
import tomllib
from dataclasses import dataclass
from pathlib import Path

from kpro.core import KproError, within

DEFAULTS = {
    "project": {
        "library_include_root": "include",
        "contest_root": "contests",
        "docs_root": "docs",
        "verify_root": "verify",
    },
    "cpp": {
        "compiler": "g++",
        "standard": "c++23",
        "compile_flags": ["-O2", "-Wall", "-Wextra"],
        "include_flags": ["-Iinclude"],
    },
    "docs": {"engine": "zensical", "host": "127.0.0.1", "port": 8000},
    "verify": {"backend": "competitive-verifier", "timeout": 30},
    "judge": {"backend": "oj-ng"},
    "contest": {"base_branch": "main", "disable_push": True, "record_snapshot_tag": True},
    "bundle": {"library_prefix": "cp/", "emit_provenance_comment": True},
}


def read_toml(path: Path) -> dict:
    try:
        with path.open("rb") as stream:
            return tomllib.load(stream)
    except (OSError, tomllib.TOMLDecodeError) as exc:
        raise KproError(f"Cannot read TOML {path}: {exc}") from exc


def find_root(start: Path | None = None) -> Path:
    location = (start or Path.cwd()).resolve()
    for path in [location, *location.parents]:
        if (path / "kpro.toml").is_file():
            return path
    raise KproError("Cannot find kpro.toml. Run kpro inside the library repository.")


@dataclass(frozen=True)
class Config:
    root: Path
    values: dict

    def section(self, name: str) -> dict:
        return self.values[name]

    def path(self, name: str) -> Path:
        return within(self.root, self.values["project"][name])


def load(root: Path | None = None) -> Config:
    root = (root or find_root()).resolve()
    data = read_toml(root / "kpro.toml")
    unknown = set(data) - set(DEFAULTS)
    if unknown:
        raise KproError(f"Unknown configuration sections: {', '.join(sorted(unknown))}")
    values = {}
    for section, defaults in DEFAULTS.items():
        supplied = data.get(section, {})
        if not isinstance(supplied, dict):
            raise KproError(f"[{section}] must be a TOML table.")
        unknown = set(supplied) - set(defaults)
        if unknown:
            raise KproError(f"Unknown keys in [{section}]: {', '.join(sorted(unknown))}")
        values[section] = defaults | supplied
        for key, value in values[section].items():
            template = defaults[key]
            valid = type(value) is type(template)
            if isinstance(template, list):
                valid = valid and all(isinstance(item, str) and item for item in value)
            elif isinstance(template, str):
                valid = valid and bool(value.strip()) and "\n" not in value
            if not valid:
                raise KproError(f"Invalid {section}.{key}: expected {type(template).__name__}.")
    for name in values["project"]:
        configured = values["project"][name]
        if configured == "." or ".." in Path(configured).parts:
            raise KproError(f"project.{name} must be a repository subdirectory.")
        within(root, configured)
    for section, key, allowed in [
        ("cpp", "standard", {"c++17", "c++20", "c++23"}),
        ("docs", "engine", {"zensical"}),
        ("verify", "backend", {"competitive-verifier", "local"}),
        ("judge", "backend", {"oj-ng", "disabled"}),
    ]:
        if values[section][key] not in allowed:
            raise KproError(
                f"Unsupported {section}.{key}; choose from {', '.join(sorted(allowed))}."
            )
    if not 1 <= values["docs"]["port"] <= 65535:
        raise KproError("docs.port must be between 1 and 65535.")
    if values["verify"]["timeout"] <= 0:
        raise KproError("verify.timeout must be positive.")
    if not re.fullmatch(r"[a-zA-Z0-9_-]+(?:/[a-zA-Z0-9_-]+)*/", values["bundle"]["library_prefix"]):
        raise KproError("bundle.library_prefix must be an include prefix ending in / (e.g. cp/).")
    return Config(root, values)
