from __future__ import annotations

import subprocess
import time

from kpro.config import Config, load
from kpro.core import KproError, executable, run
from kpro.docs import generate


def build(config: Config) -> None:
    generate(config)
    run(
        ["zensical", "build", "--strict", "--config-file", str(config.root / "zensical.toml")],
        cwd=config.root,
    )


def watched(config: Config) -> tuple:
    paths = [config.root / "kpro.toml", config.root / "zensical.toml"]
    for root in [
        config.path("docs_root"),
        config.path("library_include_root"),
        config.path("contest_root"),
        config.root / ".kpro" / "verify",
    ]:
        paths += [
            path
            for path in root.rglob("*")
            if path.is_file() and "build" not in path.relative_to(root).parts
        ]
    return tuple(
        (str(path), path.stat().st_mtime_ns, path.stat().st_size) for path in sorted(paths)
    )


def serve(config: Config, host: str | None = None, port: int | None = None) -> None:
    generate(config)
    host = host or config.section("docs")["host"]
    port = port or config.section("docs")["port"]
    if not 1 <= port <= 65535:
        raise KproError("Port must be between 1 and 65535.")
    address = f"[{host}]:{port}" if ":" in host else f"{host}:{port}"
    print(f"Documentation: http://{address}/", flush=True)
    try:
        process = subprocess.Popen(
            [
                executable("zensical") or "zensical",
                "serve",
                "--config-file",
                str(config.root / "zensical.toml"),
                "--dev-addr",
                address,
            ],
            cwd=config.root,
        )
    except FileNotFoundError as exc:
        raise KproError("Zensical is unavailable. Run `uv sync`.") from exc
    previous = watched(config)
    try:
        while process.poll() is None:
            time.sleep(0.5)
            current = watched(config)
            if current != previous:
                try:
                    config = load(config.root)
                    generate(config)
                except KproError as exc:
                    print(f"Documentation update failed: {exc}", flush=True)
                previous = current
        if process.returncode:
            raise KproError(f"Zensical server failed (exit {process.returncode}).")
    finally:
        if process.poll() is None:
            process.terminate()
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
