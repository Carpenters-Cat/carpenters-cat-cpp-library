from __future__ import annotations

import json
import os
import re
import subprocess
from pathlib import Path
from urllib.parse import urlsplit

from kpro.adapters.compiler import compile_cpp
from kpro.config import Config
from kpro.core import KproError, run, write_json


def local_check(config: Config, source: Path, cache: Path) -> None:
    executable = compile_cpp(config, source, cache / "test", extra_flags=("-UNDEBUG",))
    run([str(executable)], cwd=config.root, timeout=config.section("verify")["timeout"])


def online_check(config: Config, sources: list[Path], cache: Path) -> dict:
    if config.section("verify")["backend"] != "competitive-verifier":
        raise KproError(
            "Online verification is configured for this entry. Set verify.backend = 'competitive-verifier'; local cannot fulfill it."
        )
    files = {}
    for index, source in enumerate(sources):
        match = re.search(
            r"^\s*//\s*competitive-verifier:\s*PROBLEM\s+(https://\S+)\s*$",
            source.read_text(encoding="utf-8"),
            re.M,
        )
        if not match:
            raise KproError(f"Add // competitive-verifier: PROBLEM https://... to {source}")
        url = urlsplit(match[1])
        if url.username or url.password or url.query or url.fragment:
            raise KproError(
                "Verification problem URLs must not contain credentials, queries, or fragments."
            )
        executable = compile_cpp(config, source, cache / str(index) / "test")
        # Public CLI JSON protocol, deliberately no competitive_verifier Python imports.
        files[source.relative_to(config.root).as_posix()] = {
            "verification": {
                "type": "problem",
                "name": config.section("cpp")["compiler"],
                "problem": match[1],
                "command": [str(executable)],
                "tle": config.section("verify")["timeout"],
            },
        }
    input_path = cache / "input.json"
    output_path = cache / "result.json"
    write_json(input_path, {"files": files})
    output_path.unlink(missing_ok=True)
    env = os.environ | {
        "COMPETITIVE_VERIFY_CONFIG_PATH": str(
            config.root / ".kpro" / "cache" / "competitive-verifier"
        )
    }
    log_path = cache / "backend.log"
    with log_path.open("w", encoding="utf-8") as stream:
        result = run(
            [
                "competitive-verifier",
                "verify",
                "--verify-json",
                str(input_path),
                "--check-error",
                "--output",
                str(output_path),
            ],
            cwd=config.root,
            env=env,
            check=False,
            stdout=stream,
            stderr=subprocess.STDOUT,
        )
    if result.returncode or not output_path.exists():
        raise KproError(
            f"competitive-verifier failed. Check network access/problem URL and inspect {log_path.relative_to(config.root)}, then retry."
        )
    try:
        data = json.loads(output_path.read_text(encoding="utf-8"))
        results = data["files"]
        case_count = 0
        for source in files:
            checks = results[source]["verifications"]
            if (
                not isinstance(checks, list)
                or not checks
                or any(check.get("status") != "success" for check in checks)
            ):
                raise KproError(f"Online verification did not succeed for {source}.")
            for check in checks:
                cases = check.get("testcases")
                if (
                    not isinstance(cases, list)
                    or not cases
                    or any(case.get("status") != "AC" for case in cases)
                ):
                    raise KproError(
                        f"Online verification has no complete AC testcase evidence for {source}."
                    )
                case_count += len(cases)
    except (KeyError, TypeError, ValueError) as exc:
        raise KproError(f"Malformed competitive-verifier result: {output_path}") from exc
    print(f"  Online judge tests: {case_count}/{case_count} AC", flush=True)
    return {
        "accepted_cases": case_count,
        "result_file": output_path.relative_to(config.root).as_posix(),
    }
