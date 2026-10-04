---
title: Fenwick Tree
source: include/cp/data_structure/fenwick_tree.hpp
status: stable
tags:
  - data-structure
  - data-structure/range-query
aliases:
  - Fenwick Tree
  - Binary Indexed Tree
  - BIT
  - フェニック木
  - fenwick_tree
requires:
  - 0 <= n <= 100000000、添字 0 <= p < n、区間 0 <= l <= r <= n
  - T は bool 以外の整数型または加法可換群を表す型
complexity:
  build: O(n)
  add/sum: O(log(n + 1))
  size: O(1)
  memory: O(n)
pitfalls:
  - 区間は 0-indexed の半開区間 [l, r)
  - 整数型の総和はその型のビット幅で剰余を取る
  - 一般の T はゼロ初期化、加算代入、減算に対応する必要がある
related: []
verification:
  unit:
    - verify/unit/data_structure/fenwick_tree.test.cpp
  stress:
    - verify/stress/data_structure/fenwick_tree.test.cpp
  online:
    - verify/online/data_structure/fenwick_tree.test.cpp
---

# Fenwick Tree

一点加算と区間和を扱う Binary Indexed Tree です。頻度表、転倒数、差分列の管理に使います。
区間積の演算が加法以外の場合は Segment Tree を使います。

## API

| API | 動作 | 計算量 |
| --- | --- | --- |
| `cp::FenwickTree<T> tree(n = 0)` | n 個のゼロを構築 | O(n) |
| `cp::FenwickTree<T> tree(values)` | `const std::vector<T>&` から構築 | O(n) |
| `tree.size() const` | 要素数を返す | O(1) |
| `tree.add(int p, T x)` | p 番目へ x を加算 | O(log(n + 1)) |
| `tree.sum(int l, int r) const -> T` | [l, r) の総和。空区間はゼロ | O(log(n + 1)) |

`T` の演算は O(1) とします。整数型は内部で対応する符号なし型を使い、
符号付き整数のオーバーフローによる未定義動作を避けます。結果はビット幅の剰余です。
正確な数学的総和が必要な場合は結果が収まる型を選んでください。
Modint 等の型も使えますが、`T{}` がゼロで `+=` と `-` が加法可換群をなす必要があります。
非可換な演算や浮動小数点の厳密な集約には適しません。不正な添字は assert で検出し、
`NDEBUG` 時は呼び出し側が前提を守ります。

## ACL との対応

[ACL Fenwick Tree](https://atcoder.github.io/ac-library/production/document_en/fenwicktree.html)
の `atcoder::fenwick_tree<T>` に `cp::FenwickTree<T>` / `cp::fenwick_tree<T>` が対応し、
`add` / `sum` の添字と動作は同じです。ベクトルからの線形時間構築と `size()` を追加しています。
標準整数型と加法可換群のユーザー定義型を対象とし、コンパイラ固有の整数拡張型は保証しません。

## Example

```cpp
#include <cp/data_structure/fenwick_tree.hpp>
#include <cassert>
#include <vector>
int main() {
    cp::FenwickTree<long long> tree(std::vector<long long>{1, 2, 3, 4});
    tree.add(2, 5);
    assert(tree.sum(1, 4) == 14);
    assert(tree.sum(2, 2) == 0);
}
```

## Verification

空列、負の加算、最小・最大整数付近の剰余、符号なし整数と独自の剰余型を単体検証します。
固定 seed の乱択加算・区間和をベクトルの愚直計算と比較します。
オンライン検証は Library Checker の Point Add Range Sum の公開ケースをローカル実行します。
検証状態とヘッダー全文はドキュメントビルド時に生成されます。
