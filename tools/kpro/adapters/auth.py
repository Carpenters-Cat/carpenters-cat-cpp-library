from __future__ import annotations

import getpass
import http.cookiejar
import os
import tempfile
import warnings
from pathlib import Path
from urllib.parse import urlsplit

from kpro.config import Config
from kpro.core import KproError, run

ATCODER_URL = "https://atcoder.jp/"


def cookie_path(config: Config) -> Path:
    return config.root / ".kpro" / "auth" / "atcoder.cookie.jar"


def cookie_args(config: Config, url: str = ATCODER_URL) -> list[str]:
    if urlsplit(url).hostname != "atcoder.jp":
        return []
    path = cookie_path(config)
    # Preserve existing oj login sessions until a repository-local session is imported.
    return ["--cookie", str(path)] if path.is_file() else []


def _check(config: Config, args: list[str]) -> None:
    result = run(
        ["oj", *args, "login", "--check", ATCODER_URL],
        cwd=config.root,
        check=False,
        capture_output=True,
        timeout=120,
    )
    if result.returncode:
        raise KproError(
            "AtCoder login check failed. Check the network; sign in again in your normal "
            "browser and import a fresh REVEL_SESSION with `uv run kpro login`. "
            "Backend diagnostics are suppressed to protect cookies."
        )


def login(config: Config, *, check: bool = False) -> None:
    if config.section("judge")["backend"] != "oj-ng":
        raise KproError("AtCoder login requires judge.backend = 'oj-ng'.")
    if check:
        _check(config, cookie_args(config))
        return

    print(
        "Sign in at https://atcoder.jp/login in your normal browser.\n"
        "In DevTools, open Application/Storage > Cookies > https://atcoder.jp, "
        "then copy the REVEL_SESSION value."
    )
    # Fail instead of falling back to an echoed prompt when no terminal is available.
    with warnings.catch_warnings():
        warnings.simplefilter("error", getpass.GetPassWarning)
        try:
            value = getpass.getpass("REVEL_SESSION (hidden): ").strip()
        except (getpass.GetPassWarning, EOFError) as exc:
            raise KproError("Run `uv run kpro login` in an interactive terminal.") from exc
    if not value or any(character.isspace() or ord(character) < 32 for character in value):
        raise KproError("Paste only the nonempty REVEL_SESSION value.")

    path = cookie_path(config)
    path.parent.mkdir(parents=True, exist_ok=True, mode=0o700)
    with tempfile.NamedTemporaryFile(dir=path.parent, delete=False) as stream:
        temporary = Path(stream.name)
    try:
        jar = http.cookiejar.LWPCookieJar(str(temporary))
        jar.set_cookie(
            http.cookiejar.Cookie(
                version=0,
                name="REVEL_SESSION",
                value=value,
                port=None,
                port_specified=False,
                domain="atcoder.jp",
                domain_specified=True,
                domain_initial_dot=False,
                path="/",
                path_specified=True,
                secure=True,
                expires=None,
                discard=True,
                comment=None,
                comment_url=None,
                rest={"HttpOnly": None},
            )
        )
        jar.save(ignore_discard=True)
        _check(config, ["--cookie", str(temporary)])
        # An unsuccessful import must never replace a working session.
        temporary.chmod(0o600)
        os.replace(temporary, path)
    finally:
        temporary.unlink(missing_ok=True)
