import pytest
from kpro.adapters.compiler import has_header
from kpro.config import Config
from kpro.core import KproError, write_text


@pytest.mark.parametrize("flags", ["compile_flags", "include_flags"])
def test_header_probe_uses_submission_search_paths(repository, flags):
    directory = repository.root / ".kpro/custom-headers"
    write_text(directory / "kpro_probe.hpp", "#pragma once\n")
    assert has_header(repository, "vector")
    assert not has_header(repository, "kpro_probe.hpp")
    config = Config(
        repository.root,
        repository.values
        | {
            "cpp": repository.section("cpp")
            | {flags: [*repository.section("cpp")[flags], f"-I{directory}"]}
        },
    )
    assert has_header(config, "kpro_probe.hpp") is (flags == "compile_flags")


def test_header_probe_reports_compiler_errors(repository):
    config = Config(
        repository.root,
        repository.values
        | {"cpp": repository.section("cpp") | {"compile_flags": ["-fkpro-invalid-option"]}},
    )
    with pytest.raises(KproError, match="failed"):
        has_header(config, "bits/stdc++.h")
