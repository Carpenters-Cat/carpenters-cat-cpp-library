---
title: Topological Sort
source: include/cp/graph/topological_sort.hpp
status: stable
tags:
  - graph
aliases:
  - トポロジカルソート
  - 有向閉路検出
verification:
  unit:
    - verify/unit/graph/topological_sort.test.cpp
  stress:
    - verify/stress/graph/topological_sort.test.cpp
  online:
    - verify/online/graph/topological_sort.test.cpp
    - verify/online/graph/directed_cycle.test.cpp
---

# Topological Sort

## Summary / When to use

有向グラフのトポロジカル順序を DFS の帰りがけ順の逆順で求め、閉路を検出します。
再帰を使わず、長い DAG にも対応します。

## Preconditions / API

```cpp
std::optional<std::vector<int>> cp::topological_sort(
    const std::vector<std::vector<int>>& graph);
```

`graph[v]` は v の行き先リスト。頂点数は int の範囲、行き先は 0 以上 V 未満。
並行辺、自己ループ、非連結を許します。有向閉路がある場合は `nullopt`、
DAG なら全頂点をちょうど 1 回含む順序を返します。
空グラフは値のある空 vector を返します。

DFS の根は ID 昇順、隣接リストは入力順に走査し、帰りがけ順を反転します。
処理中の頂点に向かう辺で閉路を判定します。入力が同じなら結果は同じです。
辞書順最小の順序は保証しません。明示的なスタックと `size_t` の走査位置を使います。

## Complexity

O(V+E) 時間、O(V) 補助メモリ。入力の隣接リストは変更しません。

## Examples

```cpp
#include <cp/graph/topological_sort.hpp>
#include <cassert>
int main() {
    auto order=cp::topological_sort({{1},{},{}});
    assert((order==std::vector<int>{2,0,1}));
    assert(!cp::topological_sort({{1},{0}}));
}
```

## Verification

空、非連結、並行辺、自己ループ、閉路、20 万頂点の経路を単体確認します。
固定 seed の小グラフで全頂点順列を列挙し、DAG 判定と返された順序の全辺制約を比較します。
[AOJ Topological Sort](https://onlinejudge.u-aizu.ac.jp/problems/GRL_4_B) の順序と
[AOJ Directed Cycle](https://onlinejudge.u-aizu.ac.jp/problems/GRL_4_A) の閉路判定を検証します。
