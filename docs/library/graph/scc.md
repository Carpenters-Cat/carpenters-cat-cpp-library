---
title: Strongly Connected Components
source: include/cp/graph/scc.hpp
status: stable
tags:
  - graph
  - graph/connectivity
aliases:
  - SCC
  - 強連結成分
  - Kosaraju
  - ACL scc_graph
related: []
verification:
  unit:
    - verify/unit/graph/scc.test.cpp
  stress:
    - verify/stress/graph/scc.test.cpp
  online:
    - verify/online/graph/scc.test.cpp
---

# Strongly Connected Components

## Summary / When to use

有向グラフを強連結成分に分解します。到達可能性の整理、縮約 DAG、2-SAT に使います。
反復版 Kosaraju 法により、鎖状グラフでも再帰スタックを使いません。

## Preconditions / Pitfalls

`n >= 0`、頂点番号は `[0,n)`。自己ループ、並行辺、空グラフに対応します。
不正な頂点は assert で検出し、NDEBUG 時は呼び出し側が前提を守ります。
互いに到達可能でない成分間の並び順は一意とは限りません。

## API / ACL correspondence

| API | 意味 |
| --- | --- |
| `cp::SccGraph(n = 0)` | ACL `scc_graph` に対応 |
| `add_edge(from,to)` | 有向辺を追加 |
| `scc() const -> vector<vector<int>>` | 成分をトポロジカル順に返す |
| `scc_ids() const -> pair<int,vector<int>>` | 成分数と各頂点の成分 ID |

`scc_ids` は ACL の内部機能を公開した追加 API です。
成分をまたぐ辺 u→v では `id[u] < id[v]`。
`scc()` の各成分内の頂点は昇順です。呼び出しはグラフを変更せず、
再計算も辺追加後の再計算もできます。ACL と同じ結果の条件を満たしますが、
独立した成分の順序は ACL の出力と異なることがあります。

## Complexity

構築 O(V)、辺追加償却 O(1)、分解 O(V+E)、メモリ O(V+E)。
正順・逆順の隣接リストを保持します。

## Examples

```cpp
#include <cp/graph/scc.hpp>
#include <cassert>
int main() {
    cp::SccGraph g(3);
    g.add_edge(0,1); g.add_edge(1,0); g.add_edge(1,2);
    auto [count,id]=g.scc_ids();
    assert(count==2 && id[0]==id[1] && id[1]<id[2]);
}
```

## Verification

単体テストでは空・孤立点・自己ループ・辺追加後の再計算・30 万頂点の鎖と閉路を確認します。
乱択では推移閉包による相互到達性と成分分割を比較し、成分 ID の順序も確認します。
オンライン検証は Library Checker の Strongly Connected Components です。
