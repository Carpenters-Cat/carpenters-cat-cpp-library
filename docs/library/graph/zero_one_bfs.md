---
title: 0-1 BFS
source: include/cp/graph/zero_one_bfs.hpp
status: stable
tags:
  - graph
  - graph/shortest-path
aliases:
  - 01 BFS
  - ゼロワン BFS
  - deque 最短路
related:
  - graph/weighted_graph
  - graph/dijkstra
verification:
  unit:
    - verify/unit/graph/zero_one_bfs.test.cpp
  stress:
    - verify/stress/graph/zero_one_bfs.test.cpp
  online:
    - verify/online/graph/zero_one_bfs.test.cpp
---

# 0-1 BFS

## Summary / When to use

重みが 0 または 1 のグラフで単一始点最短路を求めます。
0 重みの更新を deque の先頭へ、1 重みの更新を末尾へ入れます。

## アルゴリズムの考え方

重みが 0/1 だけなら、Dijkstra の優先度付きキューを deque に置き換えられます。
距離 d の頂点から重み 0 の辺を使う候補は d、重み 1 なら d+1 です。
改善した候補を前者では先頭、後者では末尾に入れると、取り出す候補の距離順を保てます。
実装は候補距離も保存し、現在の距離と一致しない古い候補を捨てます。

例えば 0→1 が 1、0→2 が 0、2→1 が 0 なら、2 を先に処理して 1 の距離を 0 にします。
通常の BFS は辺の本数を数えるため、0 の辺を含む重み付き距離には使えません。
非負最短路と同じく最小の有効候補で距離が確定し、各頂点の出辺はその処理で一度走査します。
各辺からの追加・deque 操作は O(1) なので、時間は O(V+E) です。
古い候補を含む deque と結果に O(V+E) の作業メモリを使います。

全辺が 0 または 1 であることを確認してください。「小さな非負整数」一般へそのまま
拡張すると deque の距離順が崩れます。有向・無向の表現や経路復元は Weighted Graph と
共通です。0 の辺の閉路は扱えますが、負辺には対応しません。

## Preconditions / API

```cpp
cp::ShortestPathResult cp::zero_one_bfs(const cp::WeightedGraph& graph, int source);
```

`0 <= source < graph.size()`、**全辺の重みは 0 または 1**。
重み 0 の閉路、全辺 0、全辺 1、自己ループ、多重辺、非連結に対応します。
不正な重みは assert で検出します。一般の非負重みでは Dijkstra を使います。

## Complexity

O(V+E) 時間、O(V+E) の補助メモリ。
有限距離は 0..V-1 なので int64 の範囲内です。
経路復元は O(L)、再帰スタックを使いません。

## Examples

```cpp
#include <cp/graph/zero_one_bfs.hpp>
#include <cassert>
int main() {
    cp::WeightedGraph g(3);
    g.add_edge(0,1,1); g.add_edge(0,2,0); g.add_edge(2,1,0);
    auto result=cp::zero_one_bfs(g,0);
    assert(result.distance[1]==0);
    assert(result.path_to(1)->vertices.size()==3);
}
```

## Verification

単体テストはゼロ閉路、並行する候補、非連結、全辺 0/1 の 20 万頂点の鎖を確認します。
固定 seed の乱択 0/1 グラフで全始点の Dijkstra と比較し、全有限経路の辺・費用を確認します。
AOJ ALDS1_11_C のオンライン検証は全辺が 1 の場合を検証します。
0 重みを含む場合は単体・乱択比較が検証します。
## Distance and path representation

`cp::Distance` は `std::int64_t` です。`distance[v]` は `optional<Distance>`、
`state[v]` は `finite` / `unreachable` / `negative_infinity` です。
距離が有限の場合だけ optional に値が入ります。INF という予約値はありません。
`INT64_MIN` と `INT64_MAX` 自体も有限距離として使えます。

`predecessor_vertex[v]` と `predecessor_edge[v]` は前駆頂点と元の辺 ID です。
`path_to(v)` は `optional<ShortestPath>` を返し、有限の場合は始点から v までの
`vertices` と `edges` を返します。始点への空経路は頂点 1 個・辺 0 個です。
到達不能・負閉路の影響がある場合は nullopt です。
結果の各フィールドは参照用とし、経路復元の前に書き換えないでください。

## Numeric bounds / overflow policy

辺重みは int64 全域、返せる有限距離は
**[-9223372036854775808, 9223372036854775807]** です。
最終的な有限距離がこの範囲を超えた場合、関数全体が `std::overflow_error` を投げます。
到達不能や負閉路の影響を受ける頂点は距離へ変換しません。
候補の加算は GCC/Clang の `__int128_t` で行うため、最短距離より長い候補が
int64 を超えていても誤った例外や切り詰めは起こりません。
macOS/Linux の GCC/Clang を対象とします。浮動小数点・MSVC は対象外です。
