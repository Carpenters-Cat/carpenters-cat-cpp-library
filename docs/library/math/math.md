---
title: Modular arithmetic and floor sum
source: include/cp/math/math.hpp
status: stable
tags:
  - math
  - math/number-theory
aliases:
  - pow_mod
  - inv_mod
  - crt
  - floor_sum
  - 中国剰余定理
  - 床関数の総和
requires:
  - pow_mod は指数 >= 0、1 <= 法 <= INT_MAX
  - inv_mod は法 >= 1、gcd(x, mod) = 1
  - crt は同じ長さ、各法 >= 1、全法の lcm が long long に収まる
  - floor_sum は 0 <= n < 2^32、1 <= m < 2^32
complexity:
  pow_mod: O(log exponent)
  inv_mod: O(log mod)
  crt: O(k log lcm)
  floor_sum: O(log m)
  memory: 出力を除いて O(1)
pitfalls:
  - floor_sum は答えが範囲外の場合 2^64 を法とする結果を返す
  - 不正な前提は assert。NDEBUG 時は呼び出し側が前提を守る
related:
  - math/modint
verification:
  unit:
    - verify/unit/math/math.test.cpp
  stress:
    - verify/stress/math/math.test.cpp
  online:
    - verify/online/math/math.test.cpp
---

# Modular arithmetic and floor sum

ACL の Math と同じ API を `cp` 名前空間で提供します。

## API

| API | 結果 |
| --- | --- |
| `long long cp::pow_mod(long long x, long long n, int m)` | x^n mod m。負の x に対応 |
| `long long cp::inv_mod(long long x, long long m)` | 0 <= y < m かつ xy = 1 mod m。負の x に対応 |
| `pair<long long,long long> cp::crt(const vector<long long>& r, const vector<long long>& m)` | x = r[i] mod m[i] の解 (y,z)。z は lcm、0 <= y < z |
| `long long cp::floor_sum(long long n, long long m, long long a, long long b)` | i = 0,...,n-1 について floor((ai+b)/m) の総和 |

`crt` は解がなければ (0,0)、空入力なら (0,1) を返します。法は互いに素でなくても
よく、負の剰余も正規化します。入力全体の lcm が long long に収まる必要があります。
`inv_mod` は法 1 では 0 を返します。`pow_mod(0,0,m)` は 1 mod m です。

`floor_sum` の a,b は long long 全域に対応し、負のときも切り捨てを数学的な床関数と
一致させます。中間積は符号なしで扱い、答えが long long に収まらないときは
2^64 を法としたビット列を long long として返します。正確な整数の答えが必要なら
答えが long long に収まることを呼び出し側で保証してください。

## Example

```cpp
#include <cp/math/math.hpp>
#include <cassert>
int main() {
    assert(cp::pow_mod(-2, 3, 7) == 6);
    auto [r,m] = cp::crt({2,3}, {3,5});
    assert(r == 8 && m == 15);
    assert(cp::floor_sum(4, 3, -2, -1) == -7);
}
```

## Verification and provenance

境界と負数、非互いに素な CRT を単体テストし、小さい法で全解列挙、累乗の反復積、
床関数の直接総和と乱択比較します。Library Checker の Sum of Floor of Linear の
全ケースをローカル実行します。
[ACL 公式 Math API](https://atcoder.github.io/ac-library/production/document_ja/math.html)
との差は名前空間のみです。[CC0 ソースの由来](../../third_party/acl-math.md)も参照してください。
