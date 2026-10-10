---
title: Dijkstra
source: include/cp/graph/dijkstra.hpp
status: stable
tags:
  - graph
  - graph/shortest-path
aliases:
  - ダイクストラ
  - 非負最短路
  - SSSP
related:
  - graph/weighted_graph
verification:
  unit:
    - verify/unit/graph/dijkstra.test.cpp
  stress:
    - verify/stress/graph/dijkstra.test.cpp
  online:
    - verify/online/graph/dijkstra.test.cpp
---

# Dijkstra

## Summary / When to use

隣接リストの非負重みグラフで単一始点最短路を求めます。
優先度付きキューを使い、古くなった候補を除外します。

## アルゴリズムの考え方

暫定距離が最小の頂点を優先度付きキューで取り出し、その出辺を緩和します。
緩和は辺 u→v に対して `distance[u]+weight` が v の距離より小さければ置き換える操作です。
始点は 0、その他は未到達として開始します。
全辺が非負なら、未確定の頂点を経由しても今の最小距離より短い経路は作れません。
この性質により、最小候補から順に取り出すと距離を確定できます。

例えば 0→1 が 5、0→2 が 1、2→1 が 2 なら、2 を先に処理して 1 の距離を 3 にします。
実装はキュー内の既存項目を書き換えず、改善ごとに新しい `(距離,頂点)` を追加します。
古い距離の候補が出てきたら現在の距離と比較して捨てるので、古い候補による再走査を避けます。
経路の直前の辺も改善時に記録します。同距離の経路の選択には依存しないでください。

成功する緩和とキュー追加は O(E)、キューサイズも O(E+1) なので、並行辺を許す
この実装の時間は O(V+E log(E+2))、作業メモリは O(V+E) です。
単純グラフでは log E を O(log V) と書けます。
負辺があると最小候補を確定する議論が成立しないため Bellman–Ford を使います。
始点から届かない部分の辺も非負であることが API の前提です。

## Preconditions / API

```cpp
cp::ShortestPathResult cp::dijkstra(const cp::WeightedGraph& graph, int source);
```

`0 <= source < graph.size()`、**全辺の重みは 0 以上**。
始点から到達不能な辺もこの前提を守る必要があります。
重み 0 の閉路、自己ループ、多重辺、非連結グラフに対応します。
負辺を含む場合は Bellman–Ford を使います。不正な重みは assert で検出します。

## Complexity

O(V+E log(E+1)) 時間、O(V+E) の補助メモリ。
辺候補をキューに保持するため、並行辺のある場合も E を含む上界を使います。
W を最大辺重みとすると、候補の絶対値は VW 以下で、V は int に収まるので
広い整数型の加算は範囲内です。最終結果は上記の int64 範囲を確認します。

## Examples

```cpp
#include <cp/graph/dijkstra.hpp>
#include <cassert>
int main() {
    cp::WeightedGraph g(4);
    g.add_edge(0,1,5); auto id=g.add_edge(0,1,2);
    g.add_edge(1,2,0);
    auto result=cp::dijkstra(g,0);
    assert(result.distance[2]==2);
    assert(result.path_to(2)->edges.front()==id);
    assert(result.state[3]==cp::DistanceState::unreachable);
}
```

## Verification

単体テストは多重辺、自己ループ、ゼロ閉路、非連結、int64 上限、
過大な候補を無視できること、有限距離のオーバーフロー例外を確認します。
20 万頂点の経路を反復処理で復元します。固定 seed の乱択では
単純経路の全列挙と Floyd の全点対距離を比較し、経路の辺と費用も確認します。
Library Checker の Shortest Path は距離と復元経路の両方をオンライン検証します。
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
