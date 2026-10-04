---
title: Integer geometry
source: include/cp/geometry/integer_geometry.hpp
status: stable
tags:
  - geometry
aliases:
  - IntPoint
  - IntVector
  - convex hull
  - orientation
  - 整数幾何
  - 内積
  - 外積
  - 凸包
requires:
  - 幾何判定と凸包の点座標は各成分の絶対値 <= 2^62-1
  - ベクトルの dot と cross の各成分の絶対値 <= INT64_MAX
  - GCC/Clang の signed __int128_t を使う macOS/Linux 環境
complexity:
  predicates: 内積・外積・向き・線分上・交差判定は O(1)
  convex_hull: O(N log N) 時間、O(N) メモリ
pitfalls:
  - 線分の交差は端点での接触・共線な重なり・零長線分を含む
  - 凸包の keep_collinear は境界上の点を残す規則を切り替える
  - 128 bit の結果を 64 bit に変換するときは結果の範囲を確認する
related: []
verification:
  unit:
    - verify/unit/geometry/integer_geometry.test.cpp
  stress:
    - verify/stress/geometry/integer_geometry.test.cpp
  online:
    - verify/online/geometry/static_convex_hull.test.cpp
    - verify/online/geometry/orientation.test.cpp
    - verify/online/geometry/segment_intersection.test.cpp
    - verify/online/geometry/boundary_convex_hull.test.cpp
---

# Integer geometry

整数の点・ベクトル、内積・外積、向き、閉線分の交差、凸包を提供します。
浮動小数点や epsilon を使わず、符号付き 128 bit の整数で正確に判定します。

## Coordinates and products

`cp::IntPoint` は公開フィールド `int64_t x,y` を持ち、`cp::IntVector` は同じ型の別名です。
既定値は (0,0)、`IntPoint{x,y}` で構築します。点の座標とベクトルの成分には、それぞれ
次の契約があります。

幾何判定と凸包では各座標の絶対値を B=2^62-1 以下とします。
`cp::integer_geometry_coordinate_bound` に B を公開しています。
差の絶対値は最大 2B=2^63-2、外積の絶対値の粗い上界は 2(2B)^2 < 2^127-1 です。
三点の外積は **減算する前** に各成分を `cp::GeometryWide` (= `__int128_t`) へ拡張し、
差・積・差の積の減算まで同じ型で行います。

二つのベクトルの内積・外積では各成分の絶対値 <= INT64_MAX とします。
その積和の絶対値は最大 2*INT64_MAX² = 2^127-2^65+2 で、符号付き 128 bit に収まります。
INT64_MIN の成分は許容しません。このベクトルの範囲は、上記の点同士の差も含みます。

点・ベクトルの加減算、符号反転、整数スカラー倍は計算を 128 bit で行い、結果の各成分が
int64 に収まることを assert して戻します。結果を幾何上の点として使うときは改めて B
以下の契約を守ってください。幾何判定・ベクトル積も範囲を assert します。
NDEBUG 時は呼び出し側で全ての前提を守ります。GCC/Clang の整数拡張を使い、
本リポジトリの macOS/Linux ターゲットに対応します。

## API

以下で P は IntPoint、V は IntVector、W は GeometryWide です。

| API | 動作 |
| --- | --- |
| `P{x,y}`, `P{}` | int64 座標を持つ点・ベクトル、既定値は (0,0) |
| `a==b`, `a<=>b` | x、次に y の辞書順。sort に利用可能 |
| `a+b`, `a-b`, `-a`, `a*int64_t`, `int64_t*a` | 点・ベクトルの成分ごとの演算 |
| `a+=b`, `a-=b`, `a*=int64_t` | 複合代入 |
| `W cp::dot(V a,V b)` | ax*bx + ay*by |
| `W cp::cross(V a,V b)` | ax*by - ay*bx |
| `W cp::cross(P a,P b,P c)` | 三角形 abc の符号付き面積の 2 倍 |
| `int cp::orientation(P a,P b,P c)` | 反時計回り 1、時計回り -1、共線 0 |
| `bool cp::on_segment(P a,P b,P p)` | p が閉線分 ab 上にあるか |
| `bool cp::segments_intersect(P a,P b,P c,P d)` | 閉線分 ab と cd の共通点があるか |
| `vector<P> cp::convex_hull(vector<P> points,bool keep_collinear=false)` | 下記の規則で凸包を返す |

`on_segment(a,a,p)` は p=a のときだけ true です。交差は端点だけの接触、共線での重なりを
含みます。どちらか、または両方が零長でも判定できます。交点の座標を返す API は
含めていません。交点は整数とは限らないためです。

## Hull ordering and collinear points

凸包は Andrew の monotone chain です。まず座標を辞書順に sort し、重複点を除去します。
空列は空、同じ点だけなら一点、異なる二点なら辞書順の二点です。

非共線の凸包は反時計回りで、辞書順最小の点から始めます。終点に始点を重複させません。
`keep_collinear=false` (既定) は辺の内部にある点を除いて頂点だけを返します。
`keep_collinear=true` は境界上にある入力点を全て一回ずつ、辺に沿った順で残します。
凸包内部の点はどちらの場合も除きます。

全点が共線の場合、既定では辞書順の両端二点、keep_collinear=true では重複除去後の全点を
辞書順で返します。この退化ケースには反時計回りの向きはありません。

```cpp
#include <cp/geometry/integer_geometry.hpp>
#include <cassert>
int main() {
    using P = cp::IntPoint;
    assert(cp::orientation({0,0},{2,0},{1,1}) == 1);
    assert(cp::segments_intersect({0,0},{2,2},{0,2},{2,0}));
    auto hull = cp::convex_hull({{0,0},{2,0},{1,0},{2,2},{0,2},{1,1}});
    assert(hull == std::vector<P>({{0,0},{2,0},{2,2},{0,2}}));
}
```

## Verification

単体テストは重複、空・一・二点、全共線、境界上の点、接触と重なり、零長線分、負数を
扱い、B の両端と INT64_MAX のベクトルで 128 bit の積を確認します。
固定 seed の比較テストは、小さい座標で六項の shoelace 式、内積による線分上の判定、
有理パラメーターによる交差判定、Jarvis march と比較します。
凸包の頂点集合は他の二点・三点の凸結合の全列挙でも検証し、入力の包含は全ての辺と
三角形の列挙で確認します。境界保持の順序も別途比較します。

[Library Checker Static Convex Hull](https://judge.yosupo.jp/problem/static_convex_hull) は
重複・空入力・共線を含み、辺内部の点を除く仕様なので既定の凸包を検証します。
[AOJ CGL_4_A](https://onlinejudge.u-aizu.ac.jp/problems/CGL_4_A) は境界点を残し、最小 y、
同順位なら最小 x から始める仕様なので、保持モードの結果をドライバー側で rotate します。
[AOJ CGL_1_C](https://onlinejudge.u-aizu.ac.jp/problems/CGL_1_C) と
[AOJ CGL_2_B](https://onlinejudge.u-aizu.ac.jp/problems/CGL_2_B) で向き・線分上・内積と交差を
検証します。公開テストケースをダウンロードしてローカル実行し、解答は提出しません。
