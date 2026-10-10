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

## アルゴリズムの考え方

ビット集合 S を配列の添字とし、subset zeta は `F[S]=sum_{T⊆S} f[T]`、
superset zeta は `G[S]=sum_{T⊇S} f[T]` を求めます。
集合の各ビットを一つずつ扱い、そのビットが 0 の状態から 1 の状態へ加えると、
subset ではその要素を含む・含まない両方の部分集合が集約されます。
superset は逆方向へ加えます。

処理済みビットだけの選択肢を全て足した状態を不変条件にすると、全ビット処理後に
ちょうど目的の包含関係の全ての集合を一度ずつ数えます。
例えば `[1,2,3,4]` は 2 ビット集合の値で、subset zeta の結果は `[1,3,4,10]` です。
最後の要素は空集合・2 個の単集合・全集合の値を全て含みます。

Möbius 変換は同じビットごとの加算を減算へ置き換えます。
各段の変換が可逆で、異なるビットの作用が交換できるため、元の値を復元できます。
そのため zeta は加算、Möbius は減算まで使える型が必要です。

配列長 N=2^k に対し、k 段で各 N 要素を走査するので時間は O(N log N)、追加領域は O(1) です。
空列や 2 の冪でない長さは直接変換できません。マスクの全域を用意してください。
包含関係の集約を積へ利用する AND/OR 畳み込みは Bitwise Convolution にあります。

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
