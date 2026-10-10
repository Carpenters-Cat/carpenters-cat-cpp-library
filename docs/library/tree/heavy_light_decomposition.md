---
title: Heavy-Light Decomposition
source: include/cp/tree/heavy_light_decomposition.hpp
status: stable
tags:
  - graph
  - graph/connectivity
  - data-structure/range-query
aliases:
  - HLD
  - Heavy Light Decomposition
  - 重軽分解
  - 木のパスクエリ
  - 非可換パス
related:
  - tree/tree
  - data_structure/segment_tree
verification:
  unit:
    - verify/unit/tree/heavy_light_decomposition.test.cpp
  stress:
    - verify/stress/tree/heavy_light_decomposition.test.cpp
  online:
    - verify/online/tree/heavy_light_decomposition.test.cpp
---

# Heavy-Light Decomposition

## Summary / When to use

木のパスを O(log V) 個の配列区間に分解し、Segment Tree 等へ接続します。
パスの u→v 順を保ち、非可換な演算でも使える方向情報を返します。
heavy child は最大部分木の子とし、同じ大きさなら元の隣接順で先の子を選びます。

## アルゴリズムの考え方

各頂点の子のうち部分木が最大の子への辺を heavy、それ以外を light とします。
heavy を先にたどる DFS 順に番号を振ると、heavy の鎖が配列上で連続し、各部分木も
連続区間になります。木の任意の道を、これらの鎖に沿う少数の区間へ分ける構造です。

light 辺で子へ進むと子の部分木サイズは親の半分未満です。
最大の子が別にあるため、light の子のサイズが親の半分以上にはなれません。
したがって根への道に light 辺は O(log V) 本しかなく、u→v の道も O(log V) 個の
heavy の区間で表せます。`path` は鎖の頭が深い側から頭の親へ移動して分解します。

例えば道 2→1→3 は LCA の 1 を一度だけ含みます。頂点値なら 1 の位置を含め、
辺値なら各辺を子の位置に置き、LCA の位置を除外します。`edge_values=true` はこの規則です。
返される区間は常に `[left,right)` で、u から v へ進む向きが番号の逆なら `reverse=true` です。
非可換な積では区間ごとに順方向・逆方向の積を用意し、返された順に結合します。

構築は O(V)、path・LCA は O(log(V+1))、メモリは O(V) です。
各区間を Segment Tree の O(log V) 検索で処理すると、経路クエリ全体は O(log² V) です。
この型自体は値の集約や更新を行いません。番号と区間を取得して、利用側のデータ構造へ渡します。
無向の和なら reverse を無視できますが、文字列や関数合成では無視しないでください。

## API

| API | 意味 |
| --- | --- |
| `cp::HeavyLightDecomposition(tree, root = 0)` | heavy child を先に訪問する分解 |
| `size()` / `root()` | 頂点数・根 |
| `parent(v)` / `depth(v)` | 親と辺数による深さ |
| `index(v)` / `vertex(index)` | 頂点と配列添字の相互変換 |
| `head(v)` | v の heavy path の先頭頂点 |
| `edge_index(edge_id)` | 辺値を格納する子頂点の添字 |
| `path(u,v,edge_values = false)` | u→v の順に `PathSegment{left,right,reverse}` を返す |
| `subtree(v,edge_values = false)` | 部分木の半開区間 |
| `lca(u,v)` / `distance(u,v)` | 最近共通祖先 / 辺数（int64） |

全区間は `[left,right)`。`reverse==false` なら left..right-1、
true なら right-1..left の順に集約します。返された区間自体の並び順も保ちます。
頂点値モードは両端点と LCA を含みます。

## Edge values / direction

辺値は **その辺の根から見た子の index** に格納します。
根の添字には単位元などのダミーを置きます。
`path(u,v,true)` は **LCA の添字をちょうど 1 つ除外** し、パス上の辺だけを返します。
u==v の辺パスは空です。`subtree(v,true)` は v の添字を除外し、
v の部分木内部の辺だけを含みます（親との接続辺は含みません）。

通常の Segment Tree の順方向積だけでは非可換演算の逆順積は得られません。
各ノードに順方向・逆方向の 2 種類の集約を持ち、reverse フラグで使い分けます。
辺の変換自体が方向で違う場合も、親→子と子→親の値を別に保持してください。
部分木区間の順序は heavy child を優先した HLD 順であり、元の隣接順とは限りません。

## Complexity

構築 O(V)、添字・部分木取得 O(1)、パス分解・LCA・距離 O(log(V+1))、メモリ O(V)。
各区間を O(log V) の Segment Tree へ問い合わせる場合は O(log² V)。

## Examples

```cpp
#include <cp/tree/heavy_light_decomposition.hpp>
#include <cassert>
int main() {
    cp::Tree tree(4);
    tree.add_edge(0,1); tree.add_edge(0,2); tree.add_edge(1,3);
    cp::HeavyLightDecomposition h(tree);
    std::vector<int> path;
    for(auto segment:h.path(3,2)) {
        if(segment.reverse)
            for(int i=segment.right;i-->segment.left;) path.push_back(h.vertex(i));
        else
            for(int i=segment.left;i<segment.right;++i) path.push_back(h.vertex(i));
    }
    assert((path==std::vector<int>{3,1,0,2}));
    assert(h.path(3,3,true).empty());
}
```

## Verification

単一頂点、パス両方向、辺の LCA 除外、20 万頂点の深いパスを確認します。
乱択では愚直なパス・部分木の頂点と辺を列挙して比較し、文字列連結という
非可換演算を既存 Segment Tree に接続して点更新後も両方向の結果を比較します。
Library Checker の Vertex Set Path Composite では一次関数の合成と点更新を検証します。
## Input contract

`cp::Tree` は 0 始まりの無向木です。`add_edge(u,v)` は両方向の隣接辺を追加します。
非空の場合、n-1 本の辺を持つ連結・閉路なしの木と有効な根が必要です。
前処理は辺数・閉路・連結性・根を検証し、不正なら `std::invalid_argument` を投げます。
頂点番号などの個々の参照・クエリは assert で検証し、NDEBUG 時は呼び出し側が守ります。
空の Tree は根引数を省略または 0 にすると構築でき、`size()==0`、`root()==-1`。
空の木への頂点クエリはできません。木を変更したら前処理を作り直します。
前処理後のデータは所有しており、元の Tree の寿命に依存しません。
全ての前処理は反復処理で、深い木でも再帰スタックを使いません。
