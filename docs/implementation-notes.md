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

## FIFO aggregation and line envelopes

SWAG maintains FIFO operand order using two aggregate stacks; empty pop is a
no-op returning false. LiChaoTree uses CoordinateCompression for a discrete,
fixed integer query domain. Segment insertions use half-open coordinate bounds,
and min/max are selected by comparison direction rather than coefficient
negation. Unregistered coordinates and uncovered queries return nullopt. All
intermediate line evaluations must be exact in the caller-selected value type;
the library does not saturate or silently ignore integer overflow.

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

## Static and persistent range queries

SparseTable accepts an associative idempotent semigroup without requiring an
identity; its half-open empty query returns nullopt. WaveletMatrix reuses
CoordinateCompression to support negative and generic ordered values, and stores
rank bitvectors in 64-bit blocks with popcount prefixes. Its kth query uses
zero-based k and returns nullopt outside the interval's order-statistic range.
PersistentSegmentTree retains immutable roots and path-copies point updates;
identity-only initial versions use a shared implicit identity subtree. Version
branching is verified against copied arrays locally, while online range-composite
verification exercises the current version and noncommutative operand order.

## Git setup

Install the versioned push guard with `git config core.hooksPath .githooks`.
Contest start checks the installed executable hook and rolls back branch/tag/
workspace changes on failure. The snapshot tag is never moved by kpro, but it is
a normal local tag, not a cryptographic attestation.
The CLI does not create commits in the user's repository. Contest lifecycle
integration tests create and commit isolated temporary repositories instead.

## Shortest-path distance arithmetic

The shortest-path entries use GCC/Clang's `__int128_t` for temporary integer
distances. This compiler extension is supported on the specified macOS/Linux
targets; public weights and finite results remain `std::int64_t`.
Unreachable and negative-cycle-affected states are separate from numeric values,
and an out-of-range finite result raises `std::overflow_error` rather than being
clipped or treated as unreachable. Bellman–Ford uses synchronous relaxation to
bound temporary walk lengths. Floyd skips pivots with negative diagonal values
and classifies affected pairs using full-graph reachability, preventing negative
cycles from causing arithmetic blowup while preserving every finite result.

## Third-party references

- [competitive-verifier attributes and CLI](https://competitive-verifier.github.io/competitive-verifier/document.html)
- [Zensical configuration](https://zensical.org/docs/setup/basics/)
- [online-judge-tools-ng](https://pypi.org/project/online-judge-tools-ng/)

Real judge submission requires the user's authenticated session and is not
performed by the implementation acceptance test. Submission gates are tested
with a fake JudgeBackend; online library verification uses public test data.

## ACL range structures

The range structures are independent C++23 implementations with PascalCase
public types and ACL-compatible `fenwick_tree`, `segtree` and `lazy_segtree`
aliases. Method contracts, half-open intervals, boundary searches and lazy
composition order follow ACL. Fenwick Tree additionally supports linear-time
vector construction; all three expose `size()`. Segment Tree reads are const.
Fenwick Tree supports standard integer types and user-defined additive groups;
compiler-specific extended integer types are not part of its portable API.

## ACL math, graph and string integration

Math, flow and SA-IS kernels adapt the CC0 ACL source at commit
`864245a00b00dd008d1abfdc239618fdb7d139da`; retained licenses and exact source
provenance are in `docs/third_party/acl-math.md` and `docs/acl-provenance.md`.
Canonical code remains under `include/cp/`, with unconditional library includes.
Math helpers use a dedicated namespace and header-defined non-template functions
are inline for multiple translation units. Fixed/dynamic Modint retains ACL
names and adds PascalCase aliases. Public graph classes use descriptive names.

SCC uses iterative Kosaraju instead of ACL's recursive implementation, retaining
topologically ordered components. TwoSat uses that shared SCC implementation.
String inputs use unsigned-byte ordering and LCP of an empty input returns an
empty vector. MinCostFlow explicitly asserts ACL's one-call flow/slope contract.
Existing UnionFind supplies the DSU functionality; its boolean merge result and
ACL representative-returning merge difference are documented without adding a
duplicate DSU. `cp/utility/acl.hpp` is an umbrella include with a cross-library test.
