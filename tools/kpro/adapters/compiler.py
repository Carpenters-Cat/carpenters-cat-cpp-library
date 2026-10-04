from pathlib import Path

from kpro.config import Config
from kpro.core import run


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
