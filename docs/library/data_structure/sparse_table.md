---
title: Sparse Table
source: include/cp/data_structure/sparse_table.hpp
status: stable
tags:
  - data-structure
  - data-structure/range-query
aliases:
  - Sparse Table
  - スパーステーブル
  - 静的 RMQ
requires:
  - op は結合的かつ冪等、S はコピー可能
  - 要素数は int の最大値以下、区間は 0 <= l <= r <= n
complexity:
  build: O(n log(n + 1))
  prod/size: O(1)
  memory: O(n log(n + 1))
pitfalls:
  - 加算等の冪等でない演算には使えない
  - 空区間の prod は std::nullopt を返す
  - 配列の更新には対応しない
related: []
verification:
  unit:
    - verify/unit/data_structure/sparse_table.test.cpp
  stress:
    - verify/stress/data_structure/sparse_table.test.cpp
  online:
    - verify/online/data_structure/sparse_table.test.cpp
---

# Sparse Table

更新のない配列の区間 min・max・gcd 等を定数時間で計算します。
2 冪長の区間を前計算し、クエリは左右の重複する 2 区間を結合します。
演算 `op(S, S) -> S` に結合性と冪等性 `op(x, x) == x` が必要です。可換性は不要です。

## アルゴリズムの考え方

更新しない配列に対し、全ての開始位置と長さ 1,2,4,8,... の区間の集約値を前計算します。
長さ `2^k` の値は隣接する長さ `2^(k-1)` の 2 区間を結合して作れます。
例えば最小値の検索なら、一度表を作るだけで任意区間の最小値を定数時間で答えられます。

検索区間 `[l,r)` の長さ以下で最大の 2 の冪を L とし、
`[l,l+L)` と `[r-L,r)` の値を結合します。2L は区間長より大きいので区間全体を覆い、
中央では重なります。結合則と冪等性 `op(x,x)=x` により、重なりを二度数えても
答えが変わりません。可換性や単位元は不要です。

例えば `[5,2,7,1,4]` の全体の最小値は、先頭 4 個の最小値 1 と末尾 4 個の最小値 1
の最小値として求められます。和の場合は重なった部分が二重加算されるため使えません。
最小・最大・gcd は適します。空区間では単位元を仮定せず `nullopt` を返します。

段数が O(log(n + 1))、各段の要素数が O(n) なので構築・メモリは O(n log(n + 1)) です。
検索は 2 個の表参照と 1 回の演算だけで O(1) になります。
一点変更でも多くの前計算が無効になるため、更新が必要なら Segment Tree を選びます。

## API

| API | 動作 | 計算量 |
| --- | --- | --- |
| `cp::SparseTable<S, op> table(values = {})` | `std::vector<S>` を受け取って構築。省略時は空 | O(n log(n + 1)) |
| `table.size() const -> int` | 要素数 | O(1) |
| `table.prod(int l, int r) const -> std::optional<S>` | 半開区間 [l, r) の集約。空区間は nullopt | O(1) |

演算・コピーは O(1) とします。それ以外は各操作のコストを乗じてください。
単位元のない冪等な半群も扱えるように、空区間は `std::optional` で表します。
空配列の構築と `prod(0, 0)` は有効です。添字は assert で検出し、`NDEBUG` 時も前提を守ってください。
加算・xor・一般の積は冪等ではないため使えません。gcd の入力は非負とすると冪等性を保てます。

## Examples

```cpp
#include <cp/data_structure/sparse_table.hpp>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>
int minimum(int a, int b) { return std::min(a, b); }
int maximum(int a, int b) { return std::max(a, b); }
int gcd(int a, int b) { return std::gcd(a, b); }
int main() {
    std::vector<int> a{12, 6, 18, 3};
    cp::SparseTable<int, minimum> low(a);
    cp::SparseTable<int, maximum> high(a);
    cp::SparseTable<int, gcd> common(a);
    assert(low.prod(0, 3) == 6);
    assert(high.prod(1, 4) == 18);
    assert(common.prod(0, 3) == 6);
    assert(!low.prod(2, 2));
}
```

## Verification

空配列、長さ 1、最小・最大整数、デフォルト構築できない型を単体検証します。
固定 seed の配列で全区間を愚直な min・max・gcd と比較し、2 冪の前後の長さを含めます。
非可換かつ冪等な左射影でも積の順序を確認します。
オンライン検証は [Library Checker Static RMQ](https://judge.yosupo.jp/problem/staticrmq) の公開ケースをローカル実行します。
検証状態とヘッダー全文はドキュメントビルド時に生成されます。
