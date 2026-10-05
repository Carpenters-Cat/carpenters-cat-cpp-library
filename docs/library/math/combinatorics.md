---
title: Combinatorics
source: include/cp/math/combinatorics.hpp
status: stable
tags:
  - math
  - math/number-theory
aliases:
  - Combinatorics
  - nCr
  - nPr
  - factorial
  - 階乗
  - 組合せ
requires:
  - Mint は cp の固定または動的 modint、法 p は素数
  - 前計算する n は 0 <= n < p、動的法は構築後に変更しない
complexity:
  build: O(n + log p)
  ensure: 追加範囲に線形時間と O(log p)、倍増により全体 O(N + log N log p)
  query: 前計算済みなら O(1)、必要なら ensure する
  memory: O(N)
pitfalls:
  - n >= p、合成数法、巨大な n の Lucas 法には直接対応しない
related:
  - math/modint
verification:
  unit:
    - verify/unit/math/combinatorics.test.cpp
  stress:
    - verify/stress/math/combinatorics.test.cpp
  online:
    - verify/online/math/combinatorics.test.cpp
---

# Combinatorics

素数法の modint を用いた階乗表です。問い合わせで必要になった範囲まで自動で拡張します。

## API

| API | 動作 |
| --- | --- |
| `cp::Combinatorics<Mint> c(int n = 0)` | n! まで階乗と逆階乗を前計算 |
| `c.ensure(int n)` | 少なくとも n! まで用意。倍増して余分に前計算することがある |
| `c.size()` | 前計算済みの最大の添字 |
| `c.factorial(int n)` | n! |
| `c.inverse_factorial(int n)` | 1/n! |
| `c.nCr(int n, int r)` | n 個から r 個を選ぶ組合せ |
| `c.nPr(int n, int r)` | n 個から r 個を並べる順列 |

n >= 0 とします。r < 0 または r > n は 0 を返します。r が範囲内のときや階乗表の
拡張時は n < p が必要です。法 p は素数、動的 modint の場合は構築時の法を変更せず
使います。不正な前提は assert で検出し、NDEBUG 時は呼び出し側が守ります。
0! = 1、nC0 = nCn = nP0 = 1 です。

```cpp
#include <cp/math/combinatorics.hpp>
#include <cassert>
int main() {
    cp::Combinatorics<cp::modint998244353> c;
    assert(c.nCr(5,2).val() == 10);
    assert(c.nPr(5,2).val() == 20);
    c.ensure(1000);
    assert(c.factorial(1000) * c.inverse_factorial(1000) == 1);
}
```

単体テストは n=0、両端、範囲外の r、小さな素数と動的法を確認します。
固定 seed の比較テストは Pascal の三角形と直接の順列積を使います。
Library Checker の Binomial Coefficient (Prime Mod) を全ケース実行します。
同問題の n >= p は検証ドライバー側で Lucas の定理に分解し、各桁の組合せをこの API
で計算します。Lucas の定理自体は公開 API に含みません。
