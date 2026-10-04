# Implementation decisions (MVP)

## Verification adapter

Unit/stress tests run locally with the configured C++ compiler and timeout.
Online tests use the `competitive-verifier verify --verify-json ... --check-error`
CLI and its JSON input/output protocol. No backend internal Python APIs are imported.
`oj-resolve` enumerates tracked files, so it cannot verify a newly created, untracked
library without touching the user's Git index. The adapter generates the small
JSON input itself and compiles each test before invoking the backend. This keeps
`kpro lib new` → `kpro lib check` usable even before an initial commit.

Stable metadata must name at least one real verification test. All configured
tests must pass; an unavailable online backend is a failure, never a skipped pass.
Online verification runs downloaded judge tests locally; it does not submit code.

## Snapshot compilation

The bundler handles single-line literal `#include <cp/...>` / quoted `cp/...`
includes. Conditional library includes (except an outer conventional header
guard) and macro/continued include directives are rejected with an actionable
error: expanding them while deduplicating could silently change preprocessing
semantics. Keep canonical library dependencies unconditional. General C++
preprocessing remains outside the MVP scope. Comments and raw strings are not
interpreted as directives.

`run` and `test` compile the same snapshot bundle as `submit`. This extends the
specification's local compile workflow so local samples cannot accidentally test
a modified working-tree library while submission uses the contest snapshot.
`--force` may bypass failed/missing samples. Bundle and compile failures always
abort because there is no valid artifact to submit.

## Documentation

Zensical reads an ignored `.kpro/docs/source` mirror. Canonical Markdown stays
portable; full C++ source blocks exist only in the generated mirror. A source
watcher regenerates pages during `kpro docs`. Library docs, personal notes,
contest notes, aliases and hierarchical tags share a local JSON search index.
Verification records include content fingerprints, Git HEAD (null before an
initial commit), dirty state, UTC timestamp, backend and individual check results.
Fingerprints include all library headers, relevant tests/docs, tooling and uv.lock.
This deliberately invalidates conservatively rather than showing stale success.

## Git setup

Install the versioned push guard with `git config core.hooksPath .githooks`.
Contest start checks the installed executable hook and rolls back branch/tag/
workspace changes on failure. The snapshot tag is never moved by kpro, but it is
a normal local tag, not a cryptographic attestation.
The CLI does not create commits in the user's repository. Contest lifecycle
integration tests create and commit isolated temporary repositories instead.

## Third-party references

- [competitive-verifier attributes and CLI](https://competitive-verifier.github.io/competitive-verifier/document.html)
- [Zensical configuration](https://zensical.org/docs/setup/basics/)
- [online-judge-tools-ng](https://pypi.org/project/online-judge-tools-ng/)

Real judge submission requires the user's authenticated session and is not
performed by the implementation acceptance test. Submission gates are tested
with a fake JudgeBackend; online library verification uses public test data.
