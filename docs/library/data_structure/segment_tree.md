---
title: Segment Tree
source: include/cp/data_structure/segment_tree.hpp
status: stable
tags:
  - data-structure
  - data-structure/range-query
aliases:
  - Segment Tree
  - セグメント木
  - セグ木
  - segtree
requires:
  - 0 <= n <= 100000000、添字 0 <= p < n、区間 0 <= l <= r <= n
  - op は結合的な演算、e() は左右の単位元
  - 境界探索の述語は単位元で true、区間拡大に対して単調で副作用なし
complexity:
  build: O(n)
  set/prod/max_right/min_left: O(log(n + 1))
  get/all_prod/size: O(1)
  memory: O(n)
pitfalls:
  - 区間は 0-indexed の半開区間 [l, r)
  - 非可換な演算では左から右の順序で集約する
  - 演算のオーバーフローは呼び出し側で防ぐ
related: []
verification:
  unit:
    - verify/unit/data_structure/segment_tree.test.cpp
  stress:
    - verify/stress/data_structure/segment_tree.test.cpp
  online:
    - verify/online/data_structure/segment_tree.test.cpp
---

# Segment Tree

モノイドによる一点更新・区間積・境界探索です。区間和、最小値、関数の合成等に使います。
`op(S, S) -> S` と `e() -> S` をテンプレート引数で指定します。可換性は不要です。

## API

| API | 動作 | 計算量 |
| --- | --- | --- |
| `cp::SegmentTree<S, op, e> tree(n = 0)` | n 個の `e()` を構築 | O(n) |
| `cp::SegmentTree<S, op, e> tree(values)` | `const std::vector<S>&` から構築 | O(n) |
| `tree.size() const -> int` | 要素数 | O(1) |
| `tree.set(int p, S x)` | p 番目を x に置換 | O(log(n + 1)) |
| `tree.get(int p) const -> S` | p 番目の値 | O(1) |
| `tree.prod(int l, int r) const -> S` | [l, r) の積。空区間は `e()` | O(log(n + 1)) |
| `tree.all_prod() const -> S` | 全区間の積。空列は `e()` | O(1) |
| `tree.max_right(int l, Predicate f) const -> int` | `f(prod(l, r))` を満たす最大の r | O(log(n + 1)) |
| `tree.min_left(int r, Predicate f) const -> int` | `f(prod(l, r))` を満たす最小の l | O(log(n + 1)) |
| `tree.max_right<f>(int l) const -> int` | コンパイル時の述語で右境界探索 | O(log(n + 1)) |
| `tree.min_left<f>(int r) const -> int` | コンパイル時の述語で左境界探索 | O(log(n + 1)) |

演算と述語は O(1)、`S` のコピーも O(1) とします。それ以外は各操作のコストを乗じてください。
境界探索では `0 <= l, r <= n`、`f(e()) == true` が必要です。
右探索では区間を右へ、左探索では区間を左へ拡大した時、一度 false になったら true に戻らない述語を使います。
例えば非負要素の総和が閾値以下かという述語は使えますが、負数が混在する場合は保証できません。
不正な添字と `f(e())` は assert で検出し、`NDEBUG` 時は呼び出し側が前提を守ります。

## ACL との対応

[ACL Segtree](https://atcoder.github.io/ac-library/production/document_en/segtree.html)
の `atcoder::segtree<S, op, e>` に `cp::SegmentTree<S, op, e>` / `cp::segtree<S, op, e>` が対応します。
公開操作・テンプレート引数順・半開区間・境界探索の条件は同じです。`size()` を追加し、読み取り操作は const です。

## Example

```cpp
#include <cp/data_structure/segment_tree.hpp>
#include <cassert>
#include <vector>
long long op(long long a, long long b) { return a + b; }
long long e() { return 0; }
int main() {
    cp::SegmentTree<long long, op, e> tree(std::vector<long long>{1, 2, 3, 4});
    tree.set(1, 5);
    assert(tree.prod(1, 3) == 8);
    assert(tree.max_right(0, [](long long sum) { return sum <= 9; }) == 3);
    assert(tree.min_left(4, [](long long sum) { return sum <= 7; }) == 2);
}
```

## Verification

空列、単一要素、非 2 冪長、空区間、両端、即時失敗・全区間成功の境界探索を単体検証します。
乱択一点更新・積・両方向の境界を愚直計算と比較し、文字列連結でも非可換な順序を確認します。
オンライン検証は Library Checker の Point Set Range Composite の公開ケースをローカル実行します。
検証状態とヘッダー全文はドキュメントビルド時に生成されます。
