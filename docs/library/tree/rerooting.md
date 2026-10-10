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

## アルゴリズムの考え方

各頂点を根にして独立に木 DP をすると O(V²) になります。
全方位 DP は辺 u–v を切った u 側の結果を v へ送る `message(u→v)` を、全ての向きで
求めて共有します。最終的には各頂点が全隣接点の message を受け取って答えを作ります。
`combine` は隣接枝の集約、`finish` は頂点情報の追加、`lift` は辺を越える変換です。

最初の走査は仮の根 0 に対し、子から親への message を下から計算します。
次の走査は親から子へ向かい、その子以外の全隣接点の寄与から message を作ります。
子へ送る時点で他の子と親の寄与がそろっているので、各有向辺の値を正しい順で求められます。

一つの隣接点を除外するために全隣接点を毎回足すと、スターの中心で O(V²) になります。
そこで前方累積と後方累積を作り、i 番目を除いた積を `combine(prefix[i],suffix[i+1])`
で求めます。元の隣接順を保持するので、可換性は不要です。逆演算も使いません。

例えば各根からの距離和なら State を `(頂点数,距離和)` とし、combine は成分ごとの加算、
finish は自分の頂点を 1 個追加、lift は距離和へ頂点数を加えます。
道 0–1–2 の結果は根 0,1,2 でそれぞれ 3,2,3 です。
隣接点の総数は 2(V-1) なので、演算・コピーが O(1) なら時間・領域とも O(V) です。
長い文字列などを State にすると、その演算・コピー・全根の出力サイズを含めて評価します。
根を変えても同じ局所的な message の規則で表せる DP であることが利用の前提です。

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
