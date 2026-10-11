from pathlib import Path

from kpro.config import Config
from kpro.core import run


def has_header(config: Config, header: str) -> bool:
    """Probe using the same flags as standalone submission compilation."""
    cpp = config.section("cpp")
    result = run(
        [
            cpp["compiler"],
            f"-std={cpp['standard']}",
            *cpp["compile_flags"],
            # Local library include paths are omitted for the expanded bundle too.
            "-x",
            "c++",
            "-E",
            "-",
        ],
        cwd=config.root,
        input=f"#if __has_include(<{header}>)\nKPRO_HEADER_AVAILABLE\n#endif\n",
        capture_output=True,
    )
    return "KPRO_HEADER_AVAILABLE" in result.stdout.splitlines()


def compile_cpp(
    config: Config,
    source: Path,
    executable: Path,
    *,
    include_library: bool = True,
    extra_flags: tuple[str, ...] = (),
) -> Path:
    cpp = config.section("cpp")
    executable.parent.mkdir(parents=True, exist_ok=True)
    args = [cpp["compiler"], f"-std={cpp['standard']}", *cpp["compile_flags"]]
    if include_library:
        args += [f"-I{config.path('library_include_root')}", *cpp["include_flags"]]
    run([*args, *extra_flags, str(source), "-o", str(executable)], cwd=config.root)
    return executable
