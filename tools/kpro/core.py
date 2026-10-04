from __future__ import annotations

import json
import logging
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

LOG = logging.getLogger("kpro")


class KproError(Exception):
    """An actionable error suitable for command-line output."""


def executable(name: str) -> str | None:
    found = shutil.which(name)
    if found:
        return found
    sibling = Path(sys.executable).parent / name
    return str(sibling) if sibling.is_file() and os.access(sibling, os.X_OK) else None


def run(args: list[str], *, cwd: Path, check: bool = True, **kwargs) -> subprocess.CompletedProcess:
    # Do not log backend arguments: they may contain URLs or authentication information.
    LOG.debug("Executing %s in %s", args[0], cwd)
    try:
        result = subprocess.run(
            [executable(args[0]) or args[0], *args[1:]], cwd=cwd, text=True, **kwargs
        )
    except FileNotFoundError as exc:
        raise KproError(
            f"Executable not found: {args[0]}. Run `uv sync` or check PATH/configuration."
        ) from exc
    except subprocess.TimeoutExpired as exc:
        raise KproError(
            f"Command timed out: {args[0]}. Check the program or configured timeout."
        ) from exc
    if check and result.returncode:
        detail = (getattr(result, "stderr", None) or "").strip()
        raise KproError(
            f"{args[0]} failed (exit {result.returncode})." + (f"\n{detail}" if detail else "")
        )
    return result


def git(root: Path, *args: str, check: bool = True) -> str:
    return run(["git", *args], cwd=root, check=check, capture_output=True).stdout.strip()


def head(root: Path) -> str | None:
    return git(root, "rev-parse", "--verify", "HEAD", check=False) or None


def within(root: Path, relative: str) -> Path:
    path = root / relative
    if Path(relative).is_absolute() or not path.resolve().is_relative_to(root.resolve()):
        raise KproError(f"Path must stay inside {root}: {relative}")
    return path


def write_text(path: Path, content: str) -> None:
    write_bytes(path, content.encode("utf-8"))


def write_bytes(path: Path, content: bytes) -> None:
    """Atomically replace derived state; never leave a half-written lock/result."""
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(mode="wb", dir=path.parent, delete=False) as stream:
        temporary = Path(stream.name)
        stream.write(content)
    try:
        os.replace(temporary, path)
    finally:
        temporary.unlink(missing_ok=True)


def write_json(path: Path, content: object) -> None:
    write_text(path, json.dumps(content, ensure_ascii=False, indent=2) + "\n")
