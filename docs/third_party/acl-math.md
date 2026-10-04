# ACL math provenance

The math, modint and convolution headers and their private helpers are adapted
from [AtCoder Library](https://github.com/atcoder/ac-library/tree/864245a00b00dd008d1abfdc239618fdb7d139da/atcoder),
commit `864245a00b00dd008d1abfdc239618fdb7d139da` (CC0-1.0). The full dedication is in [acl-CC0.txt](acl-CC0.txt).

Adaptations: move public symbols to `cp`, isolate private symbols in
`cp::acl_math_internal`, rewrite includes to `cp/`, use unique include guards,
mark non-template functions and the dynamic modulus static member inline for
multiple translation units, and add `StaticModint` / `DynamicModint` aliases.
The upstream algorithms and supported ranges are retained. This is library
source, not a generated asset, and can be bundled without an external ACL install.
