from __future__ import annotations

import difflib
from pathlib import Path

from kpro.adapters.compiler import compile_cpp
from kpro.adapters.judge import Problem
from kpro.bundler import bundle_file
from kpro.config import Config, read_toml
from kpro.contest import locate_problem
from kpro.core import KproError, run


def test_samples(config: Config, executable: Path, directory: Path) -> None:
    cases = sorted(directory.glob("*.in"))
    if not cases:
        raise KproError(
            f"No sample inputs in {directory}. Download samples or add paired .in/.out files."
        )
    failures = []
    for case in cases:
        output = case.with_suffix(".out")
        if not output.is_file():
            failures.append(f"{case.name}: missing {output.name}")
            print(f"FAIL {case.name}: missing expected output")
            continue
        try:
            result = run(
                [str(executable)],
                cwd=config.root,
                check=False,
                input=case.read_text(encoding="utf-8"),
                capture_output=True,
                timeout=config.section("verify")["timeout"],
            )
        except KproError as exc:
            failures.append(str(exc))
            print(f"FAIL {case.name}: {exc}")
            continue
        expected = output.read_text(encoding="utf-8")
        if result.returncode == 0 and result.stdout.split() == expected.split():
            print(f"PASS {case.name}")
        else:
            failures.append(case.name)
            print(f"FAIL {case.name} (exit {result.returncode})")
            print(
                "".join(
                    difflib.unified_diff(
                        expected.splitlines(keepends=True),
                        result.stdout.splitlines(keepends=True),
                        fromfile="expected",
                        tofile="actual",
                    )
                ),
                end="",
            )
            if result.stderr:
                print(result.stderr)
    if failures:
        raise KproError(
            f"Sample tests failed: {len(failures)}/{len(cases)}. Fix the solution before submitting."
        )


def problem_data(source: Path) -> Problem:
    data = read_toml(source.parent / "problem.toml")
    if set(data) != {"id", "title", "url"} or not all(
        isinstance(value, str) and value for value in data.values()
    ):
        raise KproError("problem.toml must contain nonempty id, title, url strings.")
    return Problem(**data)


def compile_problem(config: Config, problem: str) -> tuple[Path, Path]:
    source, metadata = locate_problem(config, problem)
    # Compile the snapshot bundle for local testing too, so samples exercise submitted code.
    bundled = bundle_file(config, source, metadata)
    executable = compile_cpp(
        contest_config(config, metadata),
        bundled,
        source.parent / "build" / "main",
        include_library=False,
    )
    return source, executable


def contest_config(config: Config, metadata: dict | None) -> Config:
    if not metadata:
        return config
    return Config(config.root, config.values | {"cpp": config.section("cpp") | metadata["cpp"]})


def submit(
    config: Config, problem: str, backend, *, force: bool = False, language: str | None = None
) -> Path:
    source, metadata = locate_problem(config, problem)
    bundled = bundle_file(config, source, metadata)
    # A compile/bundle failure cannot produce a usable submission, even with --force.
    executable = compile_cpp(
        contest_config(config, metadata),
        bundled,
        source.parent / "build" / "submit",
        include_library=False,
    )
    try:
        test_samples(config, executable, source.parent / "tests")
    except KproError:
        if not force:
            raise
        print("WARNING: --force bypasses failed/missing samples.")
    backend.submit(problem_data(source), bundled, language)
    return bundled
