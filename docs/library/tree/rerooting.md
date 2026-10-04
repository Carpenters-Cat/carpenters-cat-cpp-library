---
title: Rerooting DP
source: include/cp/tree/rerooting.hpp
status: stable
tags:
  - graph
  - graph/connectivity
  - dp
aliases:
  - 全方位木 DP
  - 全方位 DP
  - Rerooting
  - 非可換全方位 DP
related:
  - tree/tree
verification:
  unit:
    - verify/unit/tree/rerooting.test.cpp
  stress:
    - verify/stress/tree/rerooting.test.cpp
  online:
    - verify/online/tree/rerooting.test.cpp
---

# Rerooting DP

## Summary / When to use

全ての頂点を根にした木 DP を、底からと上からの 2 つの走査で求めます。
State、単位元、結合、辺変換、頂点変換を呼び出し側で指定します。
隣接順を保つ prefix / suffix 積により、結合は可換でなくても構いません。

## API / algebraic contract

```cpp
auto cp::rerooting(const cp::Tree& tree, State identity,
                  Combine combine, Lift lift, Finish finish) -> std::vector<Result>;
```

| 指定するもの | 契約 |
| --- | --- |
| `identity` | State の両側単位元 |
| `combine(const State&,const State&) -> State` | 結合的な結合。可換性は不要 |
| `finish(const State&,int vertex) -> Result` | 隣接辺の集約へ頂点の情報を加える |
| `lift(const Result&,int from,int to,size_t edge_id) -> State` | from で完結した DP を辺経由で to に送る |

返り値は `result[v]` が v を根にした結果です。State と Result は異なる型でも構いません。
State はコピー構築・代入可能で、Result は移動構築可能な値型です。
Result のデフォルト構築やコピーは不要です。
呼び出す関数は同じ入力で同じ結果を返し、引数や Tree を変更しないでください。
内部では同じ頂点に複数回 finish を呼ぶので、呼び出し回数に結果を依存させません。
数値演算・オーバーフロー・出力サイズは指定した型と関数の責任です。

## Neighbor order / exact semantics

各頂点 v の隣接列は **元の `Tree::neighbors(v)` の順序** です。
親からの寄与も、その隣接辺が元々あった位置に入ります。
ある辺を除いた DP でも、残る隣接辺の順序は変えません。

`message(u→v)` は、u の隣接列から v だけを除いた寄与を順に combine し、
`finish(aggregate,u)`、`lift(result,u,v,edge_id)` を順に適用した State です。
最終結果 `result[v]` は全隣接点からの message を順に combine し、finish したものです。
初期の処理用の根は 0 ですが、これによって上記の隣接順は変わりません。
空の Tree は空の結果、単一頂点は `finish(identity,0)` を返します。
無効な木は共通 RootedTree の検証により invalid_argument です。

## Complexity

結合・変換・State のコピーが O(1) なら時間・補助メモリ・出力メモリ O(V)。
合計 O(V) 回の結合と変換を行います。文字列などサイズが増える State / Result では、
各コピーと操作、全根の出力サイズを含む計算量になります。
深い木も反復処理で扱い、再帰スタックは使いません。

## Examples

各根からの距離の総和を求めます。辺変換で、その枝の頂点数だけ距離が増えます。

```cpp
#include <cp/tree/rerooting.hpp>
#include <cassert>
struct State { long long count,sum; };
int main() {
    cp::Tree tree(3); tree.add_edge(0,1); tree.add_edge(1,2);
    auto result=cp::rerooting(tree,State{0,0},
        [](State a,State b){return State{a.count+b.count,a.sum+b.sum};},
        [](State a,int,int,std::size_t){return State{a.count,a.sum+a.count};},
        [](State a,int){return State{a.count+1,a.sum};});
    assert(result[0].sum==3 && result[1].sum==2 && result[2].sum==3);
}
```

## Verification

単一頂点、パス、スター、20 万頂点の鎖の全根の距離和を単体確認します。
State と Result が異なる場合、移動専用 Result も確認します。
乱択では各根の愚直 DP と比較します。文字列連結・頂点ラベル・方向付き辺ラベルを
用いた非可換 DP によって、親寄与の位置と除外後の隣接順も検証します。
Library Checker の Tree Path Composite Sum では modint と辺の一次変換を検証します。
