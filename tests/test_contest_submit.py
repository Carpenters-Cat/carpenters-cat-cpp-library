import pytest
from kpro.config import Config
from kpro.contest import active, end, locate_problem, lock_path, start, validate_metadata
from kpro.contest.runner import compile_problem, submit
from kpro.contest.runner import test_samples as run_samples
from kpro.core import KproError, git, head, run, write_text


def begin(config, judge):
    state = start(config, "abc999", judge)
    directory = config.root / state["workspace"] / "a"
    return state, directory


def test_start_end_records_snapshot_preserves_files(repository, judge):
    commit = head(repository.root)
    state, directory = begin(repository, judge)
    metadata = validate_metadata(directory.parent / "contest.toml")
    assert metadata["base_commit"] == commit
    assert git(repository.root, "rev-parse", metadata["snapshot_tag"]) == commit
    assert git(repository.root, "branch", "--show-current") == "contest/abc999"
    assert active(repository)["contest"] == "abc999"
    assert (directory / "tests/sample-1.in").exists()
    assert locate_problem(repository, "a")[0] == directory / "main.cpp"
    end(repository)
    assert not lock_path(repository).exists()
    assert directory.exists()
    assert git(repository.root, "branch", "--show-current") == "contest/abc999"


def test_dirty_start_never_calls_judge(repository, judge):
    write_text(repository.root / "dirty.txt", "changed")
    with pytest.raises(KproError, match="dirty"):
        start(repository, "abc999", judge)
    assert git(repository.root, "branch", "--show-current") == "main"
    assert not lock_path(repository).exists()


def test_explicit_push_policy_keeps_contest_lifecycle(repository, judge):
    config = Config(
        repository.root,
        repository.values | {"contest": repository.section("contest") | {"disable_push": False}},
    )
    begin(config, judge)
    assert active(config)["disable_push"] is False
    result = run(
        [str(repository.root / ".githooks/pre-push")],
        cwd=repository.root,
        check=False,
        capture_output=True,
    )
    assert result.returncode == 0
    end(config)
    assert not lock_path(config).exists()


def test_failed_download_does_not_create_branch_tag_or_lock(repository, judge):
    judge.fail_download = True
    with pytest.raises(RuntimeError, match="download failed"):
        start(repository, "abc999", judge)
    assert git(repository.root, "branch", "--show-current") == "main"
    assert not git(repository.root, "tag", "--list")
    assert not lock_path(repository).exists()
    assert not list(repository.path("contest_root").rglob("contest.toml"))


def test_push_guard(repository, judge, tmp_path):
    remote = tmp_path / "remote.git"
    git(repository.root, "init", "--bare", str(remote))
    git(repository.root, "remote", "add", "test", str(remote))
    git(repository.root, "push", "test", "HEAD:refs/heads/main")
    begin(repository, judge)
    result = run(
        ["git", "push", "test", "HEAD:refs/heads/contest"],
        cwd=repository.root,
        capture_output=True,
        check=False,
    )
    assert result.returncode != 0
    assert "contest mode is active" in result.stderr
    end(repository)
    git(repository.root, "push", "test", "HEAD:refs/heads/contest")


@pytest.mark.parametrize("broken", ["bundle", "compile", "sample", "missing-samples"])
def test_submission_failures_never_invoke_backend(repository, judge, broken):
    _, directory = begin(repository, judge)
    content = "#include <iostream>\nint main() { std::cout << 42; }\n"
    if broken == "bundle":
        content = "#include <cp/graph/missing.hpp>\nint main() {}\n"
    elif broken == "compile":
        content = "THIS DOES NOT COMPILE\n"
    elif broken == "sample":
        content = "#include <iostream>\nint main() { std::cout << 43; }\n"
    elif broken == "missing-samples":
        (directory / "tests/sample-1.in").unlink()
    write_text(directory / "main.cpp", content)
    with pytest.raises(KproError):
        submit(repository, "a", judge)
    assert not judge.submissions


def test_passing_submission_checks_exact_snapshot_bundle(repository, judge):
    _, directory = begin(repository, judge)
    write_text(
        directory / "main.cpp",
        "#include <cp/graph/a.hpp>\n#include <iostream>\nint main() { std::cout << answer; }\n",
    )
    write_text(repository.root / "include/cp/graph/b.hpp", "BROKEN POST CONTEST EDIT\n")
    source, executable = compile_problem(repository, "a")
    run_samples(repository, executable, source.parent / "tests")
    bundled = submit(repository, "a", judge, language="test-language")
    assert len(judge.submissions) == 1
    assert "answer = 42" in judge.submissions[0][1]
    assert "BROKEN" not in judge.submissions[0][1]
    assert bundled.read_text() == judge.submissions[0][1]
    assert judge.submissions[0][2] == "test-language"


def test_invalid_contest_metadata(repository, judge):
    _, directory = begin(repository, judge)
    path = directory.parent / "contest.toml"
    path.write_text(path.read_text().replace(head(repository.root), "main"))
    with pytest.raises(KproError, match="base_commit"):
        validate_metadata(path)


def test_contest_missing_metadata_cannot_fall_back_to_worktree(repository):
    path = repository.path("contest_root") / "2026/broken/a/main.cpp"
    write_text(path, "int main() {}\n")
    with pytest.raises(KproError, match="no contest.toml"):
        locate_problem(repository, str(path))
