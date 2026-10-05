# ACL provenance

`max_flow.hpp`, `min_cost_flow.hpp` and `string_algorithms.hpp` are adapted
from [AtCoder Library](https://github.com/atcoder/ac-library/tree/864245a00b00dd008d1abfdc239618fdb7d139da),
commit `864245a00b00dd008d1abfdc239618fdb7d139da`, under [CC0-1.0](licenses/acl-CC0.txt).
The bundled license preserves the original dedication.

The adaptations use namespace `cp`, descriptive class names, inline non-template
string functions for safe multi-translation-unit use, a standard queue and a
private CSR representation so each public header compiles independently.
Constructors reject negative sizes before allocation. Min-cost flow enforces
ACL's one-call precondition with an assertion. String inputs are interpreted
as unsigned bytes, and LCP of empty input returns an empty array.
SCC uses an independently implemented iterative Kosaraju algorithm to avoid
recursive stack overflow; TwoSat uses that SCC implementation.
