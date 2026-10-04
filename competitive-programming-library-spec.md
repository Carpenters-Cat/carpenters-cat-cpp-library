# Competitive Programming Library System Specification

**Document status:** Draft for implementation  
**Target implementation agent:** Codex  
**Primary language:** C++ for the competitive-programming library; implementation language for tooling may be Python or Rust, with Python preferred for the first version unless there is a strong reason otherwise.  
**Last updated:** 2026-10-04

---

## 1. Purpose

Build a local-first competitive-programming environment that unifies:

1. reusable C++ library management,
2. searchable documentation and personal knowledge,
3. automated verification,
4. contest workspace creation,
5. local compile/test/submit workflows,
6. generation of single-file submissions,
7. provenance of pre-contest library code,
8. storage and later search of contest notes,
9. safeguards against accidental pushes during contests.

The system should optimize for:

- reliability during contests,
- low operational overhead,
- reproducibility,
- long-term maintainability,
- easy replacement of third-party tools,
- discoverability of both code and prior reasoning.

The system is intended for personal use first. Multi-user collaboration is not a first-class requirement.

---

## 2. Design principles

### 2.1 Git commit is the source of truth for contest provenance

A contest starts from a recorded Git commit.

All pre-existing library code bundled into submissions during that contest MUST be resolvable from the recorded `base_commit`.

The implementation MUST NOT rely only on comments or timestamps to prove that code existed before the contest.

### 2.2 C++ library source is not duplicated

A library implementation MUST have exactly one source of truth:

```text
include/cp/**.hpp
```

Documentation, rendered source snippets, and bundled submissions MUST derive library code from that source.

Do not manually duplicate full library implementations into documentation.

### 2.3 Stable library code is trustworthy

The intended invariant is:

> Every `stable` library entry on `main` has valid metadata/documentation and passes the verification requirements configured for that entry.

The system SHOULD make it difficult to mark a library as usable without satisfying its checks.

### 2.4 Third-party tools are adapters, not architecture

Third-party tools may change or become unmaintained.

The core system MUST isolate them behind narrow interfaces.

In particular:

- verification backend,
- online judge backend,
- documentation generator

must not leak their internal APIs throughout the codebase.

### 2.5 Local-first operation

Core workflows MUST work locally.

The documentation UI MUST be usable from a local server.

The library search index SHOULD be buildable without a hosted external search service.

### 2.6 Deterministic generation

Given:

- the same contest metadata,
- the same solution source,
- the same `base_commit`,
- the same tool version/configuration,

submission bundling SHOULD produce semantically identical output and SHOULD be byte-for-byte deterministic where practical.

---

## 3. High-level architecture

```text
                         ┌─────────────────────────┐
                         │        kpro CLI         │
                         │  thin orchestration     │
                         └────────────┬────────────┘
                                      │
          ┌───────────────────────────┼────────────────────────────┐
          │                           │                            │
          ▼                           ▼                            ▼
 ┌──────────────────┐       ┌──────────────────┐        ┌──────────────────┐
 │ Library subsystem │       │ Knowledge system │        │ Contest subsystem │
 │ include/cp/**     │       │ docs + notes     │        │ workspaces        │
 └────────┬─────────┘       └────────┬─────────┘        └────────┬─────────┘
          │                          │                           │
          ▼                          ▼                           ▼
 verification adapter         documentation adapter       judge adapter
          │                          │                           │
 competitive-verifier            Zensical               oj-ng or future
```

The `kpro` CLI owns orchestration and project policy.

It MUST remain usable even if one external backend is replaced.

---

## 4. Third-party tool policy

### 4.1 Verification

Preferred backend:

- `competitive-verifier`

Use it through a small adapter.

The rest of the project MUST NOT depend directly on its internal Python APIs unless unavoidable.

Preferred integration style:

- subprocess invocation,
- clearly parsed exit codes/output,
- adapter-specific implementation under `tools/kpro/adapters/`.

### 4.2 Documentation

Preferred documentation engine:

- Zensical

Requirements:

- local development server,
- Markdown source,
- search,
- tags,
- code-copy UI,
- generated library source blocks.

The documentation source format MUST remain portable Markdown even if Zensical is replaced later.

### 4.3 Online judge operations

Preferred first backend:

- `online-judge-tools-ng` or a compatible maintained successor.

The project MUST define its own judge abstraction.

Minimum conceptual interface:

```text
JudgeBackend
  fetch_contest(url)
  fetch_problem(url)
  download_samples(problem)
  test(problem, executable)
  submit(problem, source_file, language?)
```

The first implementation may support AtCoder only.

Do not make the repository structure depend on backend-specific output.

### 4.4 Tools explicitly not required

Do not base the architecture on:

- legacy `online-judge-tools` as a hard dependency,
- `oj-prepare`,
- MkDocs + Material for MkDocs.

They may be supported later through optional adapters if useful.

---

## 5. Repository layout

Target layout:

```text
repo/
├── include/
│   └── cp/
│       ├── graph/
│       ├── data_structure/
│       ├── math/
│       ├── string/
│       ├── geometry/
│       └── ...
│
├── docs/
│   ├── index.md
│   ├── library/
│   │   ├── graph/
│   │   ├── data_structure/
│   │   └── ...
│   ├── notes/
│   │   └── techniques/
│   └── generated/
│
├── verify/
│   ├── online/
│   ├── unit/
│   └── stress/
│
├── contests/
│   └── YYYY/
│       └── contest-id/
│           ├── contest.toml
│           ├── notes.md
│           ├── a/
│           │   ├── main.cpp
│           │   └── tests/
│           ├── b/
│           └── ...
│
├── tools/
│   └── kpro/
│       ├── cli/
│       ├── adapters/
│       ├── bundler/
│       ├── contest/
│       ├── docs/
│       ├── library/
│       ├── verify/
│       └── config/
│
├── .githooks/
│   └── pre-push
├── kpro.toml
├── zensical.toml
├── pyproject.toml
└── README.md
```

Exact internal tooling layout may change, but the public-facing repository layout SHOULD stay stable.

---

## 6. Global configuration

Use a repository-root configuration file:

```text
kpro.toml
```

Suggested schema:

```toml
[project]
library_include_root = "include"
contest_root = "contests"
docs_root = "docs"
verify_root = "verify"

[cpp]
compiler = "g++"
standard = "c++23"
compile_flags = ["-O2", "-Wall", "-Wextra"]
include_flags = ["-Iinclude"]

[docs]
engine = "zensical"
host = "127.0.0.1"
port = 8000

[verify]
backend = "competitive-verifier"

[judge]
backend = "oj-ng"

[contest]
disable_push = true
record_snapshot_tag = true

[bundle]
library_prefix = "cp/"
emit_provenance_comment = true
```

Configuration parsing MUST validate unknown/invalid values and give actionable errors.

---

## 7. Library entry specification

Each reusable component consists of:

1. a C++ header,
2. a documentation Markdown file,
3. zero or more verification files.

Example:

```text
include/cp/graph/dijkstra.hpp
docs/library/graph/dijkstra.md
verify/online/dijkstra.test.cpp
verify/stress/dijkstra_stress.cpp
```

### 7.1 C++ include namespace

User code should include library entries as:

```cpp
#include <cp/graph/dijkstra.hpp>
```

Public headers MUST be reachable from the configured include root.

### 7.2 Library documentation metadata

Every library documentation file MUST contain front matter.

Required fields:

```yaml
---
title: Dijkstra
source: include/cp/graph/dijkstra.hpp
status: stable
tags:
  - graph
  - graph/shortest-path
aliases:
  - Dijkstra
  - ダイクストラ
  - shortest path
  - SSSP
---
```

Recommended optional fields:

```yaml
complexity:
  build: O(1)
  query: O((V + E) log V)

requires:
  - edge weights are nonnegative

pitfalls:
  - integer overflow
  - not valid for negative edges

related:
  - graph/bellman-ford
  - graph/warshall-floyd
```

### 7.3 Status values

Allowed values:

```text
experimental
stable
deprecated
```

Meaning:

- `experimental`: not assumed contest-safe.
- `stable`: expected to satisfy configured verification policy.
- `deprecated`: retained for reference but should not be recommended.

`verified` MUST NOT be a manually maintained status.

Verification state is derived from actual checks.

---

## 8. Tag system

Tags are a first-class feature.

### 8.1 Goals

Tags must support:

- browsing libraries by concept,
- browsing prior contest notes,
- cross-linking techniques and implementations,
- filtering search results,
- identifying mistakes and recurring failure modes.

### 8.2 Hierarchical tags

Use `/` as the hierarchy separator.

Examples:

```text
graph
graph/shortest-path
graph/flow
data-structure
data-structure/range-query
math
math/number-theory
string
geometry
dp
optimization
randomized

mistake
mistake/overflow
mistake/off-by-one
mistake/complexity
mistake/wrong-invariant
```

### 8.3 Controlled vocabulary

The system SHOULD maintain an allowed tag registry to avoid accidental variants.

Example registry file:

```text
docs/tags.toml
```

or equivalent structured file.

CLI validation SHOULD reject unknown tags by default, with an explicit mechanism to add new tags.

### 8.4 Search behavior

Search SHOULD index:

- title,
- aliases,
- tags,
- explanation text,
- API text,
- constraints/preconditions,
- pitfalls,
- complexity,
- contest notes.

Searching for a tag literal SHOULD surface items carrying that tag.

---

## 9. Documentation requirements

Each stable library page SHOULD contain:

```text
Title
Summary
When to use
Preconditions
API
Complexity
Pitfalls
Examples
Verification status
Related entries
Full source code
```

### 9.1 Generated source block

The full source block MUST be generated from the `source` metadata field.

Do not store a manually copied full source body in Markdown.

The generated page SHOULD expose a code-copy button.

### 9.2 Local server

Command:

```bash
kpro docs
```

Expected behavior:

1. validate docs metadata,
2. generate derived documentation artifacts,
3. start local documentation server,
4. bind to `127.0.0.1` by default,
5. print the local URL.

The server MUST NOT bind to all network interfaces unless explicitly requested.

### 9.3 Documentation build

Command:

```bash
kpro docs build
```

This MUST build the static documentation and fail on:

- broken required metadata,
- missing source files,
- invalid tags,
- invalid internal references if detectable.

---

## 10. Verification subsystem

Verification types:

```text
unit
stress/randomized
online judge
```

### 10.1 Commands

```bash
kpro verify
kpro verify graph/dijkstra
kpro lib check graph/dijkstra
```

### 10.2 `kpro lib check`

For one entry, perform as applicable:

```text
metadata validation
source existence
standalone header compile check
unit tests
stress/randomized tests
online verification
documentation build/validation
```

The output MUST summarize individual pass/fail results.

### 10.3 Verification provenance

Verification results SHOULD record at least:

```text
library id
Git commit
check type
timestamp
result
backend
```

This may be stored under a generated/cache directory.

Generated verification state SHOULD NOT require manual editing.

### 10.4 CI

A future CI workflow SHOULD run:

```text
format/lint where configured
stable library checks
documentation validation/build
```

CI integration is recommended but not mandatory for the first local MVP.

---

## 11. Library creation workflow

Command:

```bash
kpro lib new graph/dijkstra
```

It SHOULD create:

```text
include/cp/graph/dijkstra.hpp
docs/library/graph/dijkstra.md
verify/unit/dijkstra.test.cpp   # optional template
```

The template MUST include TODO markers for:

- summary,
- aliases,
- tags,
- preconditions,
- complexity,
- pitfalls.

The command MUST refuse to overwrite existing files unless explicitly forced.

---

## 12. Contest lifecycle

### 12.1 Start command

Preferred interface:

```bash
kpro contest start <contest-url-or-id>
```

Example:

```bash
kpro contest start https://atcoder.jp/contests/abc999
```

### 12.2 Preconditions

Before starting, the CLI SHOULD check:

1. current repository is valid,
2. current branch is `main` or configured base branch,
3. worktree is clean,
4. required tools are available,
5. no contest lock already exists.

If the worktree is dirty, abort by default.

### 12.3 Start sequence

The start command MUST:

1. record current `HEAD` as `base_commit`,
2. optionally create an immutable/signed or normal tag such as:
   ```text
   contest-snapshot/abc999
   ```
3. create a new contest branch, e.g.:
   ```text
   contest/abc999
   ```
4. create contest directory,
5. retrieve contest/problem metadata through the judge adapter,
6. create problem directories,
7. download samples if supported,
8. create `contest.toml`,
9. create `notes.md`,
10. enable contest push lock,
11. report the snapshot commit and created files.

### 12.4 Contest metadata

Example:

```toml
contest = "abc999"
platform = "atcoder"
url = "https://atcoder.jp/contests/abc999"

base_commit = "83bc20a2..."
snapshot_tag = "contest-snapshot/abc999"
started_at = "2026-10-04T21:00:00+09:00"

branch = "contest/abc999"

[cpp]
compiler = "g++"
standard = "c++23"
```

This file is authoritative for bundling library code during the contest.

---

## 13. Contest notes

Each contest has:

```text
contests/YYYY/contest-id/notes.md
```

A problem-specific note MAY also exist under its problem directory.

Suggested front matter:

```yaml
---
contest: abc999
problem: F
result: WA
tags:
  - graph/shortest-path
  - dp
  - mistake/overflow
---
```

Suggested body:

```text
Initial idea
Key observations
Failed approaches
Final approach
WA/TLE causes
Lessons
Reusable technique
```

Contest notes MUST be indexed by the same documentation/search system as library docs.

---

## 14. Local compile and run

### 14.1 Run

```bash
kpro run a
```

Expected behavior:

1. locate contest problem `a`,
2. compile `main.cpp`,
3. run executable interactively or according to configured mode.

### 14.2 Test samples

```bash
kpro test a
```

Expected behavior:

1. compile,
2. run all stored sample tests,
3. compare output,
4. show per-case results and diff on failure.

Judge-specific sample testing MAY delegate to the backend, but local stored tests are preferred as the stable interface.

### 14.3 Compiler diagnostics

On compilation failure, preserve the native compiler diagnostics.

Do not replace useful compiler output with a generic error.

---

## 15. Submission bundler

Command:

```bash
kpro bundle a
```

or:

```bash
kpro bundle contests/2026/abc999/a/main.cpp
```

### 15.1 Core rule

Any include matching:

```cpp
#include <cp/...>
```

MUST be expanded using the version stored at the contest's `base_commit`.

Conceptually:

```bash
git show <base_commit>:include/cp/graph/dijkstra.hpp
```

The bundler MUST NOT silently read the working-tree version of pre-existing library headers during a contest.

### 15.2 Include handling

The bundler MUST:

- recursively resolve `#include <cp/...>`,
- preserve system/standard-library includes,
- remove duplicate expanded library headers,
- handle `#pragma once`,
- detect dependency cycles,
- fail on missing library headers,
- preserve source ordering where necessary.

### 15.3 Contest-written code

The contest solution itself comes from the current working tree.

Therefore a bundle is:

```text
current contest solution
+
pre-contest library @ base_commit
```

### 15.4 Non-contest use

Outside a contest workspace, bundling MAY use current `HEAD` or the working tree, but the behavior must be explicit and documented.

### 15.5 Output location

Default:

```text
<problem-dir>/build/submit.cpp
```

Generated files SHOULD live under ignored build directories.

---

## 16. Provenance comment

Bundled submissions SHOULD begin with a generated comment.

Example:

```cpp
/*
 * Generated by kpro.
 *
 * Pre-existing library snapshot:
 *   commit: 83bc20a2...
 *   tag: contest-snapshot/abc999
 *
 * Expanded library files:
 *   cp/graph/dijkstra.hpp
 *   cp/data_structure/priority_queue.hpp
 *
 * Contest solution source is not part of the pre-contest snapshot.
 */
```

Requirements:

- commit hash MUST match `contest.toml`,
- included library file list SHOULD be exact,
- comment generation MUST be deterministic.

This provenance comment is informational only and MUST NOT claim that a contest organizer accepts it as proof of non-AI use or pre-existing authorship.

---

## 17. Submit workflow

Command:

```bash
kpro submit a
```

Default sequence:

```text
bundle
  ↓
compile bundled file
  ↓
run sample tests against bundled executable
  ↓
submit through JudgeBackend
```

Submission MUST abort if:

- bundling fails,
- bundled source does not compile,
- sample tests fail,

unless the user passes an explicit force option.

The force option SHOULD be intentionally verbose, e.g.:

```bash
kpro submit a --force
```

Do not invent a short single-letter flag for unsafe bypasses.

---

## 18. Push lock

### 18.1 Requirement

During an active contest, accidental `git push` MUST be blocked locally.

This is a safety measure, not a security boundary.

### 18.2 Mechanism

Version-control the hook:

```text
.githooks/pre-push
```

Configure:

```bash
git config core.hooksPath .githooks
```

Contest state SHOULD be represented by a local lock file such as:

```text
.git/kpro/contest-lock.toml
```

Do not store an active lock in a tracked file.

### 18.3 Hook behavior

If contest lock exists, reject every push by default.

Example output:

```text
ERROR: kpro contest mode is active.

Contest: abc999
Started: 2026-10-04T21:00:00+09:00

Push is disabled during an active contest.
Run `kpro contest end` after the contest.
```

An emergency override MAY exist, but SHOULD require an explicit environment variable or long option and SHOULD print a warning.

---

## 19. Contest end

Command:

```bash
kpro contest end
```

Expected behavior:

1. validate active contest,
2. display contest summary,
3. remove push lock,
4. preserve contest branch,
5. preserve contest metadata and notes,
6. do not auto-merge to `main`,
7. optionally print reminders about unresolved notes or uncommitted work.

The command MUST NOT delete contest files.

---

## 20. Search

### 20.1 Web UI search

Primary search UX is the local documentation server.

It SHOULD search both:

- reusable library docs,
- contest/personal notes.

### 20.2 CLI search

Provide:

```bash
kpro search "区間 直線 最小値"
```

The first implementation MAY:

- invoke/search the documentation index,
- use a generated JSON index,
- perform a local textual search.

It does not need semantic embeddings initially.

### 20.3 Indexed fields

The search index SHOULD include:

```text
title
aliases
tags
summary
body
complexity
preconditions
pitfalls
related entries
contest/problem identifiers
```

### 20.4 Ranking expectations

Exact ranking algorithm is not part of the initial specification.

However:

- exact title/alias matches SHOULD rank highly,
- tag matches SHOULD be visible,
- deprecated entries SHOULD be visually distinguishable.

---

## 21. `kpro doctor`

Command:

```bash
kpro doctor
```

Checks SHOULD include:

```text
Git available
repository root detected
compiler available
compiler version
configured C++ standard supported
competitive-verifier available if enabled
documentation engine available
judge backend available if enabled
Git hooks configured
configuration parses
required directories exist
active contest lock state
```

Output SHOULD be human-readable and return nonzero if required dependencies are missing.

---

## 22. CLI specification

Initial public command surface:

```text
kpro doctor

kpro lib new <id>
kpro lib check <id>

kpro verify [<id>]

kpro docs
kpro docs build
kpro search <query>

kpro contest start <url-or-id>
kpro contest status
kpro contest end

kpro run <problem>
kpro test <problem>
kpro bundle <problem>
kpro submit <problem>
```

CLI output should prioritize concise contest-time usability.

Machine-readable `--json` output MAY be added later.

---

## 23. Error-handling requirements

Every command MUST:

- return nonzero on failure,
- give an actionable error message,
- avoid destructive partial state where feasible.

Examples:

Bad:

```text
Error
```

Good:

```text
Cannot start contest: working tree is dirty.
Commit, stash, or discard changes before retrying.
```

If a multi-step operation fails after creating files, the system SHOULD either:

- roll back,
- or clearly report what was created and how to recover.

---

## 24. Logging

Default CLI output should be concise.

Verbose diagnostic mode:

```bash
kpro --verbose ...
```

Debug logs SHOULD include:

- invoked backend,
- executed external commands,
- resolved paths,
- selected Git commit,
- generated file path.

Do not print authentication secrets or cookies.

---

## 25. Caching

Generated/cached state SHOULD be stored under:

```text
.kpro/
```

or:

```text
.git/kpro/
```

depending on whether it is project cache or Git-local state.

Suggested split:

```text
.kpro/
  cache/
  docs/
  verify/

.git/kpro/
  contest-lock.toml
```

Transient generated content MUST NOT be required in Git.

---

## 26. Security and privacy

The system is local-first.

Requirements:

- docs server binds to localhost by default,
- no credentials committed to repository,
- judge credentials/cookies use backend-standard secure storage where possible,
- logs must redact secrets,
- contest notes remain local unless the user explicitly pushes them after the contest.

The system does not need to provide cryptographic anti-tampering guarantees in the initial version.

---

## 27. AI usage and provenance

The generated source comment MAY record pre-contest library provenance.

It MUST NOT state or imply:

- that no AI was used,
- that a contest organizer has certified the code,
- that Git history alone proves compliance with a contest rule.

Contest rules differ.

The system only records reproducible technical provenance:

```text
which library files
from which Git commit
were expanded into the submission
```

---

## 28. Non-goals for the first version

Do NOT implement in the initial MVP unless needed to satisfy another requirement:

- AI/LLM semantic search,
- cloud-hosted documentation,
- multi-user collaboration,
- browser extension,
- full IDE plugin,
- Windows support if it significantly delays macOS/Linux support,
- automatic editorial generation,
- automatic merging of contest branches,
- custom online judge scraper when an adapter already works,
- generalized C/C++ preprocessing beyond the defined `<cp/...>` library includes,
- cryptographic attestation of contest provenance.

---

## 29. Portability

Primary supported environments:

1. macOS,
2. Linux.

The implementation SHOULD avoid shell-only logic where a portable implementation is easy.

External executable discovery SHOULD use `PATH`.

Paths MUST be handled without assuming `/` string concatenation internally.

---

## 30. Testing requirements for `kpro`

### 30.1 Unit tests

Cover at least:

- config parsing,
- library metadata validation,
- tag validation,
- contest metadata parsing,
- include parsing,
- dependency graph creation,
- cycle detection,
- provenance generation,
- contest lock logic.

### 30.2 Integration tests

Create temporary Git repositories and test:

#### Contest start

- clean `main` succeeds,
- dirty worktree fails,
- records correct `base_commit`,
- creates branch/workspace,
- creates lock.

#### Bundler

Given:

```text
main.cpp
  -> cp/a.hpp
       -> cp/b.hpp
```

verify:

- both are expanded,
- duplicate `b.hpp` is emitted once,
- source is taken from `base_commit`,
- working-tree edits to `cp/a.hpp` after contest start do not alter bundle output.

#### Push lock

Verify:

- push hook rejects when lock exists,
- allows when lock does not exist.

#### Submission

With a fake JudgeBackend:

- bundle failure prevents submit,
- compile failure prevents submit,
- sample failure prevents submit,
- passing flow invokes backend submit exactly once.

### 30.3 Snapshot tests

Snapshot tests MAY be used for:

- generated documentation fragments,
- provenance comments,
- contest directory templates.

---

## 31. Acceptance criteria for MVP

The MVP is complete when the following scenario works end-to-end.

### Scenario A: add a library

1. Run:
   ```bash
   kpro lib new graph/dijkstra
   ```
2. Implement the header.
3. Fill documentation and tags.
4. Add a verification test.
5. Run:
   ```bash
   kpro lib check graph/dijkstra
   ```
6. All configured checks pass.
7. Run:
   ```bash
   kpro docs
   ```
8. Search for:
   ```text
   最短路
   ```
9. Dijkstra appears.
10. The documentation page shows generated source with a copy button.

### Scenario B: start a contest

1. Repository is clean on `main`.
2. Run:
   ```bash
   kpro contest start <contest>
   ```
3. Contest branch is created.
4. `contest.toml` contains exact pre-contest commit.
5. Problem directories and samples are created.
6. Push lock is active.

### Scenario C: use a library

1. Write:
   ```cpp
   #include <cp/graph/dijkstra.hpp>
   ```
2. Run:
   ```bash
   kpro test a
   ```
3. Local samples pass.

### Scenario D: bundle

1. Modify the working-tree copy of `dijkstra.hpp` after contest start.
2. Run:
   ```bash
   kpro bundle a
   ```
3. The resulting `submit.cpp` uses the version from `base_commit`, not the modified file.
4. The top comment lists the correct Git commit and expanded library files.
5. Bundled source compiles.

### Scenario E: submit

1. Run:
   ```bash
   kpro submit a
   ```
2. The bundled source is generated.
3. It is compiled.
4. Samples are tested.
5. Only then is the configured judge backend called.

### Scenario F: finish contest

1. Run:
   ```bash
   kpro contest end
   ```
2. Push lock is removed.
3. Contest files remain.
4. Contest branch remains.
5. Notes can be indexed by the docs/search system.

---

## 32. Recommended implementation order

Implement in this order.

### Phase 1: repository/core

- configuration loading,
- repository root detection,
- `kpro doctor`,
- metadata models,
- tag validation.

### Phase 2: bundler

- parse `<cp/...>` includes,
- recursive dependency resolution,
- duplicate removal,
- `git show <commit>:<path>` source loader,
- provenance comment,
- compile bundled result.

The bundler is central and should be well-tested early.

### Phase 3: contest lifecycle

- `contest start`,
- `contest status`,
- `contest end`,
- branch creation,
- `contest.toml`,
- lock,
- pre-push hook.

### Phase 4: local run/test

- compile,
- sample storage,
- sample runner,
- readable diffs.

### Phase 5: library workflow

- `lib new`,
- metadata validation,
- `lib check`,
- competitive-verifier adapter.

### Phase 6: docs/search

- Zensical configuration,
- generated library source blocks,
- tags,
- contest notes indexing,
- `kpro docs`,
- `kpro search`.

### Phase 7: judge integration

- `JudgeBackend`,
- AtCoder backend using maintained external tooling,
- contest/problem metadata retrieval,
- sample download,
- submit.

Judge integration should come after core workflows so that changes in external sites do not block development of the rest of the system.

---

## 33. Implementation guidance for Codex

When implementing:

1. Prefer simple, explicit abstractions over a plugin framework.
2. Do not generalize beyond the requirements without evidence it reduces complexity.
3. Keep external-tool integration in adapter modules.
4. Add tests together with each subsystem.
5. Do not silently relax invariants.
6. If behavior is ambiguous, prefer the safer contest-time behavior:
   - abort rather than overwrite,
   - abort rather than submit failing samples,
   - reject push rather than assume intent,
   - use the recorded snapshot rather than the working tree.
7. Avoid shell pipelines for logic that needs structured error handling.
8. Keep generated files clearly separated from hand-written files.
9. Document any deviation from this specification in an ADR or implementation note before relying on it.
10. Do not change the public CLI command names without an explicit reason.

---

## 34. Suggested future enhancements

After the MVP is stable, consider:

- semantic/embedding-based local search,
- Competitive Companion input,
- per-platform judge adapters,
- automatic snippet insertion into the editor,
- compilation cache,
- benchmark tracking for selected library components,
- richer dependency graph visualization,
- “recently used” library pages,
- failure/mistake statistics from notes,
- local full-text SQLite index,
- optional shell completion,
- optional editor integration,
- optional CI verification dashboard.

These are explicitly lower priority than reliability of the MVP.

---

## 35. Summary of key invariants

The implementation should preserve these invariants:

1. **A stable library has documentation and configured verification.**
2. **Library implementation exists in one canonical `.hpp` source.**
3. **Contest start records one immutable `base_commit`.**
4. **Bundling during a contest expands pre-existing library code from `base_commit`.**
5. **Submission compiles and tests the exact bundled file before submitting.**
6. **Active contest mode blocks accidental Git pushes.**
7. **Library docs and contest notes share one searchable tag/index system.**
8. **External tools are replaceable adapters, not architectural dependencies.**

These invariants are more important than convenience features.
