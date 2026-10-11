import shutil

import pytest
from kpro.adapters.compiler import has_header
from kpro.config import Config
from kpro.contest import active, end, locate_problem, lock_path, start, validate_metadata
from kpro.contest.runner import compile_problem, problem_data, submit
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


@pytest.mark.parametrize("bits_available", [False, True])
def test_generated_main_aliases_bundle_and_compile(repository, judge, monkeypatch, bits_available):
    fallback = repository.root / "include/cp/utility/template.hpp"
    if bits_available:
        # Supply bits on libc++ hosts too; the real compiler probe must find it.
        search = repository.root / ".kpro/test-headers"
        (search / "bits").mkdir(parents=True)
        shutil.copy(fallback, search / "bits/stdc++.h")
        repository = Config(
            repository.root,
            repository.values
            | {
                "cpp": repository.section("cpp")
                | {"compile_flags": [*repository.section("cpp")["compile_flags"], f"-I{search}"]}
            },
        )
        assert has_header(repository, "bits/stdc++.h")
    else:
        monkeypatch.setattr("kpro.contest.has_header", lambda *args: False)
    state, directory = begin(repository, judge)
    source = directory / "main.cpp"
    text = source.read_text()
    expected_header = "bits/stdc++.h" if bits_available else "cp/utility/template.hpp"
    assert text.startswith(f"#include <{expected_header}>\n\n")
    assert text.count("#include") == 1
    # No using declarations live in the fallback header. Break the working-tree
    # copy after start to exercise the immutable snapshot used for compilation.
    assert "using " not in fallback.read_text()
    write_text(fallback, "BROKEN WORKING TREE TEMPLATE\n")
    write_text(
        source,
        text.replace(
            "    return 0;",
            """    vi integers{3, 1, 2};
    ranges::sort(integers);
    vvi rows{integers};
    vll longs{10, 20};
    vvll long_rows{longs};
    pii index{0, 0};
    pll sums{accumulate(rows[index.first].begin(), rows[index.first].end(), 0LL),
             accumulate(long_rows[index.second].begin(), long_rows[index.second].end(), 0LL)};
    ull mask = 7;
    ld offset = 3.0L;
    ll answer = sums.first + sums.second + popcount(mask) + static_cast<ll>(offset);
    cout << answer << '\\n';
    return 0;""",
        ),
    )
    _, executable = compile_problem(repository, str(source))
    run_samples(repository, executable, directory / "tests")
    bundled = (directory / "build/submit.cpp").read_text()
    assert "BROKEN WORKING TREE" not in bundled
    if not bits_available:
        assert f"commit: {state['base_commit']}" in bundled
        assert "// begin cp/utility/template.hpp" in bundled


def test_dirty_start_never_calls_judge(repository, judge):
    write_text(repository.root / "dirty.txt", "changed")
    with pytest.raises(KproError, match="dirty"):
        start(repository, "abc999", judge)
    assert git(repository.root, "branch", "--show-current") == "main"
    assert not lock_path(repository).exists()


@pytest.mark.parametrize("custom_root", [False, True])
def test_contest_git_add_keeps_only_authored_files_and_settings(repository, judge, custom_root):
    if custom_root:
        repository = Config(
            repository.root,
            repository.values
            | {"project": repository.section("project") | {"contest_root": "practice"}},
        )
    _, directory = begin(repository, judge)
    write_text(directory / ".local/statement.md", "Downloaded statement")
    write_text(directory / "statement.html", "Downloaded HTML")
    write_text(directory / "tests/generated.md", "Downloaded explanation")
    write_text(directory / "notes.md", "My observations")
    write_text(directory / "solution.py", "print(42)")
    git(repository.root, "add", ".")
    tracked = set(git(repository.root, "ls-files", "--", str(directory.parent)).splitlines())
    prefix = directory.parent.relative_to(repository.root).as_posix()
    assert tracked == {
        f"{prefix}/.gitignore",
        f"{prefix}/contest.toml",
        f"{prefix}/notes.md",
        f"{prefix}/a/main.cpp",
        f"{prefix}/a/notes.md",
        f"{prefix}/a/solution.py",
        f"{prefix}/a/problem.toml",
    }
    assert "title" not in (directory / "problem.toml").read_text()
    assert problem_data(directory / "main.cpp").title == "a"
    assert (directory / "tests/sample-1.out").read_text() == "42\n"


def test_legacy_problem_title_remains_readable(repository, judge):
    _, directory = begin(repository, judge)
    path = directory / "problem.toml"
    write_text(path, path.read_text() + 'title = "Legacy title"\n')
    assert problem_data(directory / "main.cpp").title == "Legacy title"


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


@pytest.mark.parametrize("delete_later", [False, True])
@pytest.mark.parametrize("override", [False, True])
@pytest.mark.parametrize("contest_root", ["contests", "practice sessions"])
def test_push_guard_rejects_force_added_samples_in_outgoing_history(
    repository, judge, tmp_path, delete_later, override, contest_root, monkeypatch
):
    repository = Config(
        repository.root,
        repository.values
        | {"project": repository.section("project") | {"contest_root": contest_root}},
    )
    remote = tmp_path / "remote.git"
    git(repository.root, "init", "--bare", str(remote))
    git(repository.root, "remote", "add", "test", str(remote))
    git(repository.root, "push", "test", "HEAD:refs/heads/main")
    _, directory = begin(repository, judge)
    if override:
        monkeypatch.setenv("KPRO_ALLOW_PUSH", "1")
    else:
        end(repository)
    git(repository.root, "add", ".")
    git(repository.root, "add", "-f", str(directory / "tests/sample-1.in"))
    git(repository.root, "commit", "-m", "Test accidental sample commit")
    if delete_later:
        git(repository.root, "rm", str(directory / "tests/sample-1.in"))
        git(repository.root, "commit", "-m", "Test sample deletion")
    result = run(
        ["git", "push", "test", "HEAD:refs/heads/contest"],
        cwd=repository.root,
        capture_output=True,
        check=False,
    )
    assert result.returncode != 0
    assert "local-only files" in result.stderr
    assert "sample-1.in" in result.stderr
    assert not git(remote, "show-ref", "refs/heads/contest", check=False)


def test_push_guard_rejects_samples_without_metadata(repository, tmp_path):
    remote = tmp_path / "remote.git"
    git(repository.root, "init", "--bare", str(remote))
    git(repository.root, "remote", "add", "test", str(remote))
    path = repository.path("contest_root") / "2026/old/a/tests/sample.in"
    write_text(path, "downloaded sample")
    git(repository.root, "add", "-f", str(path))
    git(repository.root, "commit", "-m", "Test sample without contest metadata")
    result = run(
        ["git", "push", "test", "HEAD:refs/heads/contest"],
        cwd=repository.root,
        capture_output=True,
        check=False,
    )
    assert result.returncode != 0
    assert "sample.in" in result.stderr


def test_push_guard_checks_contest_branch_when_main_is_checked_out(repository, judge, tmp_path):
    config = Config(
        repository.root,
        repository.values
        | {"project": repository.section("project") | {"contest_root": "practice sessions"}},
    )
    remote = tmp_path / "remote.git"
    git(config.root, "init", "--bare", str(remote))
    git(config.root, "remote", "add", "test", str(remote))
    _, directory = begin(config, judge)
    end(config)
    git(config.root, "add", ".")
    git(config.root, "add", "-f", str(directory / "tests/sample-1.in"))
    git(config.root, "commit", "-m", "Test sample on another branch")
    git(config.root, "switch", "main")
    assert not (directory.parent / ".gitignore").exists()
    result = run(
        ["git", "push", "test", "contest/abc999:refs/heads/contest"],
        cwd=config.root,
        capture_output=True,
        check=False,
    )
    assert result.returncode != 0
    assert "sample-1.in" in result.stderr


def test_push_guard_accepts_solution_commit(repository, judge, tmp_path):
    remote = tmp_path / "remote.git"
    git(repository.root, "init", "--bare", str(remote))
    git(repository.root, "remote", "add", "test", str(remote))
    _, directory = begin(repository, judge)
    end(repository)
    git(repository.root, "add", ".")
    git(repository.root, "commit", "-m", "Test authored solution commit")
    git(repository.root, "push", "test", "HEAD:refs/heads/contest")
    assert git(remote, "rev-parse", "refs/heads/contest") == head(repository.root)


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
