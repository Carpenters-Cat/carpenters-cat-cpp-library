import pytest
from kpro.adapters.compiler import compile_cpp
from kpro.bundler import directives, expand, provenance
from kpro.core import KproError, head, write_text


def solution(config, text):
    path = config.root / "main.cpp"
    write_text(path, text)
    return path


def test_include_parser_ignores_comments_and_raw_strings():
    source = """// #include <cp/no.hpp>
/*
#include <cp/no.hpp>
*/
const char* s = R"foo(
#include <cp/no.hpp>
)foo";
  # include <cp/graph/a.hpp> // trailing comment
#include "cp/graph/b.hpp"
#include <vector>
"""
    assert list(directives(source).values()) == ["cp/graph/a.hpp", "cp/graph/b.hpp", "vector"]


def test_snapshot_recursive_dedup_deterministic_compiles(repository):
    source = solution(
        repository,
        "#include <cp/graph/a.hpp>\n#include <cp/graph/b.hpp>\n#include <iostream>\nint main() { std::cout << answer; }\n",
    )
    commit = head(repository.root)
    write_text(repository.root / "include/cp/graph/b.hpp", "THIS IS A BROKEN WORKING TREE HEADER\n")
    first = expand(repository, source, commit=commit, tag="contest-snapshot/abc999")
    second = expand(repository, source, commit=commit, tag="contest-snapshot/abc999")
    assert first.text == second.text
    assert first.files == ["cp/graph/a.hpp", "cp/graph/b.hpp"]
    assert first.dependencies["cp/graph/a.hpp"] == ["cp/graph/b.hpp", "cp/graph/b.hpp"]
    assert first.text.count("inline constexpr int answer") == 1
    assert "BROKEN" not in first.text
    assert "#pragma once" not in first.text
    assert f"commit: {commit}" in first.text
    bundled = repository.root / "build/submit.cpp"
    write_text(bundled, first.text)
    compile_cpp(repository, bundled, bundled.parent / "submit", include_library=False)


def test_cycle_detection(repository):
    write_text(repository.root / "include/cp/graph/b.hpp", "#include <cp/graph/a.hpp>\n")
    with pytest.raises(KproError, match="dependency cycle"):
        expand(repository, solution(repository, "#include <cp/graph/a.hpp>\n"))


@pytest.mark.parametrize(
    "name", ["cp/graph/missing.hpp", "cp/../escape.hpp", "cp/graph/a.cpp", "cp/graph//a.hpp"]
)
def test_missing_or_invalid_header(repository, name):
    with pytest.raises(KproError):
        expand(repository, solution(repository, f"#include <{name}>\n"))


def test_snapshot_missing_header_never_uses_worktree(repository):
    write_text(repository.root / "include/cp/graph/new.hpp", "int new_code;\n")
    with pytest.raises(KproError, match="absent from base_commit"):
        expand(
            repository,
            solution(repository, "#include <cp/graph/new.hpp>\n"),
            commit=head(repository.root),
        )


def test_provenance_has_exact_files_and_no_timestamp():
    result = provenance("a" * 40, "contest-snapshot/example", ["cp/graph/a.hpp"])
    assert result.count("cp/graph/a.hpp") == 1
    assert "a" * 40 in result
    assert "2026" not in result
    assert "AI" not in result


def test_conditional_library_include_is_rejected(repository):
    source = solution(
        repository, "#if 0\n#include <cp/graph/a.hpp>\n#endif\n#include <cp/graph/a.hpp>\n"
    )
    with pytest.raises(KproError, match="Conditional library include"):
        expand(repository, source)


def test_traditional_guard_and_multiline_comments_survive(repository):
    write_text(
        repository.root / "include/cp/graph/a.hpp",
        "#ifndef CP_A_HPP\n#define CP_A_HPP\n#include <cp/graph/b.hpp> /* comment\ncontinues */\n#endif\n",
    )
    source = solution(
        repository,
        "#include <cp/graph/a.hpp> /* comment\ncontinues */\nint main() {return answer != 42;}\n",
    )
    result = expand(repository, source)
    bundled = repository.root / "build/submit.cpp"
    write_text(bundled, result.text)
    compile_cpp(repository, bundled, bundled.parent / "submit", include_library=False)


def test_pragma_inside_raw_string_is_not_removed(repository):
    write_text(
        repository.root / "include/cp/graph/a.hpp",
        '#pragma once\ninline const char* text = R"(\n#pragma once\n)";\n',
    )
    result = expand(repository, solution(repository, "#include <cp/graph/a.hpp>\n"))
    assert result.text.count("#pragma once") == 1


def test_quoted_include_with_angle_comment(repository):
    source = solution(
        repository,
        '#include "cp/graph/b.hpp" // see <angle syntax>\nint main() {return answer != 42;}\n',
    )
    result = expand(repository, source)
    bundled = repository.root / "build/submit.cpp"
    write_text(bundled, result.text)
    compile_cpp(repository, bundled, bundled.parent / "submit", include_library=False)
