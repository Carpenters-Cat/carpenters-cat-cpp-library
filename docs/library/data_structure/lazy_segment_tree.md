---
title: Lazy Segment Tree
source: include/cp/data_structure/lazy_segment_tree.hpp
status: stable
tags:
  - data-structure
  - data-structure/range-query
aliases:
  - Lazy Segment Tree
  - 遅延セグメント木
  - 遅延評価セグメント木
  - lazy_segtree
requires:
  - 0 <= n <= 100000000、添字 0 <= p < n、区間 0 <= l <= r <= n
  - op/e はモノイド、mapping は op を保つ作用、composition は作用の合成、id は恒等作用
  - 境界探索の述語は単位元で true、区間拡大に対して単調で副作用なし
complexity:
  build: O(n)
  set/get/prod/apply/max_right/min_left: O(log(n + 1))
  all_prod/size: O(1)
  memory: O(n)
pitfalls:
  - composition(f, g) は g の後に f を作用させる順序
  - 区間和に区間加算する時は S に区間長を持たせる
  - get/prod/境界探索は遅延作用を伝播するので const ではない
related: []
verification:
  unit:
    - verify/unit/data_structure/lazy_segment_tree.test.cpp
  stress:
    - verify/stress/data_structure/lazy_segment_tree.test.cpp
  online:
    - verify/online/data_structure/lazy_segment_tree.test.cpp
---

# Lazy Segment Tree

モノイドの積と区間への作用を遅延評価するデータ構造です。区間加算・区間和、区間代入・最小値、
区間 affine 変換等に使います。可換性は不要で、積は左から右の順序です。

## 前提

`op(S, S) -> S` は結合的、`e() -> S` は左右の単位元です。
`mapping(F f, S x) -> S` は区間の集約値 x に f を作用させます。
`mapping(f, op(x, y)) == op(mapping(f, x), mapping(f, y))` が必要です。
`composition(F f, F g) -> F` は **g を先に、f を後に** 作用させる合成であり、
`mapping(composition(f, g), x) == mapping(f, mapping(g, x))` を満たします。
`id() -> F` は恒等作用で、合成後の作用も F に収まる必要があります。
要素・更新の演算で生じるオーバーフローは呼び出し側で防いでください。

## アルゴリズムの考え方

Segment Tree のノードに区間の集約値と「子にまだ渡していない更新」を持たせます。
更新区間がノード全体を覆うなら、集約値に `mapping` を適用して更新を記録し、
その時点では葉まで降りません。区間の一部に触れる操作の前に保留更新を子へ渡します。
これが遅延伝播です。長い区間の更新でも処理する境界と被覆ノードは O(log n) 個です。

例えば区間加算・区間和では集約値を `(sum, length)` とし、加算 d に対して
`mapping(d, (sum,length))=(sum+d*length,length)` とします。
和だけを持つと「何個の要素に加えたか」が分からず、正しい区間更新を計算できません。
空区間の単位元は `(0,0)` です。

正しさの鍵は「ノードの集約値には保留中の更新も反映済み」であることです。
子へ配るまでは子の値だけが古いので、検索・一点変更の経路では上から順に伝播し、
部分更新後は必要な祖先を再計算します。`mapping` が区間結合と整合し、更新の
`composition` が合成を表すなら、この手順で不変条件を維持できます。

合成は `composition(f,g)` が g の後に f を実行する順です。
アフィン更新 `g(x)=c*x+d` の後に `f(x)=a*x+b` を行うなら、合成は
`(a*c, a*d+b)` です。加算だけの例では合成が可換で誤りに気付きにくいため、
代入やアフィン変換も考えて演算を設計してください。
演算を O(1) としたとき各更新・検索は O(log(n + 1))、構築・メモリは O(n) です。
区間ごとの更新を一定サイズの集約値と作用で表せない処理には、この型は使えません。

## API

型は `cp::LazySegmentTree<S, op, e, F, mapping, composition, id>` です。

| API | 動作 | 計算量 |
| --- | --- | --- |
| `Tree tree(n = 0)` | n 個の `e()` を構築 | O(n) |
| `Tree tree(values)` | `const std::vector<S>&` から構築 | O(n) |
| `tree.size() const -> int` | 要素数 | O(1) |
| `tree.set(int p, S x)` | p 番目を x に置換 | O(log(n + 1)) |
| `tree.get(int p) -> S` | p 番目の現在値 | O(log(n + 1)) |
| `tree.prod(int l, int r) -> S` | [l, r) の積。空区間は `e()` | O(log(n + 1)) |
| `tree.all_prod() const -> S` | 全区間の積。空列は `e()` | O(1) |
| `tree.apply(int p, F f)` | p 番目へ f を作用 | O(log(n + 1)) |
| `tree.apply(int l, int r, F f)` | [l, r) の各要素へ f を作用 | O(log(n + 1)) |
| `tree.max_right(int l, Predicate g) -> int` | `g(prod(l, r))` を満たす最大の r | O(log(n + 1)) |
| `tree.min_left(int r, Predicate g) -> int` | `g(prod(l, r))` を満たす最小の l | O(log(n + 1)) |
| `tree.max_right<g>(int l) -> int` | コンパイル時の述語で右境界探索 | O(log(n + 1)) |
| `tree.min_left<g>(int r) -> int` | コンパイル時の述語で左境界探索 | O(log(n + 1)) |

演算・作用・述語・コピーは O(1) とします。それ以外は各操作のコストを乗じてください。
境界探索には `0 <= l, r <= n` と `g(e()) == true` が必要です。
右探索では右へ、左探索では左へ区間を拡大した時、一度 false になったら true に戻らない述語を使います。
区間和の非負性等は更新後も保つ必要があります。不正な添字と `g(e())` は assert で検出し、
`NDEBUG` 時は呼び出し側が前提を守ります。

## ACL との対応

[ACL Lazy Segtree](https://atcoder.github.io/ac-library/production/document_en/lazysegtree.html)
の `atcoder::lazy_segtree<S, op, e, F, mapping, composition, id>` に
`cp::LazySegmentTree<...>` / `cp::lazy_segtree<...>` が対応します。
公開操作・テンプレート引数順・作用合成順・半開区間・境界探索の条件は同じで、`size()` を追加しています。

## Example

区間加算・区間和では集約に長さを持たせます。整数の総和が long long に収まる範囲で使ってください。

```cpp
#include <cp/data_structure/lazy_segment_tree.hpp>
#include <cassert>
#include <vector>
struct S { long long sum; int length; };
S op(S x, S y) { return {x.sum + y.sum, x.length + y.length}; }
S e() { return {0, 0}; }
S mapping(long long add, S x) { return {x.sum + add * x.length, x.length}; }
long long composition(long long f, long long g) { return f + g; }
long long id() { return 0; }
int main() {
    cp::LazySegmentTree<S, op, e, long long, mapping, composition, id>
        tree(std::vector<S>{{1, 1}, {2, 1}, {3, 1}, {4, 1}});
    tree.apply(1, 4, 2);
    assert(tree.prod(0, 4).sum == 16);
    assert(tree.max_right(0, [](S x) { return x.sum <= 10; }) == 3);
    assert(tree.min_left(4, [](S x) { return x.sum <= 11; }) == 2);
}
```

`Tree(n)` は各要素を `e()` にするため、この例では長さも 0 になります。
実際の n 個のゼロを扱うには `std::vector<S>(n, {0, 1})` を渡します。

## Verification

空列、単一要素、非 2 冪長、空区間、全区間更新、更新後の set/get、非可換な affine 合成順を単体検証します。
乱択区間作用・一点作用・一点置換・積・両方向境界を愚直計算と比較します。
文字列連結と文字シフトでも積の非可換な順序と遅延作用を確認します。
オンライン検証は Library Checker の Range Affine Range Sum の公開ケースをローカル実行します。
検証状態とヘッダー全文はドキュメントビルド時に生成されます。
