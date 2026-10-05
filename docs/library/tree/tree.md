---
title: Tree and Rooted Tree
source: include/cp/tree/tree.hpp
status: stable
tags:
  - graph
  - graph/connectivity
aliases:
  - 無向木
  - 根付き木
  - 木の前処理
  - RootedTree
related:
  - tree/lca
  - tree/euler_tour
  - tree/heavy_light_decomposition
  - tree/rerooting
verification:
  unit:
    - verify/unit/tree/tree.test.cpp
---

# Tree and Rooted Tree

## Summary / When to use

木アルゴリズムで共通に使う、隣接順とグローバルな辺 ID を保つ無向木です。
重みや頂点情報は外部配列に持ち、辺 ID で対応させます。
DFS 前処理は独立した `RootedTree` として提供します。

## API

| API | 意味 |
| --- | --- |
| `cp::Tree(n = 0)` | n 頂点、辺なし。n は非負の int |
| `add_edge(u,v) -> size_t` | 無向辺を追加、0 始まりの追加順 ID を返す |
| `size() const -> int` | 頂点数 |
| `neighbors(v) const -> const vector<TreeArc>&` | `{to,edge_id}` を隣接順に参照 |
| `edges() const -> const vector<pair<int,int>>&` | 元の端点組を辺 ID 順に参照 |
| `RootedTree(tree, root = 0)` | 木を検証して DFS 前処理 |
| `rooted.root` | 根。空の場合は -1 |
| `rooted.parent[v]` | 親頂点。根は -1 |
| `rooted.parent_edge[v]` | 親への辺 ID。根は nullopt |
| `rooted.depth[v]` | 根からの辺数 |
| `rooted.order` | 隣接順を保った DFS の初回訪問順 |
| `rooted.subtree_size[v]` | v を含む部分木の頂点数 |

隣接辺の順序は `add_edge` の呼び出し順です。元の Tree は前処理で書き換えません。
Tree の参照の寿命は vector と同じで、辺追加による再確保で無効化され得ます。
`RootedTree` のフィールドは参照用として扱い、アルゴリズムへ渡すために変更しません。
自己ループ・並行辺・森は有効な木ではなく、RootedTree で拒否します。

## Complexity

Tree 構築 O(V)、辺追加償却 O(1)、参照取得 O(1)。RootedTree 構築 O(V+E)。
有効な木では E=V-1 なので時間・メモリ O(V)。

## Examples

```cpp
#include <cp/tree/tree.hpp>
#include <cassert>
int main() {
    cp::Tree tree(3);
    auto id=tree.add_edge(0,1); tree.add_edge(1,2);
    cp::RootedTree rooted(tree,2);
    assert(rooted.parent[0]==1 && rooted.parent_edge[0]==id);
    assert(rooted.depth[0]==2 && rooted.subtree_size[2]==3);
}
```

## Verification

単体テストで空、隣接順、親・深さ・部分木サイズ、辺 ID を確認します。
不正な辺数、並行辺、自己ループ、閉路、非連結、根番号は拒否を確認します。
共通前処理は各木アルゴリズムの深い木・乱択・オンライン検証でも使用します。
## Input contract

`cp::Tree` は 0 始まりの無向木です。`add_edge(u,v)` は両方向の隣接辺を追加します。
非空の場合、n-1 本の辺を持つ連結・閉路なしの木と有効な根が必要です。
前処理は辺数・閉路・連結性・根を検証し、不正なら `std::invalid_argument` を投げます。
頂点番号などの個々の参照・クエリは assert で検証し、NDEBUG 時は呼び出し側が守ります。
空の Tree は根引数を省略または 0 にすると構築でき、`size()==0`、`root()==-1`。
空の木への頂点クエリはできません。木を変更したら前処理を作り直します。
前処理後のデータは所有しており、元の Tree の寿命に依存しません。
全ての前処理は反復処理で、深い木でも再帰スタックを使いません。
