import io
import json
import shutil
import subprocess
from pathlib import Path
from urllib.error import URLError

import pytest
from kpro.adapters.auth import cookie_path
from kpro.adapters.judge import OjNgBackend, Problem
from kpro.adapters.judge_sites import resolve_contest, resolve_problem
from kpro.cli import parser
from kpro.contest import active, end, lock_path, start, validate_metadata
from kpro.contest.runner import compile_problem, submit
from kpro.contest.runner import test_samples as run_samples
from kpro.core import KproError, git, head, write_text


def oj_contest(platform="yukicoder", number="603"):
    contest_url = f"https://yukicoder.me/contests/{number}"
    problem_url = "https://yukicoder.me/problems/13012"
    if platform == "atcoder":
        contest_url = f"https://atcoder.jp/contests/{number}"
        problem_url = f"{contest_url}/tasks/{number}_a"
    return {
        "url": contest_url,
        "name": "Contest title",
        "problems": [{"name": "Problem title", "url": problem_url, "context": {"alphabet": "A"}}],
    }


def cf_contest(number=1234):
    return {
        "status": "OK",
        "result": {
            "contest": {"id": number, "name": "Contest title"},
            "problems": [
                {"contestId": number, "index": index, "name": f"Problem {index}"}
                for index in ("A", "B1", "B2")
            ],
            "rows": [],
        },
    }


def mock_cf(monkeypatch, data):
    calls = []

    def request(req, **kwargs):
        calls.append((req, kwargs))
        return io.BytesIO(json.dumps(data).encode())

    monkeypatch.setattr("kpro.adapters.judge_sites.urlopen", request)
    return calls


@pytest.mark.parametrize(
    "identifier, platform, contest_id, url",
    [
        ("abc350", "atcoder", "abc350", "https://atcoder.jp/contests/abc350"),
        ("atcoder:abc350", "atcoder", "abc350", "https://atcoder.jp/contests/abc350"),
        (
            "https://atcoder.jp/contests/abc350/",
            "atcoder",
            "abc350",
            "https://atcoder.jp/contests/abc350",
        ),
        (
            "codeforces:001234",
            "codeforces",
            "codeforces-1234",
            "https://codeforces.com/contest/1234",
        ),
        (
            "https://codeforces.com/contest/1234",
            "codeforces",
            "codeforces-1234",
            "https://codeforces.com/contest/1234",
        ),
        ("yukicoder:603", "yukicoder", "yukicoder-603", "https://yukicoder.me/contests/603"),
        (
            "http://yukicoder.me/contests/603/",
            "yukicoder",
            "yukicoder-603",
            "https://yukicoder.me/contests/603",
        ),
    ],
)
def test_contest_identifiers(identifier, platform, contest_id, url):
    reference = resolve_contest(identifier)
    assert (reference.platform, reference.id, reference.url) == (platform, contest_id, url)
    assert parser().parse_args(["contest", "start", identifier]).identifier == identifier


@pytest.mark.parametrize(
    "identifier",
    [
        "1234",
        "codeforces:abc",
        "yukicoder:0",
        "codeforces:-1234",
        "other:1234",
        "https://codeforces.com/gym/1234",
        "https://codeforces.com/group/secret/contest/1234",
        "https://codeforces.com/contest/1234/problem/A",
        "https://evil.test/contest/1234",
        "https://codeforces.com@evil.test/contest/1234",
        "https://yukicoder.me/contests/603?key=secret",
        "https://atcoder.jp/contests/../tasks",
        "https://[broken/",
        "codeforces:" + "9" * 4500,
        "yukicoder:" + "9" * 4500,
    ],
)
def test_invalid_contest_never_requests_network(repository, monkeypatch, identifier):
    def unexpected(*args, **kwargs):
        pytest.fail("Invalid identifier must fail before network/backend access")

    monkeypatch.setattr("kpro.adapters.judge.run", unexpected)
    monkeypatch.setattr("kpro.adapters.judge_sites.urlopen", unexpected)
    with pytest.raises(KproError):
        OjNgBackend(repository).fetch_contest(identifier)


def test_codeforces_uses_public_api_and_preserves_split_problem_ids(repository, monkeypatch):
    write_text(cookie_path(repository), "AtCoder-only jar")
    calls = mock_cf(monkeypatch, cf_contest())
    monkeypatch.setattr("kpro.adapters.judge.run", lambda *a, **k: pytest.fail("Do not use oj-api"))
    backend = OjNgBackend(repository)
    contest = backend.fetch_contest("codeforces:1234")
    assert contest.id == "codeforces-1234"
    assert [problem.id for problem in contest.problems] == ["a", "b1", "b2"]
    assert contest.problems[1].url == "https://codeforces.com/contest/1234/problem/B1"
    req, kwargs = calls[0]
    assert req.full_url == "https://codeforces.com/api/contest.standings?contestId=1234"
    assert req.get_header("Cookie") is None
    assert kwargs == {"timeout": 120}
    problem = backend.fetch_problem("https://codeforces.com/problemset/problem/1234/B2")
    assert problem == contest.problems[2]
    with pytest.raises(KproError, match="not in"):
        backend.fetch_problem("https://codeforces.com/contest/1234/problem/Z")


@pytest.mark.parametrize(
    "data",
    [
        {"status": "FAILED", "comment": "Private"},
        {},
        [],
        {"status": "OK", "result": {"contest": {"id": 1234}, "problems": []}},
        {
            "status": "OK",
            "result": {"contest": {"id": 999}, "problems": [{"index": "A", "name": "A"}]},
        },
        {
            "status": "OK",
            "result": {"contest": {"id": 1234}, "problems": [{"index": "../A", "name": "A"}]},
        },
        {
            "status": "OK",
            "result": {
                "contest": {"id": 1234},
                "problems": [{"index": "A", "name": "A", "contestId": 999}],
            },
        },
        {
            "status": "OK",
            "result": {"contest": {"id": 1234}, "problems": [{"index": "A", "name": None}]},
        },
        {
            "status": "OK",
            "result": {
                "contest": {"id": 1234},
                "problems": [{"index": "A", "name": "A"}, {"index": "a", "name": "a"}],
            },
        },
    ],
)
def test_codeforces_unavailable_or_malformed_metadata(repository, monkeypatch, data):
    mock_cf(monkeypatch, data)
    with pytest.raises(KproError, match="metadata"):
        OjNgBackend(repository).fetch_contest("codeforces:1234")


@pytest.mark.parametrize(
    "failure", [URLError("offline"), TimeoutError("timeout"), b"<html>challenge</html>"]
)
def test_codeforces_network_errors_are_actionable(repository, monkeypatch, failure):
    def request(*args, **kwargs):
        if isinstance(failure, bytes):
            return io.BytesIO(failure)
        raise failure

    monkeypatch.setattr("kpro.adapters.judge_sites.urlopen", request)
    with pytest.raises(KproError, match="Check URL/network"):
        OjNgBackend(repository).fetch_contest("codeforces:1234")


@pytest.mark.parametrize("platform, number", [("atcoder", "abc350"), ("yukicoder", "603")])
def test_oj_contest_and_cookie_routing(repository, monkeypatch, platform, number):
    write_text(cookie_path(repository), "AtCoder-only jar")
    calls = []

    def command(args, **kwargs):
        calls.append(args)
        return subprocess.CompletedProcess(
            args, 0, json.dumps({"status": "ok", "result": oj_contest(platform, number)}), ""
        )

    monkeypatch.setattr("kpro.adapters.judge.run", command)
    contest = OjNgBackend(repository).fetch_contest(f"{platform}:{number}")
    assert contest.platform == platform
    assert contest.problems[0].id == "a"
    assert contest.problems[0].title == "Problem title"
    assert ("--cookie" in calls[0]) is (platform == "atcoder")
    assert calls[0][-2:] == ["get-contest", contest.url]


@pytest.mark.parametrize(
    "url, expected_id",
    [
        ("https://yukicoder.me/problems/no/3538", "no-3538"),
        ("https://yukicoder.me/problems/13012", "id-13012"),
    ],
)
def test_yukicoder_standalone_problem_has_no_alphabet_or_title(
    repository, monkeypatch, url, expected_id
):
    monkeypatch.setattr(OjNgBackend, "_get", lambda *args: {"url": url, "tests": []})
    problem = OjNgBackend(repository).fetch_problem(url)
    assert problem == Problem(expected_id, expected_id, url)


@pytest.mark.parametrize("mutation", ["missing", "id", "title", "site", "duplicate", "context"])
def test_invalid_oj_metadata_is_rejected(repository, monkeypatch, mutation):
    data = oj_contest()
    if mutation == "missing":
        data = {}
    elif mutation == "id":
        data["problems"][0]["context"]["alphabet"] = "../escape"
    elif mutation == "title":
        data["problems"][0]["name"] = None
    elif mutation == "site":
        data["problems"][0]["url"] = "https://evil.test/problem"
    elif mutation == "duplicate":
        data["problems"] *= 2
    else:
        data["problems"][0]["context"] = None
    monkeypatch.setattr(OjNgBackend, "_get", lambda *args: data)
    with pytest.raises(KproError, match="metadata"):
        OjNgBackend(repository).fetch_contest("yukicoder:603")


@pytest.mark.parametrize("platform", ["atcoder", "yukicoder"])
def test_oj_failure_suppresses_diagnostics_and_names_correct_login(
    repository, monkeypatch, platform
):
    monkeypatch.setattr(
        "kpro.adapters.judge.run",
        lambda args, **kw: subprocess.CompletedProcess(args, 1, "cookie-secret", "cookie-secret"),
    )
    identifier = "abc350" if platform == "atcoder" else "yukicoder:603"
    with pytest.raises(KproError) as error:
        OjNgBackend(repository).fetch_contest(identifier)
    assert "cookie-secret" not in str(error.value)
    assert ("kpro login" in str(error.value)) is (platform == "atcoder")


@pytest.mark.parametrize("platform", ["codeforces", "yukicoder"])
def test_samples_and_submission_capability_are_site_specific(repository, monkeypatch, platform):
    write_text(cookie_path(repository), "AtCoder-only jar")
    url = (
        "https://codeforces.com/contest/1234/problem/A"
        if platform == "codeforces"
        else "https://yukicoder.me/problems/13012"
    )
    problem = Problem("a", "A", url)
    calls = []

    def command(args, **kwargs):
        calls.append(args)
        return subprocess.CompletedProcess(args, 0, "", "")

    monkeypatch.setattr("kpro.adapters.judge.run", command)
    backend = OjNgBackend(repository)
    backend.download_samples(problem, repository.root / ".kpro/samples")
    assert "--cookie" not in calls[0]
    assert calls[0][1:3] == ["download", url]
    source = repository.root / "build/submit.cpp"
    assert backend.supports_submission(problem) is (platform == "yukicoder")
    if platform == "yukicoder":
        backend.submit(problem, source, "cpp23")
        assert calls[-1] == ["oj", "submit", url, str(source), "--yes", "--language", "cpp23"]
    else:
        with pytest.raises(KproError, match="Codeforces"):
            backend.submit(problem, source)
        assert len(calls) == 1


def sample_command(args, **kwargs):
    if "get-contest" in args:
        return subprocess.CompletedProcess(
            args, 0, json.dumps({"status": "ok", "result": oj_contest(number="1234")}), ""
        )
    assert "download" in args
    directory = args[args.index("--directory") + 1]
    write_text(Path(directory) / "sample-1.in", "")
    write_text(Path(directory) / "sample-1.out", "42\n")
    return subprocess.CompletedProcess(args, 0, "", "")


def test_site_ids_do_not_collide_and_contests_keep_snapshots(repository, monkeypatch):
    mock_cf(monkeypatch, cf_contest())
    monkeypatch.setattr("kpro.adapters.judge.run", sample_command)
    backend = OjNgBackend(repository)
    base = head(repository.root)
    header = repository.root / "include/cp/graph/b.hpp"
    original_header = header.read_text()
    states = []
    for identifier in ("codeforces:1234", "yukicoder:1234"):
        state = start(repository, identifier, backend)
        states.append(state)
        workspace = repository.root / state["workspace"]
        assert validate_metadata(workspace / "contest.toml")["base_commit"] == base
        assert git(repository.root, "rev-parse", state["snapshot_tag"]) == base
        assert active(repository)["contest"] == state["contest"]
        assert (workspace / "a/problem.toml").is_file()
        assert (workspace / "notes.md").is_file()
        write_text(
            workspace / "a/main.cpp",
            "#include <cp/graph/a.hpp>\n#include <iostream>\nint main() { std::cout << answer; }\n",
        )
        write_text(header, "BROKEN POST CONTEST EDIT\n")
        source, executable = compile_problem(repository, "a")
        run_samples(repository, executable, source.parent / "tests")
        write_text(header, original_header)
        end(repository)
        git(repository.root, "switch", "main")
        # Isolated test fixture cleanup so the second contest can start cleanly.
        shutil.rmtree(workspace)
    assert states[0]["workspace"] != states[1]["workspace"]
    assert {state["branch"] for state in states} == {
        "contest/codeforces-1234",
        "contest/yukicoder-1234",
    }


@pytest.mark.parametrize("identifier", ["codeforces:1234", "yukicoder:1234"])
def test_site_download_failure_creates_no_branch_tag_or_lock(repository, monkeypatch, identifier):
    mock_cf(monkeypatch, cf_contest())

    def command(args, **kwargs):
        if "download" in args:
            raise KproError("download failed")
        return sample_command(args, **kwargs)

    monkeypatch.setattr("kpro.adapters.judge.run", command)
    with pytest.raises(KproError, match="download failed"):
        start(repository, identifier, OjNgBackend(repository))
    assert git(repository.root, "branch", "--show-current") == "main"
    assert not git(repository.root, "tag", "--list")
    assert not lock_path(repository).exists()
    assert not list(repository.path("contest_root").rglob("contest.toml"))


@pytest.mark.parametrize("force", [False, True])
def test_codeforces_submit_guides_bundling_without_compilation_or_submission(
    repository, monkeypatch, force
):
    mock_cf(monkeypatch, cf_contest())
    monkeypatch.setattr("kpro.adapters.judge.run", sample_command)
    backend = OjNgBackend(repository)
    start(repository, "codeforces:1234", backend)
    monkeypatch.setattr(
        "kpro.contest.runner.compile_cpp",
        lambda *a, **k: pytest.fail("Unsupported submission must fail early"),
    )
    with pytest.raises(KproError, match="kpro bundle a"):
        submit(repository, "a", backend, force=force)


def test_yukicoder_submission_compiles_and_checks_snapshot_before_fake_judge(
    repository, monkeypatch
):
    calls = []

    def command(args, **kwargs):
        if "submit" in args:
            calls.append(args)
            return subprocess.CompletedProcess(args, 0, "", "")
        return sample_command(args, **kwargs)

    monkeypatch.setattr("kpro.adapters.judge.run", command)
    backend = OjNgBackend(repository)
    state = start(repository, "yukicoder:1234", backend)
    workspace = repository.root / state["workspace"]
    write_text(
        workspace / "a/main.cpp",
        "#include <cp/graph/a.hpp>\n#include <iostream>\nint main() { std::cout << answer; }\n",
    )
    write_text(repository.root / "include/cp/graph/b.hpp", "BROKEN POST CONTEST EDIT\n")
    bundled = submit(repository, "a", backend, language="cpp23")
    assert len(calls) == 1
    assert str(bundled) in calls[0]
    assert calls[0][-2:] == ["--language", "cpp23"]
    assert "answer = 42" in bundled.read_text()
    assert "BROKEN" not in bundled.read_text()
    write_text(workspace / "a/main.cpp", "int main() {}\n")
    with pytest.raises(KproError, match="Sample tests failed"):
        submit(repository, "a", backend, language="cpp23")
    assert len(calls) == 1


def test_yukicoder_number_and_internal_id_are_distinct():
    assert (
        resolve_problem("https://yukicoder.me/problems/no/123").id
        != resolve_problem("https://yukicoder.me/problems/123").id
    )
