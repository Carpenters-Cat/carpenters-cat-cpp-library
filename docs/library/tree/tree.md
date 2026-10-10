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

## アルゴリズムを支える表現

木は連結で閉路のない無向グラフです。Tree は各辺を一つの ID で登録し、両端の
隣接リストへ追加します。根を決めると、各頂点の親・深さ・部分木が一意に決まります。
例えば辺 0–1、1–2、1–3 を根 0 で見ると、1 の子は 2,3、部分木サイズは 3 です。

RootedTree は反復 DFS で根付き表現を作ります。各頂点の隣接辺の走査位置を持つ
スタックを使い、初訪問で親・深さと preorder、探索終了時に親への部分木サイズ加算を行います。
部分木のサイズは「自身の 1 + 全ての子のサイズ」なので、子の終了後に計算すれば正しくなります。
preorder では一つの部分木を訪問し切ってから他へ進むため、各部分木が連続します。

入力は辺数 V-1、根の範囲、既訪問の別辺による閉路、全頂点への到達を検証します。
親の辺 ID だけを除外するので、自己ループや並行辺の閉路も検出できます。
Tree への辺追加だけでは木としての妥当性は確定せず、RootedTree やそれを使う各構造の
構築時に invalid_argument で拒否します。空木では root は -1 になります。

各頂点・辺を定数回処理し、構築とメモリは O(V) です。再帰スタックを使いません。
根付き木の情報は構築時のスナップショットなので、その後の辺追加を自動反映しません。
部分木操作は Euler Tour、祖先検索は LCA、経路の区間分解は HLD へつなげられます。

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
