---
title: Coordinate Compression
source: include/cp/data_structure/coordinate_compression.hpp
status: stable
tags:
  - data-structure
aliases:
  - 座標圧縮
  - coordinate compression
  - rank compression
requires:
  - T はコピー可能で、operator< が狭義弱順序を定める
complexity:
  build: O(n log n)
  index/lower_bound/upper_bound/contains: O(log m)
  value/size/empty/values: O(1)
  memory: O(n)、構築後の要素数は m
pitfalls:
  - 圧縮後の添字の差は元の座標の距離を保存しない
  - 未登録の値の index は std::nullopt を返す
  - value の添字は 0 <= rank < size() を満たす必要がある
  - 構築後の座標追加はできない。必要な座標を集めてから構築する
related: []
verification:
  unit:
    - verify/unit/data_structure/coordinate_compression.test.cpp
  stress:
    - verify/stress/data_structure/coordinate_compression.test.cpp
  online:
    - verify/online/data_structure/coordinate_compression.test.cpp
---

# Coordinate Compression

ソートと重複除去によって、疎な座標や大きい値を `[0, m)` の連続した添字に変換します。
元の値の順序を保ち、Fenwick Tree や Segment Tree の前処理に利用できます。
入力の値同士の減算を行わないため、整数型の最小値・最大値も扱えます。
文字列など、狭義弱順序で比較できる型にも対応します。

## アルゴリズムの考え方

座標圧縮は、値そのものを大小関係の等しい整数の順位へ置き換えます。
例えば `[100, -20, 100, 7]` の異なる値を整列すると `[-20, 7, 100]` になり、
元の列は `[2, 0, 2, 1]` と表せます。大きな座標でも順位を配列の添字にできるため、
Fenwick Tree の頻度表や Wavelet Matrix の内部表現に使えます。

構築ではソート後に同値な隣接要素を除去します。同値性は `==` ではなく
`!(a < b) && !(b < a)` で判定するため、比較だけを備える型も利用できます。
検索時は二分探索し、`lower_bound(x)` は x 未満の異なる値の数、
`upper_bound(x)` は x 以下の異なる値の数を返します。
`index(x)` は探索位置に同値な値がある場合だけ順位を返します。

整列済みの異なる値へ一対一に順位を振るので、順序と同値性を保存できます。
一方、差や距離は保存しません。上の例で順位差 1 が座標差 27 や 93 に対応するため、
区間の実際の長さが必要なら `value(rank)` で元の座標を参照します。
追加する値を後から登録する API はないので、必要な座標を先に収集してください。

ソートが構築の O(n log n) を占め、重複除去は O(n) です。
異なる値が m 個なら検索は O(log(m + 1))、順位から値への参照は O(1) です。
比較や値のコピーが定数時間でない型では、そのコストも掛かります。

## API

| API | 動作 |
| --- | --- |
| `cp::CoordinateCompression<T> c(values = {})` | `std::vector<T>` を受け取り、昇順の異なる値を構築 |
| `c.size()` / `c.empty()` | 登録された異なる値の数 / 空かどうか |
| `c.index(x)` | 登録された値の `std::optional<std::size_t>` の添字。未登録なら `std::nullopt` |
| `c.contains(x)` | `x` と同値の値が登録されているか |
| `c.lower_bound(x)` | `x` 未満の異なる値の数。未登録の値にも使用可能 |
| `c.upper_bound(x)` | `x` 以下の異なる値の数。未登録の値にも使用可能 |
| `c.value(rank)` | 圧縮前の値。通常は `const T&`、`bool` は値を返す。不正な添字は assert で検出 |
| `c.values()` | 昇順に並んだ全座標への `const std::vector<T>&` |

`index`・`contains` の同値性は `!(a < b) && !(b < a)` です。
`operator==` は必要ありません。比較上同値でも表現が異なる値がある場合、
`value` はその同値類の代表値を返します。整数・文字列では元の値をそのまま復元します。
`bool` の座標は値を返し、その他の型の参照はオブジェクトの破棄・代入まで有効です。
`NDEBUG` 時も `value` の前提を守ってください。
境界検索は最後の値より大きい入力では `size()` を返します。

## Examples

```cpp
#include <cp/data_structure/coordinate_compression.hpp>
#include <cassert>
#include <vector>

int main() {
    std::vector<long long> a{1000000000, -7, 1000000000, 42};
    cp::CoordinateCompression<long long> c(a);
    assert(c.size() == 3);
    assert(*c.index(-7) == 0);
    assert(c.value(*c.index(42)) == 42);
    assert(!c.index(0));
    assert(c.lower_bound(0) == 1);
    assert(c.upper_bound(42) == 2);
    // 元の座標が [0, 100) に入る添字は [1, 2)。
    assert(c.lower_bound(100) - c.lower_bound(0) == 1);
}
```

## Verification

単体テストでは空列、重複、負数、整数の最小値・最大値、未登録値、文字列、
等値比較演算子を持たない型を確認します。固定 seed の乱択テストで集合の列挙から
得られる順位・境界と比較し、元の値の復元と順序を確認します。
Library Checker の Static Range Frequency では、座標圧縮で分類した位置列に
二分探索を行い、未登録値も含む区間内出現回数を検証します。
