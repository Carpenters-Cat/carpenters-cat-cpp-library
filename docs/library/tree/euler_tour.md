---
title: Euler Tour
source: include/cp/tree/euler_tour.hpp
status: stable
tags:
  - graph
  - graph/connectivity
  - data-structure/range-query
aliases:
  - オイラーツアー
  - DFS order
  - 部分木区間
  - in out
related:
  - tree/tree
  - data_structure/fenwick_tree
verification:
  unit:
    - verify/unit/tree/euler_tour.test.cpp
  stress:
    - verify/stress/tree/euler_tour.test.cpp
  online:
    - verify/online/tree/euler_tour.test.cpp
---

# Euler Tour

## Summary / When to use

木の各部分木を DFS の初回訪問順における連続した半開区間へ写します。
Fenwick Tree や Segment Tree と接続して部分木の更新・集約に使います。
各頂点が 1 回だけ現れる V 要素の列です。歩道としての 2V-1 要素のツアーではありません。

## アルゴリズムの考え方

DFS で頂点を初めて訪れる順に 0,1,2,... と番号を付け、木を一次元の配列に置きます。
DFS は v の全ての子孫を訪問してから v の外へ戻るので、v の部分木は連続区間です。
この実装では `in[v]` を訪問位置、`out[v]=in[v]+subtree_size[v]` とし、
部分木を半開区間 `[in[v],out[v])` へ変換します。

例えば根 0、辺 0–1、1–2、1–3 で訪問順が `[0,1,2,3]` なら、
1 の部分木は `[1,4)` です。頂点の値を `array[in[v]]=value[v]` と置けば、
部分木の総和は Fenwick Tree や Segment Tree の区間和で求められます。
区間作用を使えば部分木への一括更新にも利用できます。

番号付けと部分木サイズの計算は O(V)、各頂点の区間取得は O(1) です。
実際の集約の計算量は組み合わせるデータ構造によります。
ここでいう Euler Tour は「各頂点を一度並べる preorder」であり、戻りも記録する
長さ 2V-1 の DFS 歩道とは異なります。この配列だけをそのまま LCA の RMQ に使わないでください。
根を変えると部分木も変わり、木の辺の更新も反映しないため、必要なら再構築します。

## API

| API | 意味 |
| --- | --- |
| `cp::EulerTour(tree, root = 0)` | 隣接順を保つ DFS 順を構築 |
| `size()` / `root()` | 頂点数・根 |
| `parent(v)` / `depth(v)` | 親頂点・根からの辺数 |
| `in(v)` | v の訪問添字 |
| `out(v)` | v の部分木の終端添字 |
| `subtree(v) -> pair<int,int>` | `[in(v),out(v))` |
| `vertex(index)` | 添字から元の頂点へ |
| `order() const -> const vector<int>&` | 全訪問順 |

部分木区間は v 自身を含みます。根の区間は `[0,V)`、葉の区間は長さ 1。
`vertex(in(v))==v` です。子の訪問順は元の `Tree::neighbors(v)` の順序です。
`out(v)` は訪問される別の頂点の ID ではなく、配列の半開終端です。

## Complexity

構築 O(V)、全取得 O(1)、メモリ O(V)。
区間に接続するデータ構造の操作コストは別途必要です。

## Examples

```cpp
#include <cp/tree/euler_tour.hpp>
#include <cassert>
int main() {
    cp::Tree tree(4);
    tree.add_edge(0,1); tree.add_edge(1,2); tree.add_edge(1,3);
    cp::EulerTour tour(tree);
    auto [left,right]=tour.subtree(1);
    assert(right-left==3 && tour.vertex(left)==1);
    assert(tour.subtree(0)==std::pair(0,4));
}
```

## Verification

単一頂点、隣接順、根の変更、20 万頂点のパスを確認します。
乱択では親をたどる部分木の愚直列挙と区間の頂点集合を比較し、
元の隣接順に沿う DFS 順も比較します。
Library Checker の Vertex Add Subtree Sum を既存 Fenwick Tree と合わせて検証します。
## Input contract

`cp::Tree` は 0 始まりの無向木です。`add_edge(u,v)` は両方向の隣接辺を追加します。
非空の場合、n-1 本の辺を持つ連結・閉路なしの木と有効な根が必要です。
前処理は辺数・閉路・連結性・根を検証し、不正なら `std::invalid_argument` を投げます。
頂点番号などの個々の参照・クエリは assert で検証し、NDEBUG 時は呼び出し側が守ります。
空の Tree は根引数を省略または 0 にすると構築でき、`size()==0`、`root()==-1`。
空の木への頂点クエリはできません。木を変更したら前処理を作り直します。
前処理後のデータは所有しており、元の Tree の寿命に依存しません。
全ての前処理は反復処理で、深い木でも再帰スタックを使いません。
