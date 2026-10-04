import pytest
from kpro.config import load
from kpro.core import KproError, write_text
from kpro.library import create, load_entry, validate_id, validate_tags


@pytest.mark.parametrize(
    "extra",
    [
        "\n[unknown]\nx = 1\n",
        "\n[cpp]\ninvalid = true\n",
        "\n[cpp]\ncompiler = 1\n",
        "\n[docs]\nport = true\n",
        "\n[docs]\nport = 0\n",
        '\n[verify]\nbackend = "fake"\n',
        '\n[project]\ndocs_root = "../docs"\n',
        "\n[cpp]\ncompile_flags = [1]\n",
    ],
)
def test_invalid_config(repository, extra):
    write_text(repository.root / "kpro.toml", extra)
    with pytest.raises(KproError):
        load(repository.root)


def test_defaults_and_unknown_key(repository):
    write_text(repository.root / "kpro.toml", "")
    config = load(repository.root)
    assert config.section("cpp")["standard"] == "c++23"
    assert config.section("docs")["host"] == "127.0.0.1"


@pytest.mark.parametrize(
    "entry_id", ["../escape", "graph/../../a", "/graph/a", "graph/a.hpp", "A", "graph/a b"]
)
def test_invalid_id(entry_id):
    with pytest.raises(KproError):
        validate_id(entry_id)


def test_lib_new_refuses_partial_overwrite(repository):
    paths = create(repository, "graph/new")
    original = paths[0].read_text()
    assert "TODO" in paths[1].read_text()
    assert "return 1" in paths[2].read_text()
    with pytest.raises(KproError, match="overwrite"):
        create(repository, "graph/new")
    assert paths[0].read_text() == original
    assert load_entry(repository, "graph/new").metadata["status"] == "experimental"


@pytest.mark.parametrize("tags", [["graphs"], "graph", [42]])
def test_bad_tags(tags, tmp_path):
    with pytest.raises(KproError):
        validate_tags(tags, {"graph"}, tmp_path)


@pytest.mark.parametrize(
    "replacement",
    [
        ("status: experimental", "status: verified"),
        ("tags: []", "tags: [graphs]"),
        ("source: include/cp/graph/new.hpp", "source: include/cp/graph/missing.hpp"),
        ("aliases:", "unknown:\n- bad\naliases:"),
    ],
)
def test_invalid_metadata(repository, replacement):
    _, doc, _ = create(repository, "graph/new")
    doc.write_text(doc.read_text().replace(*replacement))
    with pytest.raises(KproError):
        load_entry(repository, "graph/new")


def test_stable_requires_verification_and_completed_metadata(repository):
    _, doc, test = create(repository, "graph/new")
    doc.write_text(doc.read_text().replace("status: experimental", "status: stable"))
    with pytest.raises(KproError, match="Stable entry"):
        load_entry(repository, "graph/new")
    test.unlink()
    with pytest.raises(KproError, match="Missing/invalid"):
        load_entry(repository, "graph/new")
