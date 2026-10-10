---
title: Li Chao Tree
source: include/cp/data_structure/li_chao_tree.hpp
status: stable
tags:
  - data-structure
  - data-structure/range-query
  - optimization
aliases:
  - Li Chao Tree
  - Li Chao Segment Tree
  - 李超線段樹
  - 直線追加最小値
  - 線分追加最小値
requires:
  - X は bool 以外の標準整数型。問い合わせ座標は構築時に登録する
  - Y は正確な順序付き整数演算を行う型。全ての X から Y への変換と a * Y(x) + b が正確に表現可能
  - add_segment の区間は low <= high
complexity:
  build: O(n log n)、n は入力座標数
  add_line/query/contains: O(log(m + 1))、m は異なる座標数
  add_segment: O(log(m + 1)^2)
  size/coordinates: O(1)
  Line.evaluate: O(1)
  memory: O(n + m)、座標配列と 4m + 1 個の optional ノード
pitfalls:
  - 離散座標のデータ構造で、未登録 x の query は nullopt
  - 線分は low を含み high を含まない半開区間
  - 整数オーバーフローを自動で飽和・検出しない
related:
  - data_structure/coordinate_compression
verification:
  unit:
    - verify/unit/data_structure/li_chao_tree.test.cpp
  stress:
    - verify/stress/data_structure/li_chao_tree.test.cpp
  online:
    - verify/online/data_structure/li_chao_tree.test.cpp
    - verify/online/data_structure/li_chao_tree_segment.test.cpp
---

# Li Chao Tree

一次関数 `y = a*x + b` を追加し、登録された整数座標での最小値または最大値を求めます。
傾き・追加順・問い合わせ順に単調性は不要です。指定した x 区間でだけ有効な線分も追加できます。
`cp::CoordinateCompression<X>` で離散座標を管理し、座標の差を計算しないため負の座標や極端な座標にも対応します。

## アルゴリズムの考え方

複数の直線 `y=a*x+b` に対して、指定した x での最小値または最大値を求めます。
例えば `dp[i]=min_j(a[j]*x[i]+b[j])` の形に整理できる DP では、状態 j が確定するたび
直線を追加できます。傾きやクエリの順序が単調である必要はありません。
この実装ではクエリに使う整数座標を最初に収集し、圧縮した順序上に木を作ります。

各ノードで現在の直線と新しい直線を比較し、区間の中央の座標で良い方を残します。
2 本の直線の差は一次式なので、優劣が逆転する場所は高々 1 か所です。
中央で負けた直線が勝つ可能性のある側だけへ再帰すれば、全座標での最良値の候補を
失わずに済みます。同じ傾きなら切片の良い方だけを残します。

クエリは対象座標の葉までの経路に保存された候補を比較します。
直線追加と検索は木の高さ分の O(log(m + 1))、m は異なる登録座標数です。
区間限定の追加は `[low,high)` を O(log m) 個のノードで覆い、各ノードで直線追加を
行うので O(log²(m + 1)) になります。

例えば `y=x` と `y=-x+4` の最小値は、登録座標 0,2,4 でそれぞれ 0,2,0 です。
圧縮後の順位ではなく元の x を代入するので、座標間の間隔が不均一でも構いません。
ただし登録されていない座標には `query` が答えず、`nullopt` を返します。
また、結果の最小値だけでなく比較する全ての `a*x+b` が Y に収まる必要があります。

## API

型は `cp::LiChaoTree<X = long long, Y = long long, bool Minimize = true>` です。
`Minimize = true` は最小、false は最大を求めます。係数と結果は Y 型です。
公開型 `Tree::Line` は `Y slope, intercept` の 2 フィールドを持ち、`{a, b}` で構築します。

| API | 動作 | 計算量 |
| --- | --- | --- |
| `Tree tree(std::vector<X> coordinates = {})` | 問い合わせ座標をソート・重複除去して構築。省略時は空 | O(n log n) |
| `tree.size() const -> std::size_t` | 異なる登録座標数 | O(1) |
| `tree.coordinates() const -> const std::vector<X>&` | 昇順の登録座標一覧 | O(1) |
| `tree.contains(X x) const -> bool` | x が登録されているか | O(log(m + 1)) |
| `tree.add_line(Line line)` | 全登録座標に対する直線を追加 | O(log(m + 1)) |
| `tree.add_line(Y a, Y b)` | `Line{a, b}` を追加 | O(log(m + 1)) |
| `tree.add_segment(X low, X high, Line line)` | [low, high) 内の登録座標に線分を追加 | O(log(m + 1)^2) |
| `tree.add_segment(X low, X high, Y a, Y b)` | `Line{a, b}` を区間に追加 | O(log(m + 1)^2) |
| `tree.query(X x) const -> std::optional<Y>` | x に適用される直線・線分の最小/最大値 | O(log(m + 1)) |
| `line.evaluate(X x) const -> Y` | `slope * static_cast<Y>(x) + intercept` | O(1) |

係数の演算・比較・コピーは O(1) とします。それ以外は各操作のコストを乗じてください。
未登録の x、または適用される直線・線分がない場合の query は nullopt です。
空の座標集合でも構築・追加・問い合わせができ、追加は何もせず問い合わせは nullopt です。
構築後に座標は追加できません。オンライン入力で問い合わせ座標が事前に分からない場合は、
全操作を読み込み、query の座標を集めて構築してから元の順序で操作を再生します。

線分の端点は登録座標でなくても構いません。low == high や登録座標を含まない区間への追加は何もしません。
逆順の区間は assert で検出し、`NDEBUG` 時も前提を守ってください。
返された座標配列の参照はオブジェクトの破棄・代入まで有効です。直線や線分の削除には対応しません。

## 評価とオーバーフロー

Y は順序付きの正確な整数演算を行う型を使ってください。剰余型や浮動小数点の近似演算は対象外です。
各登録 x が Y に変換可能で、**掛け算の中間値と加算後の値の両方**が Y に収まる必要があります。
この前提は query する座標だけでなく、挿入中に比較する登録座標にも必要です。
線分の場合はその有効区間内の登録座標で前提を満たしてください。
既定の long long は約 ±9.22e18 の範囲であり、呼び出し側で係数・座標の範囲から安全性を確認します。
最大値版は係数を符号反転せず比較の向きだけを変えるため、最小整数の符号反転による問題はありません。
同じ傾きは良い切片を残し、交点で同じ値になる直線はどちらを保持しても結果は同じです。

## Example

```cpp
#include <cp/data_structure/li_chao_tree.hpp>
#include <cassert>
int main() {
    cp::LiChaoTree<> low({-2, 0, 2});
    low.add_line(1, 3);
    low.add_segment(-1, 2, 0, -5);
    assert(low.query(-2) == 1);
    assert(low.query(0) == -5);
    assert(low.query(2) == 5);
    assert(!low.query(1));
    cp::LiChaoTree<long long, long long, false> high({-2, 0, 2});
    high.add_line(1, 3); high.add_line(-1, 3);
    assert(high.query(-2) == 5 && high.query(2) == 5);
}
```

## Verification

空座標集合、直線なし、同じ傾き、交点、負の座標、重複座標、線分の両端・空区間・未登録端点、
最小値版と最大値版、整数の最小・最大座標を単体検証します。
固定 seed の直線・線分追加列で各登録座標の答えを全直線の走査と比較します。
オンライン検証は [Line Add Get Min](https://judge.yosupo.jp/problem/line_add_get_min) と
[Segment Add Get Min](https://judge.yosupo.jp/problem/segment_add_get_min) の公開ケースをローカル実行します。
検証用プログラムは問い合わせ座標を事前収集し、元の操作順に再生します。
検証状態とヘッダー全文はドキュメントビルド時に生成されます。
