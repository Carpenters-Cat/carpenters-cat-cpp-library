---
title: Min Cost Flow
source: include/cp/graph/min_cost_flow.hpp
status: stable
tags:
  - graph
  - graph/flow
aliases:
  - 最小費用流
  - 最小費用最大流
  - ACL mcf_graph
  - slope
related: []
verification:
  unit:
    - verify/unit/graph/min_cost_flow.test.cpp
  stress:
    - verify/stress/graph/min_cost_flow.test.cpp
  online:
    - verify/online/graph/min_cost_flow.test.cpp
---

# Min Cost Flow

## Summary / When to use

非負コストの有向グラフで最小費用 s-t フローを求めます。
ACL のポテンシャル付き最短路実装に基づく `cp::MinCostFlow<Cap,Cost>` です。
`slope` は流量を増やしたときの最小費用の折れ線を返します。

## Preconditions / Pitfalls

`n >= 0`、頂点は `[0,n)`、`s != t`、容量・コスト・流量上限は非負です。
`Cap` は整数型、`Cost` は符号付き整数型を使います。総流量が `Cap`、
総費用とポテンシャル差・距離（最大辺コストを C として概ね nC）が
`Cost` に収まる必要があります。入力の負コストは非対応です。
自己ループ・並行辺・ゼロ容量・ゼロコストに対応します。

`flow` または `slope` はオブジェクトごとに **一度だけ** 呼べます。
ACL と同じ前提を assert でも確認します。各候補の流量上限を比較するときは
別のグラフを作るか、最初に `slope` を求めて補間します。
不正な入力は assert で検出し、NDEBUG 時は呼び出し側が前提を守ります。

## アルゴリズムの考え方

容量制約のもとで指定量のフローを送る費用を最小にします。逆辺には元の費用の負を付け、
過去の流れを取り消すと費用も戻します。元の辺の費用は非負ですが、残余辺には負辺が現れます。
そこで頂点ポテンシャル h により費用を `cost(u,v)+h[u]-h[v]` へ変換します。
経路上で h が相殺されるので、同じ始点・終点の経路の優劣は変わりません。

残余容量が正の辺の変換後費用を非負に保ち、Dijkstra で最も安い増加経路を探します。
距離でポテンシャルを更新し、経路の残余容量の最小値までまとめて流します。
最小費用の流れを最も安い増加経路で拡張することで、増加後の流量についても最適性を保てます。
例えば単位費用 2・容量 3 の経路と、単位費用 5・容量 2 の経路なら、
流量 0,3,5 に対する最小費用は 0,6,16 です。`slope` は費用関数の折れ点を返し、
同じ単位費用の隣接区間はまとめます。

成功した増加の回数を A とすると、終了時の探索も含め、各探索の初期化と緩和から
O((A+1)(V+E log(E+2))) が時間の目安です。整数容量では A は送った流量以下ですが、
容量が大きいだけで必ず探索回数も大きくなるわけではありません。メモリは O(V+E) です。
要求量を送れたかは戻り値の流量で確認します。`flow` / `slope` は合計 1 回だけ呼ぶ契約なので、
複数の要求量を検討するなら一度の slope を使ってください。

## API / ACL correspondence

| API | 意味 |
| --- | --- |
| `MinCostFlow<Cap,Cost>(n = 0)` | ACL `mcf_graph<Cap,Cost>` に対応 |
| `add_edge(from,to,cap,cost) -> int` | 辺追加、追加順の辺 ID |
| `get_edge(id) -> edge` | `from,to,cap,flow,cost` のコピー |
| `edges() -> vector<edge>` | 全辺を追加順に返す |
| `flow(s,t[,limit]) -> pair<Cap,Cost>` | 流せた量とその最小費用 |
| `slope(s,t[,limit]) -> vector<pair<Cap,Cost>>` | 費用関数の折れ点 |

メソッド名・辺構造体は ACL と同じです。省略した上限は `Cap` の最大値です。
到達不能の場合、`flow` は `{0,0}` を返します。
`slope` は `{0,0}` から始まり、同一の限界費用を持つ区間をまとめます。
隣接する折れ点の間は線形補間できます。要求量を流せなかった場合も
部分的な結果を返すので、戻り値の流量を確認してください。

## Complexity

構築 O(1)、辺追加償却 O(1)、辺取得 O(1)、全辺取得 O(E)。
成功した増加路による更新の回数を A とすると、`flow/slope` は
O((A+1)(V+E)log(V+E+1))。残余グラフの構築と、最後の到達不能な探索も含みます。
整数容量では各更新で 1 以上流すため A は得られる流量 F 以下です。
流量上限 0 や到達不能で F=0 の場合も初期化・探索の時間が必要です。メモリ O(V+E)。

## Examples

```cpp
#include <cp/graph/min_cost_flow.hpp>
#include <cassert>
int main() {
    cp::MinCostFlow<int,long long> g(2);
    g.add_edge(0,1,3,4);
    auto [sent,cost]=g.flow(0,1,2);
    assert(sent==2 && cost==8);
}
```

## Verification

単体テストでは上限、到達不能、ゼロコスト、折れ点の併合、64 bit 費用を確認します。
乱択では小グラフの全辺流量を全列挙して各流量の最小費用と比較し、
得られた辺流量の容量制約と流量保存を確認します。
オンライン検証は AOJ GRL_6_B の公開テストをローカル実行します。

## Provenance

[ACL の利用元・ライセンス](../../acl-provenance.md)。
