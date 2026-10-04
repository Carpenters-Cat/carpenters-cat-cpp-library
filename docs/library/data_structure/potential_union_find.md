---
title: Potential Union-Find
source: include/cp/data_structure/potential_union_find.hpp
status: stable
tags:
  - data-structure
  - data-structure/union-find
  - graph/connectivity
aliases:
  - Potential Union-Find
  - Weighted Union-Find
  - ポテンシャル付き Union-Find
  - 重み付き Union-Find
  - 差分制約
requires:
  - n >= 0、各頂点番号は 0 <= v < n
  - T は正確な等値判定を持つ加法可換群、T{} はゼロ
  - ポテンシャルの差と内部の加減算・符号反転が T に収まる
complexity:
  build: O(n)
  merge/leader/potential/same/size/difference: 償却 O(α(n))
  components: O(1)
  memory: O(n)
pitfalls:
  - merge(a, b, d) の符号は potential[b] - potential[a] = d
  - 矛盾する制約は採用せず、既存の関係を保持する
  - potential は代表元に対する相対値で、絶対的な原点ではない
related:
  - data_structure/union_find
verification:
  unit:
    - verify/unit/data_structure/potential_union_find.test.cpp
  stress:
    - verify/stress/data_structure/potential_union_find.test.cpp
  online:
    - verify/online/data_structure/potential_union_find.test.cpp
---

# Potential Union-Find

差分制約 `potential[b] - potential[a] = d` の追加、差の取得、矛盾判定を行います。
重み付き Union-Find とも呼ばれます。測定値の相対差、座標差、剰余による相対関係等に使います。
union by size とポテンシャルを保つ経路圧縮を使い、各成分の原点は代表元です。

## API

型は `cp::PotentialUnionFind<T = long long>` です。`T` は `+`、二項 `-`、単項 `-`、`==`、コピーを持ち、
`T{}` を加法のゼロとして扱える必要があります。可換性・結合性と正確な等値判定が必要です。

| API | 動作 | 計算量 |
| --- | --- | --- |
| `PotentialUnionFind<T> uf(n = 0)` | n 個の独立した頂点を構築 | O(n) |
| `uf.merge(int a, int b, T d) -> MergeResult` | `potential[b] - potential[a] = d` を追加し結果を返す | 償却 O(α(n)) |
| `uf.leader(int v) -> int` | 成分の代表元 | 償却 O(α(n)) |
| `uf.potential(int v) -> T` | `potential[v] - potential[leader(v)]` | 償却 O(α(n)) |
| `uf.same(int a, int b) -> bool` | 同一成分か | 償却 O(α(n)) |
| `uf.size(int v) -> int` | 成分の頂点数 | 償却 O(α(n)) |
| `uf.components() const -> int` | 成分数 | O(1) |
| `uf.difference(int a, int b) -> std::optional<T>` | 同一成分なら `potential[b] - potential[a]`、未接続なら nullopt | 償却 O(α(n)) |

`PotentialUnionFind<T>::MergeResult` は公開 enum class で、次の 3 値を持ちます。

| 値 | 意味 |
| --- | --- |
| `merged` | 異なる成分を接続した |
| `already_consistent` | 既に同じ成分で、要求した差が既存の差に一致した |
| `contradiction` | 既に同じ成分で差が一致しない。制約は採用しない |

矛盾を返しても既存の差は変わりません。成分に「矛盾した」というフラグを残す方式ではありません。
呼び出し側が矛盾を記録し、必要なら処理を中断してください。読み取り操作も経路圧縮のため const ではありません。
代表元や `potential(v)` の原点は併合で変わりますが、既知の頂点間の `difference` は保たれます。
`T` の演算とコピーは O(1) とします。それ以外は各操作のコストを乗じてください。
整数の中間値も含めてオーバーフローを防いでください。浮動小数点は誤差で等値判定が崩れるため適しません。
頂点番号は assert で検出し、`NDEBUG` 時は呼び出し側が前提を守ります。

## Example

```cpp
#include <cp/data_structure/potential_union_find.hpp>
#include <cassert>
int main() {
    using UF = cp::PotentialUnionFind<long long>;
    using Result = UF::MergeResult;
    UF uf(4);
    assert(uf.merge(0, 1, 5) == Result::merged);
    assert(uf.merge(1, 2, -2) == Result::merged);
    assert(uf.difference(0, 2) == 3);
    assert(uf.difference(2, 0) == -3);
    assert(!uf.difference(0, 3));
    assert(uf.merge(0, 2, 3) == Result::already_consistent);
    assert(uf.merge(0, 2, 4) == Result::contradiction);
    assert(uf.difference(0, 2) == 3);
}
```

## Verification

空集合、単一頂点、逆向きの併合、経路圧縮、正負の差、重複制約、矛盾、未接続問い合わせ、
大きな整数と独自の剰余型を単体検証します。固定 seed の乱択制約を制約グラフの BFS と比較し、
矛盾の拒否後も全頂点の差・成分サイズが正しいことを確認します。
オンライン検証は [AOJ DSL_1_B](https://onlinejudge.u-aizu.ac.jp/problems/DSL_1_B) の公開ケースをローカル実行します。
この問題の制約には矛盾がないため、矛盾検出は単体・乱択検証が担当します。
検証状態とヘッダー全文はドキュメントビルド時に生成されます。
