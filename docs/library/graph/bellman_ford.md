---
title: Bellman–Ford
source: include/cp/graph/bellman_ford.hpp
status: stable
tags:
  - graph
  - graph/shortest-path
aliases:
  - Bellman Ford
  - ベルマンフォード
  - 負閉路
  - 負辺最短路
related:
  - graph/weighted_graph
verification:
  unit:
    - verify/unit/graph/bellman_ford.test.cpp
  stress:
    - verify/stress/graph/bellman_ford.test.cpp
  online:
    - verify/online/graph/bellman_ford.test.cpp
---

# Bellman–Ford

## Summary / When to use

負辺を含む有向グラフで単一始点最短路を求めます。
始点から到達可能な負閉路と、その下流の頂点を識別します。

## アルゴリズムの考え方

辺数を制限した最短距離を順に改善する動的計画法です。
各ラウンドで前のラウンドの距離だけを参照して全辺を緩和します。
始点だけを 0 とする初期状態から、k 回後には高々 k 本の辺を使う歩道の最小重みが得られます。
同じラウンドで更新した距離を参照しない同期的な緩和が、この実装の特徴です。

負閉路の影響がない最短経路は頂点を繰り返さない経路にでき、辺数は高々 V-1 です。
V 回目でも改善する頂点には、始点から到達できる負閉路の影響があります。
さらにその頂点から出辺をたどって影響を伝播します。閉路から到達できる頂点も、
閉路を何度も回ることで距離をいくらでも小さくできるためです。
例えば 0→1 が 1、1→2 が -2、2→1 が 1、2→3 が 5 なら、1,2,3 が負の無限大です。
始点から到達不能な別の負閉路は、この始点の結果に影響しません。

更新がないラウンドでは以降も変化しないため終了します。
全辺走査を高々 V 回行い、影響伝播は O(V+E) なので時間は O(VE+V+E) です。
距離、次の距離、直前の辺、影響フラグに O(V) の作業領域を使います。
負の無限大と到達不能はどちらも数値距離を持たないので、`state` で区別します。
非負辺だけなら Dijkstra、0/1 だけなら 0-1 BFS の方が速くなります。

## Preconditions / API

```cpp
cp::ShortestPathResult cp::bellman_ford(const cp::WeightedGraph& graph, int source);
```

`0 <= source < graph.size()`。辺重みは int64 全域を扱えます。
始点から到達不能な負閉路は結果へ影響しません。
始点から負閉路に到達でき、負閉路から v に到達できる場合だけ
`state[v] == negative_infinity`。その距離は下に有界でないため optional は空です。
負閉路に到達する前の頂点や別の枝の有限距離は通常通り返します。

## Complexity / implementation

同期式の緩和を最大 V 回行い、各ラウンドの候補は前ラウンドだけから作ります。
V 回目に改善した頂点から隣接リストを走査して負閉路の影響を伝播します。
時間 O(VE+V)、補助メモリ O(V)。
各ラウンドで更新された頂点だけ距離を反映し、配列全体のコピーを避けます。
途中で改善がなければ打ち切ります。W を最大絶対辺重みとすると、
探索する歩道は最大 V 辺なので広い整数での絶対値は VW 以下です。

## Examples

```cpp
#include <cp/graph/bellman_ford.hpp>
#include <cassert>
int main() {
    cp::WeightedGraph g(4);
    g.add_edge(0,1,2); g.add_edge(1,2,-3); g.add_edge(2,2,-1);
    auto result=cp::bellman_ford(g,0);
    assert(result.distance[1]==2);
    assert(result.state[2]==cp::DistanceState::negative_infinity);
    assert(result.state[3]==cp::DistanceState::unreachable);
    assert(result.has_negative_cycle() && !result.path_to(2));
}
```

## Verification

到達可能・到達不能な負閉路、下流の頂点、負辺のゼロ閉路、並行辺、
int64 下限、有限距離のオーバーフロー、1 頂点の負自己ループを単体確認します。
乱択では単純経路と単純閉路を全列挙し、最短距離・負閉路の影響と復元経路を比較します。
オンライン検証は AOJ GRL_1_B です。
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
