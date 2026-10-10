---
title: Deterministic 64-bit primality
source: include/cp/math/primality.hpp
status: stable
tags:
  - math
  - math/number-theory
aliases:
  - Miller-Rabin
  - is_prime
  - is_prime_u64
  - 素数判定
requires:
  - mul_mod_u64 と pow_mod_u64 の法は 1..UINT64_MAX
  - GCC または Clang の __uint128_t を使う macOS/Linux 環境
complexity:
  is_prime: O(log n)、固定 7 底
  mul_mod_u64: 固定長整数で O(1)
  pow_mod_u64: O(log exponent)
  memory: O(1)
pitfalls:
  - 負の値を unsigned API へ変換する前に signed の is_prime を使う
related:
  - math/sieve
  - math/factorization
verification:
  unit:
    - verify/unit/math/primality.test.cpp
  stress:
    - verify/stress/math/primality.test.cpp
  online:
    - verify/online/math/primality.test.cpp
---

# Deterministic 64-bit primality

Miller–Rabin を固定の 7 底で実行し、uint64_t 全域を決定的に判定します。
0 と 1 は素数ではありません。負数を取る `is_prime` も false を返します。

## アルゴリズムの考え方

Miller–Rabin 法は奇数 n に対し `n-1=d*2^s`、d は奇数と分解し、
底 a ごとに `a^d,a^(2d),...,a^(2^(s-1)d)` を法 n で調べます。
素数なら最初が 1、またはこの列のどこかが -1 になります。
そうならない底は n が合成数である証拠です。素数体では 1 の平方根が ±1 だけなので、
Fermat の小定理による最後の 1 へ至る前の値が制限されることを利用します。

任意に選んだ少数の底では合成数が通過する可能性がありますが、この実装は uint64_t
全域用の固定 7 底 `2,325,9375,28178,450775,9780504,1795265022` を使います。
入力範囲をこの集合の保証に合わせ、乱数による確率判定にはしていません。
先に小さい素数で割り、0 と 1 を除外します。底が n の倍数の場合はその底を飛ばします。

例えば 15 の判定では小素数の割り算で合成数と分かります。
大きな候補では二分累乗と高々 O(log n) 回の二乗を固定個の底で行うため、時間は O(log n)、
メモリは O(1) です。積は uint128 の中間型で計算し、64 bit の乗算の桁あふれを防ぎます。
これは GCC/Clang の整数拡張を使う、このリポジトリの macOS/Linux 向け実装です。
多数の小さい整数には Sieve、合成数の因子が欲しい場合には Factorization を選びます。

## API

| API | 動作 |
| --- | --- |
| `cp::is_prime_u64(uint64_t n)` | 0..UINT64_MAX の素数判定 |
| `cp::is_prime(Integer n)` | 標準の 64 bit 以下の整数型。符号付き負数は false |
| `cp::mul_mod_u64(uint64_t a, uint64_t b, uint64_t m)` | a*b mod m。a,b は正規化不要 |
| `cp::pow_mod_u64(uint64_t a, uint64_t e, uint64_t m)` | a^e mod m。e は uint64 全域 |

m>=1 を assert します。m=1 は 0、e=0 は 1 mod m を返します。
乗算を `__uint128_t` へ拡張するので、a,b,m が uint64 の上端でも積を失いません。
GCC/Clang 拡張を使います。これは本リポジトリの macOS/Linux のコンパイラー向けです。
符号付きの `is_prime` は int64 の全域を受けられ、負数を uint64 へ変換する前に除外します。
`is_prime_u64` 自体は unsigned の関数なので、負数の暗黙変換を避けてください。

底は `2, 325, 9375, 28178, 450775, 9780504, 1795265022` です。
[決定的 Miller–Rabin の記録を公開する原資料](https://miller-rabin.appspot.com/) に
Jim Sinclair の集合として記載され、少なくとも 2^64 未満の範囲を対象としています。
底が n の倍数ならその検査を省略します。小さい素数による割り算を先に行います。

```cpp
#include <cp/math/primality.hpp>
#include <cassert>
int main() {
    assert(!cp::is_prime(-5));
    assert(cp::is_prime_u64(18446744073709551557ULL));
    assert(cp::mul_mod_u64(UINT64_MAX-1, UINT64_MAX-1, UINT64_MAX) == 1);
}
```

Carmichael 数、強擬素数、2^64 近くの素数・合成数を単体テストします。篩との全件比較と、
128 bit の乗算を使わない二進加算法による剰余積・累乗の乱択比較を行います。
Library Checker Primality Test の全ケースをローカルで実行します。
