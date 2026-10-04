---
title: Kruskal / Minimum Spanning Forest
source: include/cp/graph/kruskal.hpp
status: stable
tags:
  - graph
  - graph/connectivity
aliases:
  - Kruskal
  - 最小全域木
  - 最小全域森
related:
  - graph/weighted_graph
  - data_structure/union_find
verification:
  unit:
    - verify/unit/graph/kruskal.test.cpp
  stress:
    - verify/stress/graph/kruskal.test.cpp
  online:
    - verify/online/graph/kruskal.test.cpp
---

# Kruskal / Minimum Spanning Forest

## Summary / When to use

無向重み付きグラフの各連結成分の最小全域木を Kruskal 法で求めます。
既存の `cp::UnionFind` で閉路を判定します。

## Preconditions / API

```cpp
cp::MinimumSpanningForest cp::kruskal(const cp::WeightedGraph& graph);
```

`WeightedGraph` の **各レコードを 1 本の無向候補辺**として扱います。
無向辺は `add_edge(a,b,w)` を 1 回呼んで登録します。両方向を登録した場合は
異なる ID を持つ多重辺です。有向最小全域木を求める API ではありません。
頂点 ID は 0 以上 V 未満。負重み・同重み・自己ループ・多重辺・非連結に対応します。

戻り値の `cost` は全成分の合計費用、`edge_ids` は元グラフの辺 ID、
`components` は成分数です。`connected()` は成分数 0 または 1 のとき true。
空グラフは費用 0、辺なし、成分数 0 とします。
選択辺は (重み, 元の ID) の昇順で貪欲に選び、その順番で返します。
同重みの選択も決定的です。返された辺数は V-components です。

## Numeric contract

重みと返す費用は `cp::Distance` (`int64_t`) 全域を使えます。
和を GCC/Clang の `__int128_t` で計算し、**最終費用だけ** int64 の範囲を確認します。
最終費用が範囲外なら `std::overflow_error`。途中の和が範囲外でも最終和が収まれば返します。
V は int に収まり、選択辺は V-1 以下なので 128 bit の和は範囲内です。
macOS/Linux の GCC/Clang が対象です。

## Complexity

O(V+E log(E+1)) 時間、O(V+E) 補助メモリ。

## Examples

```cpp
#include <cp/graph/kruskal.hpp>
#include <cassert>
int main() {
    cp::WeightedGraph g(4);
    auto a=g.add_edge(0,1,-2); auto b=g.add_edge(1,2,3);
    g.add_edge(0,2,7);
    auto forest=cp::kruskal(g);
    assert(forest.cost==1 && forest.components==2 && !forest.connected());
    assert((forest.edge_ids==std::vector<std::size_t>{a,b}));
}
```

## Verification

単体で負辺・自己ループ・多重辺・同重み・非連結・空グラフ・int64 境界と
途中/最終オーバーフローを確認します。固定 seed の小グラフでは全辺部分集合を列挙し、
ラベル伝播による連結性・最小費用・選択 ID を比較します。
[Library Checker Minimum Spanning Tree](https://judge.yosupo.jp/problem/minimum_spanning_tree)
の費用と選択辺 ID を検証します。
