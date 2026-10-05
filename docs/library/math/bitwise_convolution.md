---
title: Bitwise Convolution and Walsh-Hadamard Transform
source: include/cp/math/bitwise_convolution.hpp
status: stable
tags:
  - math
aliases:
  - AND 畳み込み
  - OR 畳み込み
  - XOR 畳み込み
  - Walsh Hadamard transform
  - FWT
requires:
  - T は加減乗算を持つ可換環で、T{} がゼロ
  - XOR の逆変換には配列長による除算が必要
complexity:
  convolution: O(N log(N+1))、N は長い入力長以上の最小の 2 の冪
  walsh_hadamard: O(N log(N+1))、追加領域 O(1)
  memory: 畳み込みは O(N)
pitfalls:
  - XOR を Modint で使う場合、法と配列長が互いに素であること
  - 整数型では中間の加減乗算がすべて範囲内で、逆変換の除算が割り切れること
  - 浮動小数点型では丸め誤差がある
related:
  - math/subset_transform
  - math/modint
verification:
  unit:
    - verify/unit/math/bitwise_convolution.test.cpp
  stress:
    - verify/stress/math/bitwise_convolution.test.cpp
  online:
    - verify/online/math/bitwise_and_convolution.test.cpp
    - verify/online/math/bitwise_or_convolution.test.cpp
    - verify/online/math/bitwise_xor_convolution.test.cpp
---

# Bitwise Convolution and Walsh-Hadamard Transform

添字を AND・OR・XOR で合成する畳み込みを O(N log(N+1)) で求めます。
Modint のほか、中間計算が収まる整数型にも対応します。

## API

| API | 動作 |
| --- | --- |
| `cp::and_convolution(std::vector<T> a, std::vector<T> b)` | i AND j = k の全 a[i]*b[j] を集約 |
| `cp::or_convolution(std::vector<T> a, std::vector<T> b)` | i OR j = k の全 a[i]*b[j] を集約 |
| `cp::xor_convolution(std::vector<T> a, std::vector<T> b)` | i XOR j = k の全 a[i]*b[j] を集約 |
| `cp::walsh_hadamard(std::vector<T>& a, bool inverse = false)` | in-place の XOR 用変換。逆変換は変換後に長さで除算 |

畳み込みは入力を値で受け取り、`std::move` で作業配列の所有権を渡せます。
長い方の入力長以上の最小の 2 の冪までゼロで補完し、その長さの結果を返します。
片方が空なら補完後の零列、両方が空なら空列を返します。
入力長が違う場合や 2 の冪でない場合も、この同じ規則を使います。
出力の添字は補完後の bit 幅で表され、畳み込み後の末尾ゼロを削除しません。

Walsh–Hadamard 単体の入力は長さ `2^k`（k>=0）です。空列や不正な長さは assert で検出します。
順変換は蝶演算 `(x,y) -> (x+y,x-y)`、逆変換は同じ演算の後、全係数を T(N) で割ります。
T(N) が正しく表せることが必要です。Modint では法が奇数なら 2 の冪 N が可逆です。
整数では逆変換の各除算が割り切れることが前提です。整数の順変換結果や、全中間計算が
収まる整数畳み込みはこの条件を満たします。任意の整数列の逆変換は割り切れるとは限りません。
AND・OR には除算が必要ありません。NDEBUG 時も各長さ・代数条件を守ってください。

## Example

```cpp
#include <cp/math/bitwise_convolution.hpp>
#include <cp/math/modint.hpp>
#include <cassert>
#include <vector>

int main() {
    using Mint = cp::modint998244353;
    auto c = cp::xor_convolution(std::vector<Mint>{1, 2}, std::vector<Mint>{3, 4});
    assert(c[0] == 11 && c[1] == 10);
    auto d = cp::or_convolution(std::vector<long long>{1, 2, 3}, {4});
    assert((d == std::vector<long long>{4, 8, 12, 0}));
}
```

## Verification

単体テストは長さ1、空列、零列、補完、負の係数、既知の蝶演算、奇合成数法を確認します。
固定 seed の乱択では全添字対を列挙して整数と Modint の各畳み込みを比較し、変換の復元も確認します。
Library Checker の Bitwise AND / XOR Convolution をローカル検証します。
OR は入力と結果の添字を N-1-i に反転して AND 問題を解きます。
De Morgan の法則 `(~i) OR (~j) = ~(i AND j)` により、同じ公開ケースで OR の全入力を検証できます。
