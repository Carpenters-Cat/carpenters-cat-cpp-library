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

## アルゴリズムの考え方

### 累乗と逆元

`pow_mod` は指数を二進数に分解し、底を二乗しながら 1 のビットの分だけ結果へ掛けます。
未処理の指数を e、現在の底を b、結果を r とすると `r*b^e` が元の累乗に等しいことを
保ち、指数が 0 になれば答えになります。指数のビット数分なので O(log(e+1)) です。
`inv_mod` は拡張 Euclid 法で `a*x+m*y=gcd(a,m)` を求め、gcd が 1 のときの x を
逆元として返します。法が素数である必要はなく、互いに素であることが必要です。

### 中国剰余定理

CRT は `x=r0 (mod m0)` と `x=r1 (mod m1)` を一つずつ併合します。
`x=r0+m0*t` と置くと、t に関する合同式になります。g=`gcd(m0,m1)` としたとき
`r1-r0` が g で割れなければ解はありません。割れるなら両辺を g で割って逆元を使い、
新しい法を lcm にして最小非負解を得ます。互いに素でない法にも対応する理由はこの判定です。
例えば x≡1 (mod 4)、x≡3 (mod 6) は x≡9 (mod 12) にまとまります。
各併合は Euclid 法の対数時間で、最終 lcm が long long に収まる前提です。

### floor sum

`floor_sum` は `sum_{i=0}^{n-1} floor((a*i+b)/m)` を一項ずつ足さずに計算します。
a,m の商を取り出せば三角数 `n(n-1)/2` の倍、b,m の商なら n の倍が先に加算できます。
残った `0<=a,b<m` の部分は、直線の下の格子点を数える問題として軸を交換し、
より小さい法の同形の問題に変換します。Euclid 法と同じように法が減るため O(log m) です。
例えば n=4,m=3,a=2,b=1 の各項は 0,1,1,2 で合計 4 です。
負の a,b は先に非負の剰余へ直し、取り出した整数部分を補正します。
結果の剰余仕様と引数の範囲は下の API 契約を確認してください。

`detail/internal_math.hpp` はこれらの共有処理、Barrett reduction、固定法の素数判定、
NTT の原始根探索をまとめています。内部関数を直接使うより、用途ごとの公開 API を選びます。

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
