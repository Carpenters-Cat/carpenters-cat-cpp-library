---
title: Wavelet Matrix
source: include/cp/data_structure/wavelet_matrix.hpp
status: stable
tags:
  - data-structure
  - data-structure/range-query
aliases:
  - Wavelet Matrix
  - ウェーブレット行列
  - 区間順序統計
  - 区間頻度
requires:
  - T はコピー可能、operator< が狭義弱順序を定める
  - 要素数は int の最大値以下、区間は 0 <= l <= r <= n
  - range_frequency の値域は low <= high
complexity:
  build: O(n log(n + 1))
  kth_smallest/frequency/count_less/range_frequency: O(log(m + 1))、m は異なる値の数
  size: O(1)
  memory: O(n + ceil(n / 64) log(m + 1)) ワード、T の座標配列を含む
pitfalls:
  - 配列は静的で構築後の更新には対応しない
  - k は 0-indexed。範囲外の k と空区間の kth_smallest は nullopt
  - 値域は low を含み high を含まない半開区間
related:
  - data_structure/coordinate_compression
verification:
  unit:
    - verify/unit/data_structure/wavelet_matrix.test.cpp
  stress:
    - verify/stress/data_structure/wavelet_matrix.test.cpp
  online:
    - verify/online/data_structure/wavelet_matrix.test.cpp
    - verify/online/data_structure/wavelet_matrix_frequency.test.cpp
---

# Wavelet Matrix

静的配列の区間内で k 番目の値、値の出現回数、指定値域の要素数を求めます。
`cp::CoordinateCompression<T>` で値を順序を保つ非負 rank に変換するため、
負数、整数の最小・最大値、文字列、`operator==` を持たない順序型にも対応します。
元の T にビット演算・減算は行いません。同値性は `!(a < b) && !(b < a)` です。

## API

| API | 動作 | 計算量 |
| --- | --- | --- |
| `cp::WaveletMatrix<T> wm(values = {})` | `const std::vector<T>&` から静的配列を構築。省略時は空 | O(n log(n + 1)) |
| `wm.size() const -> int` | 要素数 | O(1) |
| `wm.kth_smallest(int l, int r, int k) const -> std::optional<T>` | [l, r) で 0-indexed の k 番目に小さい値 | O(log(m + 1)) |
| `wm.frequency(int l, int r, const T& x) const -> int` | [l, r) 内で x と同値な要素数 | O(log(m + 1)) |
| `wm.count_less(int l, int r, const T& x) const -> int` | [l, r) 内で x 未満の要素数 | O(log(m + 1)) |
| `wm.range_frequency(int l, int r, const T& low, const T& high) const -> int` | [l, r) 内で low <= x < high の要素数 | O(log(m + 1)) |

m は異なる値の数です。比較・コピーは O(1) とします。それ以外は各操作のコストを乗じてください。
各 rank bit のビット列は 64 bit 単位に格納し、ブロックごとの popcount prefix を用いて O(1) で rank を計算します。
構築時の一時 rank 配列は O(n) ワード、構築後のビット列と rank index は O(ceil(n / 64) log(m + 1)) ワードです。
座標配列は重複除去後も確保容量が最大 n のため、全体のメモリには O(n) を含めます。

空区間の頻度・値域頻度・count_less は 0 です。未登録値の頻度は 0 です。
`k < 0`、`k >= r - l` の場合 `kth_smallest` は nullopt を返します。
値域が空の `range_frequency(l, r, x, x)` は 0 です。逆順の値域と不正な配列区間は assert で検出し、
`NDEBUG` 時は呼び出し側が前提を守ってください。比較上同値な値の表現が違う場合、kth_smallest は同値類の代表値を返します。

## Example

```cpp
#include <cp/data_structure/wavelet_matrix.hpp>
#include <cassert>
#include <vector>
int main() {
    cp::WaveletMatrix<long long> wm(std::vector<long long>{7, -3, 7, 2, -3});
    assert(wm.kth_smallest(0, 5, 2) == 2);
    assert(wm.frequency(1, 5, -3) == 2);
    assert(wm.range_frequency(0, 5, -3, 7) == 3);
    assert(wm.count_less(0, 5, 0) == 2);
    assert(!wm.kth_smallest(2, 2, 0));
}
```

## Verification

空列、全要素同一、重複、負数、整数の最小・最大値、未登録値、空値域、k の両端と範囲外、
文字列、bool、順序比較だけを持つ型を単体検証します。固定 seed の配列の区間ソート・走査と全操作を比較し、
64 bit のブロック境界と 2 冪前後の異なる値の数を含めます。
オンライン検証は [Range Kth Smallest](https://judge.yosupo.jp/problem/range_kth_smallest) と
[Static Range Frequency](https://judge.yosupo.jp/problem/static_range_frequency) の公開ケースをローカル実行します。
検証状態とヘッダー全文はドキュメントビルド時に生成されます。
