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

## アルゴリズムの考え方

素数法 p の下で階乗 `fact[n]=n!` と逆階乗 `ifact[n]=1/n!` を前計算します。
組合せは `nCr=fact[n]*ifact[r]*ifact[n-r]`、順列は `nPr=fact[n]*ifact[n-r]` で求まり、
表があれば各問い合わせは O(1) です。例えば `5C2=120/(2*6)=10` になります。
同じ法で多数の組合せを計算する DP や包除原理に向きます。

階乗は前から掛け、逆階乗は最大位置の階乗の逆元を一度だけ計算して、
`ifact[i-1]=ifact[i]*i` で後ろから埋めます。各位置で逆元を計算する必要がなく、
長さ N の構築は O(N+log p) です。階乗の定義とこの逆向きの式により、両方の表の
各要素が正しいことが帰納的に分かります。

`ensure` は現在の最大添字の約 2 倍まで広げるので、1 要素ずつ要求しても総走査量は
幾何級数として O(N) です。拡張ごとの逆元計算を含めると O(N+log(N+1) log p) です。
拡張後の `size()` は要求値を超えることがあります。

n が p 以上になると n! が 0 になり逆元が存在しません。この表は `n < p` が前提で、
Lucas の定理などを自動適用しません。動的 modint の法変更でも表は無効になります。
範囲外の r を 0 とする仕様は、数え上げの境界条件に使えます。

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
