from __future__ import annotations

from datetime import datetime, timezone

from kpro.adapters.compiler import compile_cpp
from kpro.adapters.verifier import local_check, online_check
from kpro.config import Config
from kpro.core import KproError, git, head, write_json, write_text
from kpro.docs import fingerprint, generate
from kpro.library import Entry, load_entry, validate_id


def check(config: Config, entry_id: str, *, build_docs: bool = True) -> bool:
    validate_id(entry_id)
    results = []
    entry: Entry | None = None
    cache = config.root / ".kpro" / "verify" / entry_id
    cache.mkdir(parents=True, exist_ok=True)

    def perform(name: str, function, backend: str = "kpro"):
        evidence = None
        try:
            evidence = function()
            result, detail = "pass", ""
        except (KproError, OSError) as exc:
            result, detail = "fail", str(exc)
        results.append({"type": name, "result": result, "backend": backend, "detail": detail})
        if isinstance(evidence, dict):
            results[-1]["evidence"] = evidence
        print(
            f"{'PASS' if result == 'pass' else 'FAIL'} {entry_id}: {name}"
            + (f"\n  {detail}" if detail else ""),
            flush=True,
        )

    def metadata():
        nonlocal entry
        entry = load_entry(config, entry_id)

    perform("metadata/source/tags", metadata)
    if entry:
        initial_fingerprint = fingerprint(config, entry)
        translation_unit = cache / "standalone.cpp"
        include = entry.source.relative_to(config.path("library_include_root")).as_posix()
        write_text(translation_unit, f"#include <{include}>\nint main() {{}}\n")
        perform(
            "standalone header",
            lambda: compile_cpp(config, translation_unit, cache / "standalone"),
            config.section("cpp")["compiler"],
        )
        for kind in ("unit", "stress"):
            for index, path in enumerate(entry.checks[kind]):
                perform(
                    f"{kind}: {path.name}",
                    lambda path=path, index=index, kind=kind: local_check(
                        config, path, cache / kind / str(index)
                    ),
                    "local",
                )
        if entry.checks["online"]:
            perform(
                "online",
                lambda: online_check(config, entry.checks["online"], cache / "online"),
                "competitive-verifier",
            )
        perform("documentation validation", lambda: generate(config))

        def unchanged():
            if fingerprint(config, entry) != initial_fingerprint:
                raise KproError(
                    "Sources/configuration changed during verification. Run the checks again."
                )

        perform("sources unchanged during verification", unchanged)
    record = {
        "library_id": entry_id,
        "git_commit": head(config.root),
        "dirty": bool(git(config.root, "status", "--porcelain", check=False)),
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "fingerprint": fingerprint(config, entry) if entry else None,
        "checks": results,
        "result": "pass" if all(item["result"] == "pass" for item in results) else "fail",
    }
    result_path = config.root / ".kpro" / "verify" / f"{entry_id}.json"
    write_json(result_path, record)
    if entry and build_docs:
        from kpro.adapters.documentation import build

        perform("documentation build", lambda: build(config), "zensical")
        record["result"] = "pass" if all(item["result"] == "pass" for item in results) else "fail"
        write_json(result_path, record)
        generate(config)
    return record["result"] == "pass"
