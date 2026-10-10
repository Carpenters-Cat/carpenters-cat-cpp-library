---
title: LCA and Binary Lifting
source: include/cp/tree/lca.hpp
status: stable
tags:
  - graph
  - graph/connectivity
aliases:
  - LCA
  - Lowest Common Ancestor
  - 最近共通祖先
  - ダブリング
  - 祖先ジャンプ
related:
  - tree/tree
verification:
  unit:
    - verify/unit/tree/lca.test.cpp
  stress:
    - verify/stress/tree/lca.test.cpp
  online:
    - verify/online/tree/lca.test.cpp
---

# LCA and Binary Lifting

## Summary / When to use

木の最近共通祖先、祖先へのジャンプ、頂点間の辺数を求めます。
任意の根から親を求め、2 の冪の長さの祖先を前処理します。

## アルゴリズムの考え方

二分累乗と同じ発想で、各頂点から `2^k` 個上の祖先を表に保存します。
`up[k][v]=up[k-1][up[k-1][v]]` により、2 回の半分のジャンプを合成して作れます。
親が存在しなければ -1 です。祖先へ s 段進む jump は、s の 1 のビットのジャンプを
合成するので O(log(V+1)) です。根より上を要求した場合は -1 を返します。

LCA（最も深い共通祖先）は、まず深い方を持ち上げて深さをそろえます。
一致すればその頂点が答えです。違うなら大きな k から見て、飛んだ先の祖先が異なる
ジャンプだけを両方に適用します。共通祖先の手前に留まりながら距離を縮めるため、
最後に両頂点の親が一致し、それが最も深い共通祖先になります。

例えば根 0、辺 0–1、1–2、1–3 では LCA(2,3)=1 です。
u→v の道は共通祖先まで上り、そこから下るので、辺数は
`depth[u]+depth[v]-2*depth[lca(u,v)]` になります。
この distance は重みの和ではなく辺数です。

前計算とメモリは O(V log(V+1))、jump・LCA・distance は O(log(V+1)) です。
木や根は固定です。部分木の区間だけが必要なら Euler Tour、経路を区間データ構造で
処理したいなら HLD を選ぶと、用途に合う前計算になります。

## API

| API | 意味 |
| --- | --- |
| `cp::Lca(tree, root = 0)` | 二分累乗による祖先表を構築 |
| `size()` / `root()` | 頂点数・根 |
| `parent(v)` / `parent_edge(v)` | 親頂点と元の親辺 ID。根では -1 / nullopt |
| `depth(v)` | 根からの辺数 |
| `jump(v, uint64_t steps) -> int` | steps 本だけ親へ移動。存在しなければ -1 |
| `lca(u,v) -> int` | 根に対する最近共通祖先 |
| `distance(u,v) -> int64_t` | u-v パスの辺数 |

`jump(v,0)==v`、根の 1 つ上や深さを超える祖先は -1 です。
steps は符号なし整数で、最大値を渡した場合も範囲を確認して -1 を返します。
`lca(v,v)==v`、`distance(v,v)==0`。重み付き距離は含みません。
距離は 0..V-1、途中の加減算も int64 で行います。

## Complexity

構築 O(V log(V+1))、LCA / jump O(log(V+1))、距離 O(log(V+1))。
親・深さ・根の取得 O(1)、メモリ O(V log(V+1))。

## Examples

```cpp
#include <cp/tree/lca.hpp>
#include <cassert>
int main() {
    cp::Tree tree(4);
    tree.add_edge(0,1); tree.add_edge(1,2); tree.add_edge(1,3);
    cp::Lca ancestor(tree);
    assert(ancestor.lca(2,3)==1 && ancestor.distance(2,3)==2);
    assert(ancestor.jump(2,2)==0 && ancestor.jump(2,3)==-1);
}
```

## Verification

単一頂点、パス、スター、同一頂点、非 0 の根、存在しない祖先、最大 steps、
20 万頂点の深い木を単体確認します。乱択では BFS で根付けし、
親をたどる愚直 LCA・全祖先・距離と比較します。
オンライン検証は Library Checker の Lowest Common Ancestor です。
## Input contract

`cp::Tree` は 0 始まりの無向木です。`add_edge(u,v)` は両方向の隣接辺を追加します。
非空の場合、n-1 本の辺を持つ連結・閉路なしの木と有効な根が必要です。
前処理は辺数・閉路・連結性・根を検証し、不正なら `std::invalid_argument` を投げます。
頂点番号などの個々の参照・クエリは assert で検証し、NDEBUG 時は呼び出し側が守ります。
空の Tree は根引数を省略または 0 にすると構築でき、`size()==0`、`root()==-1`。
空の木への頂点クエリはできません。木を変更したら前処理を作り直します。
前処理後のデータは所有しており、元の Tree の寿命に依存しません。
全ての前処理は反復処理で、深い木でも再帰スタックを使いません。
