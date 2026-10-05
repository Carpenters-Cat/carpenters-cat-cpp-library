---
title: Weighted Graph and Shortest Path Results
source: include/cp/graph/weighted_graph.hpp
status: stable
tags:
  - graph
  - graph/shortest-path
aliases:
  - 重み付きグラフ
  - 隣接リスト
  - DistanceState
  - ShortestPathResult
  - 経路復元
related:
  - graph/dijkstra
  - graph/zero_one_bfs
  - graph/bellman_ford
  - graph/floyd_warshall
verification:
  unit:
    - verify/unit/graph/weighted_graph.test.cpp
---

# Weighted Graph and Shortest Path Results

## Summary / When to use

最短路ライブラリで共有する有向重み付きグラフ、距離状態、経路の型です。
隣接リストの各要素は、追加順に付くグローバルな辺 ID です。
無向辺は逆向きも含めて 2 回追加します。

## Preconditions / API

頂点数は `0 <= n <= INT_MAX`、各頂点は `[0,n)`。
自己ループ・並行辺・負重みを保持できます。アルゴリズムごとの重み制限は別途守ります。
不正な頂点・辺 ID は assert で検出し、NDEBUG 時は呼び出し側が前提を守ります。

| API | 意味 |
| --- | --- |
| `WeightedGraph(n = 0)` | n 頂点、辺なし |
| `size() const -> int` | 頂点数 |
| `add_edge(from,to,weight) -> size_t` | 有向辺を追加して辺 ID を返す |
| `edges() const -> const vector<WeightedEdge>&` | 追加順の辺列 |
| `get_edge(id) const -> const WeightedEdge&` | `from,to,weight` を参照 |
| `outgoing(v) const -> const vector<size_t>&` | v を始点とする辺 ID の隣接リスト |
| `ShortestPath` | `vector<int> vertices` と `vector<size_t> edges` |
| `ShortestPathResult` | 始点、距離、距離状態、前駆頂点・辺 |
| `result.has_negative_cycle() const` | 始点から到達可能な負閉路の影響があるか |
| `result.path_to(target) const` | 有限な経路を反復処理で復元 |

参照の寿命は通常の vector と同じです。`add_edge` の再確保は参照を無効化し得ます。
辺 ID は追加後も変わりません。結果は計算時点のグラフに対するものです。
グラフを編集した場合は最短路を再計算します。

## Complexity

グラフ構築 O(V)、辺追加償却 O(1)、各参照取得 O(1)、グラフメモリ O(V+E)。
単一始点の結果は O(V) メモリ、経路復元は返す経路長 L に対して O(L)。
`has_negative_cycle` は O(V) です。全処理は再帰スタックを使いません。

## Examples

```cpp
#include <cp/graph/weighted_graph.hpp>
#include <cassert>
int main() {
    cp::WeightedGraph g(3);
    auto id=g.add_edge(0,1,-2);
    assert(g.get_edge(id).weight==-2);
    assert(g.outgoing(0).front()==id);
}
```

## Verification

単体テストでは空グラフ、辺 ID、並行辺、自己ループ、距離状態と経路を確認します。
アルゴリズムの単体・乱択・オンライン検証は各最短路エントリに登録されています。
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
