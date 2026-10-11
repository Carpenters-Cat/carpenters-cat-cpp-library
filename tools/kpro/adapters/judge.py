from __future__ import annotations

import json
from pathlib import Path
from typing import Protocol
from urllib.parse import urlsplit

from kpro.adapters.auth import cookie_args
from kpro.adapters.judge_sites import SITES, resolve_contest, resolve_problem
from kpro.adapters.judge_sites import Contest as Contest
from kpro.adapters.judge_sites import Problem as Problem
from kpro.config import Config
from kpro.core import KproError, run


class JudgeBackend(Protocol):
    def fetch_contest(self, identifier: str) -> Contest: ...
    def fetch_problem(self, url: str) -> Problem: ...
    def download_samples(self, problem: Problem, directory: Path) -> None: ...
    def test(self, problem: Problem, executable: Path, directory: Path) -> None: ...
    def supports_submission(self, problem: Problem) -> bool: ...
    def submit(self, problem: Problem, source_file: Path, language: str | None = None) -> None: ...


class OjNgBackend:
    def __init__(self, config: Config):
        self.config = config

    def _get(self, command: str, url: str) -> dict:
        result = run(
            ["oj-api", *cookie_args(self.config, url), command, url],
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
            hint = (
                "Run `uv run kpro login` to import an AtCoder browser session."
                if urlsplit(url).hostname == "atcoder.jp"
                else "Check the URL/network and your oj login session."
            )
            raise KproError(
                f"oj-api could not fetch {url}. {hint} Backend diagnostics are suppressed to protect cookies."
            ) from exc

    def fetch_contest(self, identifier: str) -> Contest:
        reference = resolve_contest(identifier)
        return SITES[reference.platform].fetch_contest(reference, self._get)

    def fetch_problem(self, url: str) -> Problem:
        reference = resolve_problem(url)
        return SITES[reference.platform].fetch_problem(reference, self._get)

    def download_samples(self, problem: Problem, directory: Path) -> None:
        reference = resolve_problem(problem.url)
        directory.mkdir(parents=True, exist_ok=True)
        run(
            [
                "oj",
                *cookie_args(self.config, reference.url),
                "download",
                reference.url,
                "--directory",
                str(directory),
            ],
            cwd=self.config.root,
            timeout=120,
        )

    def test(self, problem: Problem, executable: Path, directory: Path) -> None:
        from kpro.contest.runner import test_samples

        test_samples(self.config, executable, directory)

    def supports_submission(self, problem: Problem) -> bool:
        return SITES[resolve_problem(problem.url).platform].supports_submission

    def submit(self, problem: Problem, source_file: Path, language: str | None = None) -> None:
        reference = resolve_problem(problem.url)
        if not self.supports_submission(problem):
            raise KproError(
                "Automatic submission to Codeforces is unavailable. Use `kpro bundle` and submit the generated source in your browser."
            )
        args = [
            "oj",
            *cookie_args(self.config, reference.url),
            "submit",
            reference.url,
            str(source_file),
            "--yes",
        ]
        if language:
            args += ["--language", language]
        run(args, cwd=self.config.root)


def backend(config: Config) -> JudgeBackend:
    if config.section("judge")["backend"] == "disabled":
        raise KproError(
            "Judge backend is disabled. Set judge.backend = 'oj-ng' for contest retrieval/submission."
        )
    return OjNgBackend(config)
