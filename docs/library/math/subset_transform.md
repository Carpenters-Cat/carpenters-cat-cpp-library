---
title: Subset and Superset Transforms
source: include/cp/math/subset_transform.hpp
status: stable
tags:
  - math
  - dp
aliases:
  - 部分集合ゼータ変換
  - 上位集合ゼータ変換
  - メビウス変換
  - subset zeta transform
  - superset mobius transform
  - SOS DP
requires:
  - 入力長は 2^k、k >= 0（空列は不可）
  - T の加算は可換で結合的、逆変換には減算が必要
complexity:
  transform: O(N log(N+1))、N は配列長
  memory: 追加 O(1)、入力をその場で変更
pitfalls:
  - 添字は k bit の集合を表す。k = 0 の入力長は 1
  - 整数の全中間計算が型の範囲に収まることを呼び出し側が保証する
  - NDEBUG 時も長さの前提を守る
related:
  - math/bitwise_convolution
verification:
  unit:
    - verify/unit/math/subset_transform.test.cpp
  stress:
    - verify/stress/math/subset_transform.test.cpp
  online:
    - verify/online/math/bitwise_and_convolution.test.cpp
    - verify/online/math/bitwise_or_convolution.test.cpp
---

# Subset and Superset Transforms

集合の部分集合・上位集合に沿って値を集約し、その逆変換を行います。
bit を一つずつ処理する in-place の変換です。整数や Modint を使用できます。
通常の浮動小数点では演算誤差により完全な復元は保証しません。

## API

| API | 入力 a から得られる値 |
| --- | --- |
| `cp::subset_zeta(std::vector<T>& a)` | a[S] = 元の a[T] の総和（T は S の部分集合） |
| `cp::subset_mobius(std::vector<T>& a)` | 部分集合ゼータ変換の逆 |
| `cp::superset_zeta(std::vector<T>& a)` | a[S] = 元の a[T] の総和（T は S の上位集合） |
| `cp::superset_mobius(std::vector<T>& a)` | 上位集合ゼータ変換の逆 |

すべて返り値は void で、入力を変更します。長さは `2^k`、`k >= 0` です。
空列や 2 の冪でない長さは assert で検出します。
ゼータ変換は `+=`、メビウス変換は `-=` を使用します。
演算の計算量を O(1) として、時間 O(N log(N+1))、追加領域 O(1) です。

OR 畳み込みは部分集合変換、AND 畳み込みは上位集合変換を使用します。
共通処理はこのヘッダーにのみ実装し、畳み込みヘッダーから依存を読み込みます。

## Example

```cpp
#include <cp/math/subset_transform.hpp>
#include <cassert>
#include <vector>

int main() {
    std::vector<int> a{1, 2, 3, 4};
    cp::subset_zeta(a);
    assert((a == std::vector<int>{1, 3, 4, 10}));
    cp::subset_mobius(a);
    assert((a == std::vector<int>{1, 2, 3, 4}));
}
```

## Verification

単体テストは k=0、零列、既知の集約結果を確認します。固定 seed の乱択テストは
部分集合・上位集合を列挙した総和と比較し、両方向の変換順で復元を確認します。
オンライン検証では、AND と OR の畳み込みから全4変換を使用します。
OR は De Morgan の添字補集合により Library Checker の AND 問題へ厳密に変換します。
