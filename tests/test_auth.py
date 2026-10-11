import http.cookiejar
import subprocess

import pytest
from kpro.adapters.auth import cookie_args, cookie_path, login
from kpro.adapters.judge import OjNgBackend, Problem
from kpro.cli import parser
from kpro.core import KproError, git, write_text


def test_browser_cookie_import_is_private_and_used_by_all_backend_commands(
    repository, monkeypatch, capsys
):
    secret = "test-session-secret"
    monkeypatch.setattr("kpro.adapters.auth.getpass.getpass", lambda prompt: secret)
    calls = []

    def check_session(args, **kwargs):
        calls.append(args)
        assert secret not in " ".join(args)
        assert kwargs["capture_output"]
        path = args[args.index("--cookie") + 1]
        jar = http.cookiejar.LWPCookieJar(path)
        jar.load(ignore_discard=True)
        cookies = list(jar)
        assert len(cookies) == 1
        assert cookies[0].value == secret
        assert cookies[0].domain == "atcoder.jp"
        assert cookies[0].secure
        return subprocess.CompletedProcess(args, 0, "", "")

    monkeypatch.setattr("kpro.adapters.auth.run", check_session)
    login(repository)
    path = cookie_path(repository)
    assert path.stat().st_mode & 0o777 == 0o600
    assert path.parent.stat().st_mode & 0o777 == 0o700
    assert git(repository.root, "check-ignore", str(path))
    assert secret not in capsys.readouterr().out
    assert list(path.parent.iterdir()) == [path]
    login(repository, check=True)
    assert calls[-1][1:3] == ["--cookie", str(path)]

    def backend_command(args, **kwargs):
        calls.append(args)
        return subprocess.CompletedProcess(args, 0, '{"status":"ok","result":{}}', "")

    monkeypatch.setattr("kpro.adapters.judge.run", backend_command)
    backend = OjNgBackend(repository)
    problem = Problem("a", "A", "https://atcoder.jp/contests/abc999/tasks/abc999_a")
    backend._get("get-problem", problem.url)
    backend.download_samples(problem, repository.root / ".kpro/samples")
    backend.submit(problem, repository.root / "build/submit.cpp")
    for args in calls[-3:]:
        assert args[1:3] == ["--cookie", str(path)]
        assert secret not in " ".join(args)


@pytest.mark.parametrize("failure", ["rejected", "timeout"])
def test_failed_import_preserves_previous_session(repository, monkeypatch, capsys, failure):
    path = cookie_path(repository)
    write_text(path, "previous working cookie jar")
    secret = "test-secret-never-log"
    monkeypatch.setattr("kpro.adapters.auth.getpass.getpass", lambda prompt: secret)

    def reject(args, **kwargs):
        if failure == "timeout":
            raise KproError("Command timed out: oj.")
        return subprocess.CompletedProcess(args, 1, secret, secret)

    monkeypatch.setattr("kpro.adapters.auth.run", reject)
    with pytest.raises(KproError) as error:
        login(repository)
    assert secret not in str(error.value)
    assert secret not in capsys.readouterr().out
    assert path.read_text() == "previous working cookie jar"
    assert list(path.parent.iterdir()) == [path]


def test_default_oj_session_remains_available(repository, monkeypatch):
    assert cookie_args(repository) == []
    calls = []

    def check_session(args, **kwargs):
        calls.append(args)
        return subprocess.CompletedProcess(args, 0, "", "")

    monkeypatch.setattr("kpro.adapters.auth.run", check_session)
    login(repository, check=True)
    assert calls == [["oj", "login", "--check", "https://atcoder.jp/"]]
    assert not cookie_path(repository).exists()


def test_nonterminal_import_does_not_echo_cookie(repository, monkeypatch):
    import getpass
    import warnings

    def no_terminal(prompt):
        warnings.warn("Cannot control echo", getpass.GetPassWarning)
        pytest.fail("Must stop before a plain input prompt")

    monkeypatch.setattr("kpro.adapters.auth.getpass.getpass", no_terminal)
    with pytest.raises(KproError, match="interactive terminal"):
        login(repository)
    assert not cookie_path(repository).exists()


def test_login_command_parser():
    assert parser().parse_args(["login"]).check is False
    assert parser().parse_args(["login", "--check"]).check is True
