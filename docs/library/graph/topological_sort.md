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

## アルゴリズムの考え方

全ての有向辺 u→v で u が v より先に現れる順序を求めます。
タスクの依存関係や DAG 上の DP で、先行状態が完成してから後続状態を処理できます。
例えば 0→2、1→2 なら `[0,1,2]` も `[1,0,2]` も正しく、辞書順最小の契約はありません。

DFS で頂点を未訪問・探索中・探索済みの 3 状態に分け、全出辺の処理後に終了順へ追加します。
探索中の頂点への辺は DFS の祖先へ戻る有向閉路なので、`nullopt` を返します。
探索済みへの辺は閉路の証拠ではありません。自己ループは探索中の自身への辺です。
閉路がなければ、辺 u→v で v は必ず u より先に探索を終了します。
未訪問なら v の探索を完了してから u を終了し、探索済みなら既に終了しているためです。
終了順を反転すれば全ての辺の順序制約を満たせます。

非連結でも全ての未訪問頂点を起点に探索します。スタックと走査位置を明示的に持ち、
時間は O(V+E)、結果を含む作業領域は O(V) です。
空グラフは成功した空の順序を返すので、失敗した `nullopt` と区別してください。

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
