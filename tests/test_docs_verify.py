import json
import subprocess

import pytest
import yaml
from kpro.adapters import verifier
from kpro.config import Config
from kpro.core import KproError, write_json, write_text
from kpro.docs import fingerprint, generate, search, verification_state
from kpro.library import create, load_entry
from kpro.verify import check


def implemented_entry(config):
    source, doc, test = create(config, "graph/example")
    write_text(source, "#pragma once\ninline constexpr int example = 42;\n")
    write_text(
        test,
        "#include <cp/graph/example.hpp>\n#include <cassert>\nint main() {assert(example == 42);}\n",
    )
    metadata = {
        "title": "Example",
        "source": "include/cp/graph/example.hpp",
        "status": "stable",
        "tags": ["graph/connectivity"],
        "aliases": ["連結成分", "Example"],
        "verification": {"unit": ["verify/unit/graph/example.test.cpp"]},
    }
    write_text(
        doc,
        "---\n"
        + yaml.safe_dump(metadata, allow_unicode=True)
        + "---\n\n# Example\n\nAn example with a documented API.\n",
    )
    return load_entry(config, "graph/example")


def test_generated_source_search_tags_and_contest_notes(repository):
    entry = implemented_entry(repository)
    note = repository.path("contest_root") / "2026/abc999/notes.md"
    write_text(
        note,
        "---\ncontest: abc999\nproblem: F\nresult: WA\ntags: [graph/connectivity, mistake/overflow]\n---\n\n# Notes\n\n整数のオーバーフローで失敗した。\n",
    )
    output = generate(repository)
    page = (output / "library/graph/example.md").read_text()
    assert entry.source.read_text().strip() in page
    assert "inline constexpr" not in entry.doc.read_text()
    assert "graph/connectivity" in (output / "tags.md").read_text()
    assert search(repository, "連結成分")[0]["id"] == "graph/example"
    assert {item["id"] for item in search(repository, "graph/connectivity")} == {
        "graph/example",
        "contest/2026/abc999/notes.md",
    }
    assert search(repository, "オーバーフロー")[0]["id"].startswith("contest/")


def test_broken_document_link_fails(repository):
    implemented_entry(repository)
    output = generate(repository)
    previous = (output / "index.md").read_bytes()
    write_text(repository.path("docs_root") / "bad.md", "# Broken\n\n[bad](missing.md)\n")
    with pytest.raises(KproError, match="Broken local link"):
        generate(repository)
    assert (output / "index.md").read_bytes() == previous


def test_successful_verification_record_becomes_stale(repository, monkeypatch):
    entry = implemented_entry(repository)
    monkeypatch.setattr("kpro.adapters.documentation.build", lambda config: generate(config))
    assert check(repository, entry.id)
    record_path = repository.root / ".kpro/verify/graph/example.json"
    record = json.loads(record_path.read_text())
    assert record["git_commit"]
    assert record["dirty"]
    assert record["timestamp"]
    assert all(item["result"] == "pass" for item in record["checks"])
    assert verification_state(repository, entry) == "検証済み"
    entry.source.write_text(entry.source.read_text().replace("42", "43"))
    assert fingerprint(repository, entry) != record["fingerprint"]
    assert "再検証" in verification_state(repository, entry)
    assert not check(repository, entry.id)
    assert json.loads(record_path.read_text())["result"] == "fail"


def test_ndebug_cannot_disable_verification_assertions(repository):
    entry = implemented_entry(repository)
    entry.source.write_text(entry.source.read_text().replace("42", "43"))
    config = Config(
        repository.root,
        repository.values
        | {"cpp": repository.section("cpp") | {"compile_flags": ["-O2", "-DNDEBUG"]}},
    )
    with pytest.raises(KproError):
        verifier.local_check(config, entry.checks["unit"][0], repository.root / ".kpro/test-ndebug")


def test_source_edit_during_verification_cannot_be_green(repository, monkeypatch):
    entry = implemented_entry(repository)

    def mutate_after_test(*args):
        entry.source.write_text(entry.source.read_text().replace("42", "43"))

    monkeypatch.setattr("kpro.verify.local_check", mutate_after_test)
    monkeypatch.setattr("kpro.adapters.documentation.build", lambda config: generate(config))
    assert not check(repository, entry.id)
    record = json.loads((repository.root / ".kpro/verify/graph/example.json").read_text())
    assert any(
        item["type"] == "sources unchanged during verification" and item["result"] == "fail"
        for item in record["checks"]
    )


@pytest.mark.parametrize(
    "result",
    [
        {"files": {}},
        {"files": {"verify/online/graph/example.test.cpp": {"verifications": []}}},
        {
            "files": {
                "verify/online/graph/example.test.cpp": {
                    "verifications": [{"status": "success", "testcases": []}]
                }
            }
        },
    ],
)
def test_empty_online_success_is_rejected(repository, monkeypatch, result):
    source = repository.path("verify_root") / "online/graph/example.test.cpp"
    write_text(
        source,
        "// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/aplusb\nint main() {}\n",
    )
    cache = repository.root / ".kpro/online-fake"
    cache.mkdir()
    monkeypatch.setattr(verifier, "compile_cpp", lambda *args: cache / "test")

    def fake_run(*args, **kwargs):
        write_json(cache / "result.json", result)
        return subprocess.CompletedProcess([], 0)

    monkeypatch.setattr(verifier, "run", fake_run)
    with pytest.raises(KproError):
        verifier.online_check(repository, [source], cache)
