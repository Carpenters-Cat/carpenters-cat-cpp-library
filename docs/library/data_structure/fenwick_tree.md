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

## アルゴリズムの考え方

配列全体の累積和を毎回作り直さず、長さが 2 の冪になる区間の和を保存します。
説明用に内部位置を 1-indexed の i とすると、i 番目は
`[i - lowbit(i), i)` の元配列の和です。`lowbit(i)` は i の最下位の 1 ビットが表す値です。
例えば先頭 7 要素の和は `[0,4)`、`[4,6)`、`[6,7)` の 3 区間に分かれます。

前方の和を求めるときは i から `lowbit(i)` を引き、互いに重ならない区間を加えます。
これで先頭の全要素を過不足なく覆うため、区間和は `prefix(r) - prefix(l)` です。
一点加算では、その点を含む保存区間だけを上方向へたどって更新します。
実装は配列を 0-indexed に置き、検索で `r &= r - 1`、更新で `i |= i + 1` を使います。

各移動で処理するビット位置が変わるので、検索・更新は O(log(n + 1)) です。
ベクトルからの構築では、各位置の部分和を `i | (i + 1)` の親へ一度だけ渡し、
O(n) で完成させます。要素を順に `add` する構築の O(n log n) より速くなります。

転倒数では左から値 x を読み、圧縮後の順位を p として「処理済みの個数 − p 以下の個数」
を加えた後、p の頻度を 1 増やします。等しい値を転倒に数えない境界に注意します。
一点代入が必要なら元の値を別途持ち、差分を `add` してください。
任意の区間から前方区間を引き算するため、最小値のように逆演算のない集約には適しません。

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
