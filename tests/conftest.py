import shutil
from pathlib import Path

import pytest
from kpro.adapters.judge import Contest, Problem
from kpro.config import load
from kpro.core import git, write_text

PROJECT = Path(__file__).parents[1]


class FakeJudge:
    def __init__(self):
        self.submissions = []
        self.fail_download = False

    def fetch_contest(self, identifier):
        return Contest(
            "abc999",
            "atcoder",
            "https://atcoder.jp/contests/abc999",
            [Problem("a", "A", "https://atcoder.jp/contests/abc999/tasks/abc999_a")],
        )

    def fetch_problem(self, url):
        return Problem("a", "A", url)

    def download_samples(self, problem, directory):
        if self.fail_download:
            raise RuntimeError("download failed")
        write_text(directory / "sample-1.in", "")
        write_text(directory / "sample-1.out", "42\n")

    def submit(self, problem, source_file, language=None):
        self.submissions.append((problem, source_file.read_text(), language))


@pytest.fixture
def repository(tmp_path):
    root = tmp_path / "repo"
    root.mkdir()
    for name in ("kpro.toml", ".gitignore", "zensical.toml"):
        shutil.copy(PROJECT / name, root / name)
    shutil.copytree(PROJECT / ".githooks", root / ".githooks")
    (root / ".githooks/pre-push").chmod(0o755)
    for name in (
        "include/cp",
        "docs/library",
        "verify/unit",
        "verify/stress",
        "verify/online",
        "contests",
        ".kpro",
    ):
        (root / name).mkdir(parents=True, exist_ok=True)
    shutil.copy(PROJECT / "docs/tags.toml", root / "docs/tags.toml")
    write_text(root / "docs/index.md", "# Test library\n")
    write_text(root / "include/cp/graph/b.hpp", "#pragma once\ninline constexpr int answer = 42;\n")
    write_text(
        root / "include/cp/graph/a.hpp",
        "#pragma once\n#include <cp/graph/b.hpp>\n#include <cp/graph/b.hpp>\n",
    )
    git(root, "init", "-b", "main")
    git(root, "config", "user.name", "Test User")
    git(root, "config", "user.email", "test@example.invalid")
    git(root, "config", "core.hooksPath", ".githooks")
    git(root, "add", ".")
    git(root, "commit", "-m", "Initial test snapshot")
    return load(root)


@pytest.fixture
def judge():
    return FakeJudge()
