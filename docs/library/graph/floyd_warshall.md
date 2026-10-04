---
title: Warshall–Floyd
source: include/cp/graph/floyd_warshall.hpp
status: stable
tags:
  - graph
  - graph/shortest-path
aliases:
  - Floyd Warshall
  - Warshall Floyd
  - ワーシャルフロイド
  - 全点対最短路
  - APSP
related:
  - graph/weighted_graph
  - graph/bellman_ford
verification:
  unit:
    - verify/unit/graph/floyd_warshall.test.cpp
  stress:
    - verify/stress/graph/floyd_warshall.test.cpp
  online:
    - verify/online/graph/floyd_warshall.test.cpp
---

# Warshall–Floyd

## Summary / When to use

小規模・密な有向グラフの全点対最短路を求めます。
距離行列または辺列・隣接リストを持つ WeightedGraph から計算できます。

## API / Preconditions

| API | 意味 |
| --- | --- |
| `floyd_warshall(const DistanceMatrix&)` | 直接辺の重み行列から計算 |
| `floyd_warshall(const WeightedGraph&)` | 並行辺の最小重みを選んで計算 |
| `AllPairsShortestPathResult::distance[i][j]` | 有限距離だけ optional に格納 |
| `state[i][j]` | finite / unreachable / negative_infinity |
| `predecessor_vertex[i][j]` | i→j の最後の辺の始点 |
| `predecessor_edge[i][j]` | i→j の最後の辺 ID |
| `path(i,j) const` | 頂点列と辺 ID 列を返す、有限以外は nullopt |
| `has_negative_cycle() const` | グラフのどこかに負閉路が存在するか |

`DistanceMatrix` は `vector<vector<optional<Distance>>>`。
行列は正方形で、辺なしを `nullopt`、重み 0 の辺を値 0 で表現します。
対角は空経路の 0 と入力値の小さい方で初期化します。
負自己ループも保持します。空行列にも対応します。
行列サイズは int に収まり、各行の長さが一致する必要があります。

グラフ版の辺 ID は `add_edge` が返した元の ID です。
行列版の辺 ID は **row*n+column** です（0 始まり）。
経路の各辺は元の行列の直接辺を指します。対角の空経路は辺を含みません。

## Negative cycles / numeric bounds

i から負閉路へ到達でき、その負閉路から j へ到達できる場合だけ
`state[i][j] == negative_infinity`。到達不能とは区別します。
無関係な組の有限距離と経路は通常通り返します。

先にグラフ全体の到達可能性を求めます。距離の DP は対角が負になった
中継頂点を記録し、それを使った緩和を省きます。その頂点を経由できる組は
負閉路の影響を受けます。有限な組の最短路はその頂点を通らないため、
通常の Floyd と同じ距離を得られます。負閉路を繰り返すことで値が
指数的に小さくなる計算を避け、途中の候補は最大絶対辺重み W に対して
絶対値 2VW 以下の広い整数で扱います。**距離の切り詰めは行いません**。
最終的な有限距離が int64 の範囲外なら `overflow_error` です。

## Complexity

時間 O(V³)、補助メモリ・結果メモリ O(V²)、復元 O(L)。
`has_negative_cycle` は O(V)。全処理は反復処理です。

## Examples

```cpp
#include <cp/graph/floyd_warshall.hpp>
#include <cassert>
int main() {
    cp::DistanceMatrix d(3,std::vector<std::optional<cp::Distance>>(3));
    d[0][1]=2; d[1][2]=-5;
    auto result=cp::floyd_warshall(d);
    assert(result.distance[0][2]==-3);
    assert(result.state[2][0]==cp::DistanceState::unreachable);
    assert(result.path(0,2)->edges.size()==2);
}
```

## Verification

単体テストは空行列、非連結、負辺・負閉路、行列版とグラフ版の辺 ID、
int64 境界、有限距離のオーバーフロー、極小重みを持つ密な負閉路を確認します。
乱択では単純経路・単純閉路の全列挙、各始点の Bellman–Ford と比較します。
行列版とグラフ版の結果、復元経路の辺と費用も確認します。
オンライン検証は AOJ GRL_1_C です。

## Numeric type

`Distance` は int64 全域、有限距離の範囲は
[-9223372036854775808, 9223372036854775807]。
INF は予約せず、有限以外の distance は nullopt です。
加算には macOS/Linux の GCC/Clang が提供する `__int128_t` を使います。
結果フィールドは参照用とし、経路復元前に書き換えないでください。
