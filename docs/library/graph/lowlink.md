---
title: Lowlink / Bridges / Articulation Points
source: include/cp/graph/lowlink.hpp
status: stable
tags:
  - graph
  - graph/connectivity
aliases:
  - Lowlink
  - 橋
  - 関節点
verification:
  unit:
    - verify/unit/graph/lowlink.test.cpp
  stress:
    - verify/stress/graph/lowlink.test.cpp
  online:
    - verify/online/graph/articulation_points.test.cpp
    - verify/online/graph/bridges.test.cpp
---

# Lowlink / Bridges / Articulation Points

## Summary / When to use

無向グラフの DFS 訪問順序・lowlink・橋・関節点を求めます。
明示的なスタックを使い、深い木でも実行スタックを消費しません。

## アルゴリズムの考え方

無向グラフを DFS し、訪問順 `order[v]` と、v の部分木から木の辺を下り、
高々 1 本の非木辺で戻れる最小の訪問順 `low[v]` を計算します。
初期値は自身の訪問順で、既訪問の隣接点の順序と、探索を終えた子の low で小さくします。
実装はスタックと隣接辺の走査位置を使い、再帰から戻る処理も再現します。

親 p と子 v の辺が橋である条件は `low[v] > order[p]` です。
子の部分木から親または祖先へ戻る別の辺がなく、その辺の削除で分断されるためです。
非根 p が関節点になる条件は、いずれかの子で `low[v] >= order[p]` となることです。
等号でも親自身の削除後には祖先側へ出られません。DFS の根は例外で、子が 2 個以上なら
関節点です。例えば三角形 0–1–2–0 に辺 2–3 を付けると、その辺だけが橋、2 が関節点です。

親頂点でなく親の辺 ID だけを除外し、並行辺のもう一方を戻る辺として扱います。
これにより並行辺を誤って橋と判定しません。非連結なら各未訪問頂点から探索を始めます。
頂点・辺を定数回走査するため時間とグラフを含むメモリは O(V+E) です。
有向グラフの強連結成分には別の SCC を使います。

## Preconditions / API

```cpp
cp::UndirectedGraph graph(int n=0);
std::size_t graph.add_edge(int from,int to);
int graph.size() const;
const std::vector<cp::UndirectedEdge>& graph.edges() const;
const cp::UndirectedEdge& graph.get_edge(std::size_t id) const;
const std::vector<std::size_t>& graph.incident(int vertex) const;
cp::LowlinkResult cp::lowlink(const cp::UndirectedGraph& graph);
```

n は非負の int、頂点は 0 以上 n 未満、辺 ID は登録済みの範囲。
`UndirectedEdge` は `int from,to` を持ちます。`add_edge` は登録順の大域辺 ID を返し、
両端の incident リストに同じ ID を追加します。自己ループはその頂点に 2 回登録します。
多重辺・自己ループ・非連結・孤立頂点・空グラフに対応します。

結果の `order[v]` は DFS 発見時刻 (0..V-1)、`low[v]` は v の部分木から
木辺で下り、最大 1 本の非木辺を使って到達できる最小発見時刻です。
親の **辺 ID だけ**を除外し、親への別の並行辺は非木辺として使います。
根は頂点 ID 昇順、incident リストは登録順で走査するため結果は決定的です。
`bridges` は橋の元の ID の昇順、`articulation_points` は頂点 ID 昇順、
`components` は元グラフの連結成分数。空なら全 vector は空で成分数 0。
橋・関節点はその辺/頂点を削除したとき全体の成分数が増えるものです。
根の関節点判定は DFS の子が 2 個以上かどうかで決めます。
グラフを変更した後は結果を再計算してください。

## Complexity

O(V+E) 時間、O(V+E) 補助メモリ。再帰なし。
グラフの格納も O(V+E)、1 辺の追加は償却 O(1)。

## Examples

```cpp
#include <cp/graph/lowlink.hpp>
#include <cassert>
int main() {
    cp::UndirectedGraph g(3);
    g.add_edge(0,1); g.add_edge(0,1);
    auto id=g.add_edge(1,2);
    auto result=cp::lowlink(g);
    assert((result.bridges==std::vector<std::size_t>{id}));
    assert((result.articulation_points==std::vector<int>{1}));
}
```

## Verification

単体で訪問順序/low の既知値、根の子、並行辺、自己ループ、孤立点、非連結を確認し、
20 万頂点の経路で橋・関節点と再帰なしの処理を確認します。
固定 seed の小グラフでは各辺/各頂点を削除し、独立した BFS 成分数と比較します。
DFS の木を別に構築し、各部分木の全頂点・非木辺の列挙から low 値も比較します。
[AOJ Articulation Points](https://onlinejudge.u-aizu.ac.jp/problems/GRL_3_A) と
[AOJ Bridges](https://onlinejudge.u-aizu.ac.jp/problems/GRL_3_B) を検証します。
