from __future__ import annotations

import json
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Protocol

from kpro.config import Config
from kpro.core import KproError, run


@dataclass(frozen=True)
class Problem:
    id: str
    title: str
    url: str


@dataclass(frozen=True)
class Contest:
    id: str
    platform: str
    url: str
    problems: list[Problem]


class JudgeBackend(Protocol):
    def fetch_contest(self, identifier: str) -> Contest: ...
    def fetch_problem(self, url: str) -> Problem: ...
    def download_samples(self, problem: Problem, directory: Path) -> None: ...
    def test(self, problem: Problem, executable: Path, directory: Path) -> None: ...
    def submit(self, problem: Problem, source_file: Path, language: str | None = None) -> None: ...


class OjNgBackend:
    def __init__(self, config: Config):
        self.config = config

    def _get(self, command: str, url: str) -> dict:
        result = run(
            ["oj-api", command, url],
            cwd=self.config.root,
            check=False,
            capture_output=True,
            timeout=120,
        )
        try:
            data = json.loads(result.stdout)
            if result.returncode or data["status"] != "ok" or not isinstance(data["result"], dict):
                raise ValueError("backend failed")
            return data["result"]
        except (ValueError, KeyError, TypeError) as exc:
            raise KproError(
                f"oj-api could not fetch {url}. Check URL/network or run `oj login https://atcoder.jp/`. Backend diagnostics are suppressed to protect cookies."
            ) from exc

    def fetch_contest(self, identifier: str) -> Contest:
        match = re.fullmatch(r"(?:https://atcoder\.jp/contests/)?([a-z0-9_-]+)/?", identifier)
        if not match:
            raise KproError("Use an AtCoder contest id or https://atcoder.jp/contests/<id>.")
        contest_id = match[1]
        url = f"https://atcoder.jp/contests/{contest_id}"
        data = self._get("get-contest", url)
        try:
            problems = [
                Problem(item["context"]["alphabet"].lower(), item["name"], item["url"])
                for item in data["problems"]
            ]
        except (KeyError, TypeError, AttributeError) as exc:
            raise KproError(
                "Unexpected oj-api contest metadata. Check installed online-judge-api-client-ng version."
            ) from exc
        return Contest(contest_id, "atcoder", url, problems)

    def fetch_problem(self, url: str) -> Problem:
        data = self._get("get-problem", url)
        try:
            return Problem(data["context"]["alphabet"].lower(), data["name"], data["url"])
        except (KeyError, TypeError, AttributeError) as exc:
            raise KproError("Unexpected oj-api problem metadata.") from exc

    def download_samples(self, problem: Problem, directory: Path) -> None:
        directory.mkdir(parents=True, exist_ok=True)
        run(
            ["oj", "download", problem.url, "--directory", str(directory)],
            cwd=self.config.root,
            timeout=120,
        )

    def test(self, problem: Problem, executable: Path, directory: Path) -> None:
        from kpro.contest.runner import test_samples

        test_samples(self.config, executable, directory)

    def submit(self, problem: Problem, source_file: Path, language: str | None = None) -> None:
        args = ["oj", "submit", problem.url, str(source_file), "--yes"]
        if language:
            args += ["--language", language]
        run(args, cwd=self.config.root)


def backend(config: Config) -> JudgeBackend:
    if config.section("judge")["backend"] == "disabled":
        raise KproError(
            "Judge backend is disabled. Set judge.backend = 'oj-ng' for contest retrieval/submission."
        )
    return OjNgBackend(config)
