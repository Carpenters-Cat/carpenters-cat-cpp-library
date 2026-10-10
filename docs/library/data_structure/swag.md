---
title: SWAG (Sliding Window Aggregation)
source: include/cp/data_structure/swag.hpp
status: stable
tags:
  - data-structure
  - data-structure/range-query
aliases:
  - SWAG
  - Sliding Window Aggregation
  - スライディングウィンドウ集約
  - モノイドキュー
requires:
  - op は結合的な演算、e() は左右の単位元
  - S はコピー・ムーブ可能
complexity:
  build: O(n)
  push/pop: 償却 O(1)、単一の操作は最悪 O(n)
  prod/size/empty: O(1)
  memory: O(h)、h はこれまでの最大キュー長
pitfalls:
  - キュー先頭から末尾の順序で集約する
  - 空キューの prod は e()、pop は false を返す
  - 演算・コピーのコストが大きい場合は計算量にそのコストを掛ける
related: []
verification:
  unit:
    - verify/unit/data_structure/swag.test.cpp
  stress:
    - verify/stress/data_structure/swag.test.cpp
  online:
    - verify/online/data_structure/swag.test.cpp
---

# SWAG (Sliding Window Aggregation)

FIFO キューの全要素をモノイドで集約します。スライドする区間の総和・最小値・関数合成等に使います。
`op(S, S) -> S` と `e() -> S` をテンプレート引数で指定し、非可換な演算にも対応します。
逆元は不要です。

## アルゴリズムの考え方

キューを前半と後半の 2 個のスタックで表し、それぞれに積の累積値を保存します。
後半への push は末尾へ加えるので `op(これまでの積,新しい値)`、
前半へ移すときは先頭側に積み直すので `op(移した値,これまでの積)` です。
全体の積は `op(前半の積,後半の積)` で得られます。

例えばキューが `[a,b,c]` のとき、後半から前半へ c,b,a の順で移しても、前半の
集約値は `op(a,op(b,c))` となります。pop で a を外した後は b,c の積が残ります。
この順序を保つので、和や最小値だけでなく行列積や関数合成にも使えます。
結合則は必要ですが、逆演算と可換性は不要です。

前半が空のときだけ後半の全要素を移します。各要素は push、スタック間の移動、pop を
高々一度ずつ受けるため、操作列全体では要素数に比例する仕事しかありません。
移動を伴う 1 回の pop は O(n) でも、push/pop は償却 O(1)、積の取得は常に O(1) です。

幅 K のスライド区間なら、右端を push し、長さが K を超えたら pop して prod を読みます。
区間の両端が単調に右へ進む問題に向きます。任意位置の削除、両端への追加、過去の
区間への巻き戻しはこの API の対象外です。大きな値型では演算とコピーの費用も考えます。

## API

| API | 動作 | 計算量 |
| --- | --- | --- |
| `cp::SWAG<S, op, e> queue(values = {})` | `const std::vector<S>&` を先頭から順に追加して構築。省略時は空 | O(n) |
| `queue.push(S x)` | 末尾に x を追加 | 償却 O(1) |
| `queue.pop() -> bool` | 先頭を削除。空なら何もせず false、それ以外は true | 償却 O(1) |
| `queue.prod() const -> S` | 先頭から末尾への全要素の積。空なら `e()` | O(1) |
| `queue.size() const -> std::size_t` | 要素数 | O(1) |
| `queue.empty() const -> bool` | 空か | O(1) |

`cp::SlidingWindowAggregation<S, op, e>` は同じ型の別名です。
演算・コピー・ムーブは O(1) とします。それ以外は各操作のコストを乗じてください。
オーバーフロー等は演算の実装側で防いでください。

## 償却計算量

末尾側のスタックは追加順の積、先頭側のスタックは逆向きに積を持ちます。
先頭側が空の時だけ、末尾側の全要素を移し替えてから pop します。
各要素は追加・移し替え・削除をそれぞれ最大 1 回受けるため、操作列全体では要素数に比例する仕事です。
移し替えや vector の再確保を行う単一の push/pop は最悪 O(n)、償却は O(1) です。
両スタックの確保容量は要素削除後も保持するため、メモリは現在の長さではなく過去の最大長 h に対して O(h) です。

## Example

```cpp
#include <cp/data_structure/swag.hpp>
#include <cassert>
#include <string>
std::string op(std::string a, std::string b) { return a + b; }
std::string e() { return {}; }
int main() {
    cp::SWAG<std::string, op, e> queue;
    queue.push("a"); queue.push("b"); queue.push("c");
    assert(queue.prod() == "abc");
    assert(queue.pop());
    queue.push("d");
    assert(queue.prod() == "bcd");
    queue.pop(); queue.pop(); queue.pop();
    assert(queue.prod().empty() && !queue.pop());
}
```

文字列連結の例は順序を示すためのものです。文字列のコピー・連結は O(1) ではなく、上記の定数時間の計算量には当てはまりません。

## Verification

空キュー、単一要素、移し替え、両スタックの同時利用、再利用、20 万要素の追加・削除を単体検証します。
固定 seed の追加・削除列を deque の愚直集約と比較し、文字列連結と affine 合成で非可換な順序を確認します。
オンライン検証は [Library Checker Queue Operate All Composite](https://judge.yosupo.jp/problem/queue_operate_all_composite)
の公開ケースをローカル実行します。検証状態とヘッダー全文はドキュメントビルド時に生成されます。
