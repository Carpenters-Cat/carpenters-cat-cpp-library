---
title: Max Flow
source: include/cp/graph/max_flow.hpp
status: stable
tags:
  - graph
  - graph/flow
aliases:
  - 最大流
  - Dinic
  - ACL mf_graph
related: []
verification:
  unit:
    - verify/unit/graph/max_flow.test.cpp
  stress:
    - verify/stress/graph/max_flow.test.cpp
  online:
    - verify/online/graph/max_flow.test.cpp
---

# Max Flow

## Summary / When to use

有向グラフの最大 s-t フロー、最小カット、二部マッチングに使います。
ACL の Dinic 実装を基にした `cp::MaxFlow<Cap>` です。

## Preconditions / Pitfalls

`n >= 0`、頂点番号は `[0,n)`、容量と流量上限は非負です。
`Cap` は整数型を使い、最大流と容量の和が型に収まる必要があります。
`s != t`。自己ループ、並行辺、逆方向の辺、容量 0 に対応します。
最大流探索は再帰を用い、深さは O(V) です。非常に深いグラフでは
実行環境のスタック容量に注意してください。

## API / ACL correspondence

| API | 意味 |
| --- | --- |
| `MaxFlow<Cap>(n = 0)` | ACL `mf_graph<Cap>` に対応 |
| `add_edge(from,to,cap) -> int` | 辺追加、追加順の辺 ID |
| `get_edge(id) -> edge` | `from,to,cap,flow` のコピー |
| `edges() -> vector<edge>` | 全辺を追加順に返す |
| `change_edge(id,new_cap,new_flow)` | 容量と流量を指定値へ変更 |
| `flow(s,t[,limit]) -> Cap` | 残余グラフに追加で流せた量 |
| `min_cut(s) -> vector<bool>` | 現在の残余グラフで s から到達可能か |

メソッド名・辺構造体は ACL と同じです。`flow` は繰り返し呼べます。
`change_edge` の前提は `0 <= new_flow <= new_cap` です。
この操作は流量保存を自動修復しないため、呼び出し側で整合する状態を保ちます。
`min_cut` が最小カットを表すのは、上限による中断なしで最大流を求めた後です。

## Complexity

構築 O(V)、辺追加償却 O(1)、辺取得 O(1)、全辺取得 O(E)、
`change_edge` O(1)、最大流 O(V²E)、最小カット O(V+E)、メモリ O(V+E)。

## Examples

```cpp
#include <cp/graph/max_flow.hpp>
#include <cassert>
int main() {
    cp::MaxFlow<long long> g(3);
    g.add_edge(0,1,5); g.add_edge(1,2,3);
    assert(g.flow(0,2)==3);
    auto cut=g.min_cut(0);
    assert(cut[0] && cut[1] && !cut[2]);
}
```

## Verification

単体テストでは自己ループ、並行辺、流量上限、繰り返し探索、辺の変更、
64 bit 境界を確認します。乱択では全 s-t カットの列挙と比較し、
容量制約と各頂点の流量保存も確認します。
オンライン検証は AOJ GRL_6_A の公開テストをローカル実行します。

## Provenance

[ACL の利用元・ライセンス](../../acl-provenance.md)。
