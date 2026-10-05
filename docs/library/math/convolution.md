---
title: Convolution
source: include/cp/math/convolution.hpp
status: stable
tags:
  - math
  - math/number-theory
aliases:
  - convolution
  - convolution_ll
  - NTT
  - 畳み込み
  - 整数畳み込み
requires:
  - mod 畳み込みは素数法 2 <= mod <= 2000001000
  - mod 畳み込みの出力長を切り上げた 2 の冪が mod-1 を割る
  - 整数畳み込みは出力長 <= 2^24、全出力係数が long long に収まる
complexity:
  convolution: O((n+m) log(n+m) + log mod)
  convolution_ll: O((n+m) log(n+m))
  memory: O(n+m)
pitfalls:
  - 動的 modint は対応しない
  - 1000000007 の法では長い mod 畳み込みを直接行えない
related:
  - math/modint
verification:
  unit:
    - verify/unit/math/convolution.test.cpp
  stress:
    - verify/stress/math/convolution.test.cpp
  online:
    - verify/online/math/convolution.test.cpp
---

# Convolution

c[k] = sum(a[i] * b[j], i+j=k) を計算します。ACL と同じ NTT と 3 法 CRT を使い、
空列があれば空列を返し、それ以外の出力長は n+m-1 です。

## API

| API | 結果 |
| --- | --- |
| `cp::convolution(const vector<Mint>& a, const vector<Mint>& b)` | static_modint の係数で mod 畳み込み |
| `cp::convolution(vector<Mint>&& a, vector<Mint>&& b)` | 作業用の配列へ所有権を移して畳み込み |
| `cp::convolution<mod = 998244353>(const vector<T>& a, const vector<T>& b)` | 整数型 T を法で正規化し、同じ T で 0 <= c[k] < mod を返す |
| `cp::convolution_ll(const vector<long long>& a, const vector<long long>& b)` | 符号付き整数の正確な畳み込み |

mod 畳み込みの法は素数、2 <= mod <= 2000001000 とします。出力長以上の最小の 2 の冪を
z とすると z が mod-1 を割る必要があります。998244353 では出力長 <= 2^23 です。
小さい方の入力長が 60 以下なら愚直積を使いますが、長さの前提は変わりません。
整数型のオーバーロードでは T が正規化済みの結果を表現できる必要があります。

整数畳み込みは出力長 <= 2^24、各出力係数が long long に収まることが前提です。
入力は負数や long long の両端を含められます。個々の積・部分和が long long を超えても、
最終係数が範囲内なら正確に復元します。任意法の畳み込みや mod 2^64 畳み込みとは
異なる契約です。長さの不正は assert で検出しますが、係数の範囲は検査しません。

## Example

```cpp
#include <cp/math/convolution.hpp>
#include <cassert>
int main() {
    auto c = cp::convolution(std::vector<int>{1,2}, std::vector<int>{3,4});
    assert((c == std::vector<int>{3,10,8}));
    auto d = cp::convolution_ll({-1,2}, {3,-4});
    assert((d == std::vector<long long>{-3,10,-8}));
}
```

## Verification and provenance

空列・負数・符号付き整数の両端、NTT の大きな入力を単体テストし、固定 seed の乱択で
愚直積と比較します。Library Checker の Convolution の全ケースをローカル実行します。
同問題は固定法の検証で、整数畳み込みの符号・範囲はローカルの比較テストが担当します。
[ACL 公式 Convolution API](https://atcoder.github.io/ac-library/production/document_ja/convolution.html)
との差は名前空間のみです。[CC0 ソースの由来](../../third_party/acl-math.md)も参照してください。
