---
title: Prime sieve and smallest prime factors
source: include/cp/math/sieve.hpp
status: stable
tags:
  - math
  - math/number-theory
aliases:
  - PrimeSieve
  - SPF
  - linear sieve
  - segmented sieve
  - 素数篩
  - 最小素因数
requires:
  - 上限は非負の int、表への問い合わせは 0 <= n <= limit
complexity:
  build: PrimeSieve は O(N) 時間・O(N) メモリ
  factorize: O(log n)、素数判定と SPF の取得は O(1)
  enumeration: 区間篩は O(N log log N) の marking と各ブロックでの素数走査
  enumeration-memory: 作業 O(sqrt N + B)、出力 O(pi(N))、B = 32768
pitfalls:
  - SPF 表は整数ごとに int を保持するので大きな上限ではメモリに注意
  - 0 の素因数分解は invalid_argument、1 の分解は空列
related:
  - math/primality
  - math/factorization
verification:
  unit:
    - verify/unit/math/sieve.test.cpp
  stress:
    - verify/stress/math/sieve.test.cpp
  online:
    - verify/online/math/enumerate_primes.test.cpp
---

# Prime sieve and smallest prime factors

`PrimeSieve` は線形篩で、素数列、素数判定表、最小素因数表をまとめて構築します。
素数列は昇順です。0 は素数でなく SPF=0、1 は素数でなく SPF=1 と定義します。

## API

| API | 動作 |
| --- | --- |
| `cp::PrimeSieve s(int limit)` | limit 以下の表を構築。limit>=0 |
| `s.limit()` | 構築した上限 |
| `s.primes()` | 昇順の素数一覧の const vector<int>& |
| `s.prime_table()` | 0..limit の判定表の const vector<bool>& |
| `s.smallest_factors()` | 0..limit の SPF 表の const vector<int>& |
| `s.is_prime(int n)` | 表を引いて素数判定 |
| `s.smallest_prime_factor(int n)` | 最小素因数。0/1 の規約は上記 |
| `s.factorize(int n)` | 昇順の (素数,指数) の vector<pair<int,int>> |
| `cp::enumerate_primes(int limit)` | SPF 表を持たず、区間篩で昇順の全素数を列挙 |

`factorize(1)` は空、`factorize(0)` は `std::invalid_argument` を投げます。
表の範囲外や負の上限は assert の対象です。NDEBUG 時は呼び出し側で守ってください。
表のメモリは概ね 4(N+1) bytes の SPF、(N+1)/8 bytes の素数フラグ、4*pi(N) bytes の
素数列で、vector の容量等は別途必要です。上限は int 全域の非負値を表現できますが、
実際に確保できるメモリに収める必要があります。積 x*p は uint64 で計算します。

`enumerate_primes` は SPF を必要としない大きな上限に使います。sqrt(limit) 以下の
素数を線形篩で用意し、奇数 B=32768 個ずつ処理します。素数の二乗とブロックの境界は
int64 で計算し、上限近くでも int の積を溢れさせません。marking は O(N log log N)、
ブロックごとの走査を含めると追加 O((N/B+1)*pi(sqrt N)) です。全素数を返すため出力の
メモリは別途必要です。

```cpp
#include <cp/math/sieve.hpp>
#include <cassert>
int main() {
    cp::PrimeSieve s(100);
    assert(s.is_prime(97));
    assert((s.factorize(72) == std::vector<std::pair<int,int>>{{2,3},{3,2}}));
    assert(cp::enumerate_primes(100) == s.primes());
}
```

単体テストは 0/1/2、素数冪、上限、区間篩のブロック境界を確認します。乱択で試し割り
と照合します。Library Checker Enumerate Primes の全ケースを区間篩で検証します。
同問題の上限は 5 億で、全 SPF 表を要求する問題ではありません。SPF と表内の分解は
独立したローカル比較テストで検証します。
