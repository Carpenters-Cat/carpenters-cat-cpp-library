---
title: Persistent Segment Tree
source: include/cp/data_structure/persistent_segment_tree.hpp
status: stable
tags:
  - data-structure
  - data-structure/range-query
aliases:
  - Persistent Segment Tree
  - 永続 Segment Tree
  - 永続セグメント木
  - Path Copying
requires:
  - op は結合的、e() は左右の単位元、S はコピー可能
  - n >= 0、添字 0 <= p < n、区間 0 <= l <= r <= n
  - Version は同じインスタンスが返した有効な版番号
complexity:
  build: 値列 O(n)、全要素 e() の版 O(1)
  set: 償却 O(log(n + 1))
  get/prod: O(log(n + 1))
  all_prod/size/versions/initial_version/node_count: O(1)
  memory: 値列構築 O(n)、各更新で O(log(n + 1)) ノードと O(1) の root
pitfalls:
  - set は新しい版番号を返し、既存の版は変更しない
  - 非可換な演算でも左から右の順序を保つ
  - 版・ノードを削除する API はなく全更新分のメモリを保持する
related: []
verification:
  unit:
    - verify/unit/data_structure/persistent_segment_tree.test.cpp
  stress:
    - verify/stress/data_structure/persistent_segment_tree.test.cpp
  online:
    - verify/online/data_structure/persistent_segment_tree.test.cpp
---

# Persistent Segment Tree

一点置換時に根から葉への経路だけをコピーし、過去の版を保持するモノイド Segment Tree です。
任意の過去の版から分岐でき、各版の区間積をオンラインで問い合わせられます。
`op(S, S) -> S` は結合的、`e() -> S` は左右の単位元で、可換性は不要です。

## アルゴリズムの考え方

過去の配列を壊さずに一点更新する Segment Tree です。版ごとに根の ID を持ち、
更新する葉への経路だけをコピーします。変更しない側の子は以前の版と共有します。
例えば `[1,2,3]` の版 v0 から中央を 9 にした v1 を作っても、v0 の和は 6、v1 は 13
のままです。v0 から別の更新を行えば、v1 とは独立した枝の版を作れます。

新しいノードの値はコピーした子と共有した子を `op` で結合して計算します。
既存ノードを書き換えないので、古い根からの検索結果が変化しないことが正しさの根拠です。
非可換な演算でも左の子・右の子の順に結合します。検索では指定した版の根から降り、
通常の Segment Tree と同じ区間分割を使います。

木の高さは O(log(n + 1)) なので、1 回の更新で増えるノード数と処理量もこの範囲です。
q 回の更新後のメモリは、値列から構築した場合 O(n+q log(n + 1)) です。
全要素が単位元の初期版は、どの深さでも使える単位元ノードを共有して O(1) で作ります。
ノードを vector に追加する再確保もあるため、更新時間の保証は償却です。

Rollback Union-Find のような「直前の状態へ戻す」操作と違い、保持している全ての版を
いつでも参照できます。代わりに更新ごとにメモリが増え、版やノードを削除する API は
ありません。長い履歴を扱う場合は `node_count()` と総更新数から必要メモリを見積もります。

## API

型は `cp::PersistentSegmentTree<S, op, e>`、公開型 `Version` は `std::size_t` です。

| API | 動作 | 計算量 |
| --- | --- | --- |
| `Tree tree(int n = 0)` | n 個の `e()` からなる初期版を共有の単位元ノードで構築 | O(1) |
| `Tree tree(const std::vector<S>& values)` | 値列から初期版を構築 | O(n) |
| `tree.size() const -> int` | 配列の要素数 | O(1) |
| `tree.initial_version() const -> Version` | 初期版番号 0 | O(1) |
| `tree.versions() const -> std::size_t` | 初期版を含む版の数 | O(1) |
| `tree.node_count() const -> std::size_t` | 共通の単位元ノードを含む確保済みノード数 | O(1) |
| `tree.set(Version v, int p, S x) -> Version` | v の p 番目を x に置換した新しい版を作る | 償却 O(log(n + 1)) |
| `tree.get(Version v, int p) const -> S` | 指定版の p 番目の値 | O(log(n + 1)) |
| `tree.prod(Version v, int l, int r) const -> S` | 指定版の [l, r) の積。空区間は `e()` | O(log(n + 1)) |
| `tree.all_prod(Version v) const -> S` | 指定版の全区間積。空配列は `e()` | O(1) |

演算・コピーは O(1) とします。それ以外は各操作のコストを乗じてください。
値列の要素数は int の最大値以下です。単位元版は値列を作らないため、疎な更新では大きな n も扱えます。
版番号は追加順の整数で、過去の版から set しても既存の版と版番号は保たれます。
別インスタンスの版番号を混用しないでください。整数であるため、その誤りは検出できません。
不正な添字・存在しない版番号は assert で検出し、`NDEBUG` 時は前提を守ってください。
演算のオーバーフローを呼び出し側で防いでください。

## メモリ

値列構築は単位元ノード 1 個と 2n - 1 個の木ノードを使います。空列は単位元ノード 1 個だけです。
各更新は最大 `ceil(log2(n)) + 1` ノードと root 1 個を追加し、変わらない部分木を共有します。
同じ値へ置換する場合も新しい版・ノードを作ります。U 回の更新後は O(n + U log(n + 1)) メモリです。
配列で管理するノード・root の再確保を含むため、set の時間計算量は償却です。
各版の保持・削除を選ぶ GC は行いません。必要な版数と値型のサイズからメモリを見積もってください。

## Example

```cpp
#include <cp/data_structure/persistent_segment_tree.hpp>
#include <cassert>
#include <vector>
long long op(long long a, long long b) { return a + b; }
long long e() { return 0; }
int main() {
    cp::PersistentSegmentTree<long long, op, e> tree(std::vector<long long>{1, 2, 3});
    const auto initial = tree.initial_version();
    const auto first = tree.set(initial, 1, 10);
    const auto branch = tree.set(initial, 0, -1);
    assert(tree.all_prod(initial) == 6);
    assert(tree.all_prod(first) == 14);
    assert(tree.all_prod(branch) == 4);
    assert(tree.prod(first, 1, 3) == 13);
    assert(tree.prod(initial, 2, 2) == 0);
}
```

## Verification

空列、単一要素、空区間、単位元版、過去の版の不変性、版の分岐、大きな疎配列、
各更新のノード増分を単体検証します。固定 seed の更新を版ごとにコピーした配列と比較し、
過去の任意の版の区間積・一点取得・全体積を確認します。文字列連結でも非可換な順序を検証します。
オンライン検証は [Point Set Range Composite](https://judge.yosupo.jp/problem/point_set_range_composite) の
公開ケースをローカル実行します。この問題は最新の版を使い、過去版と分岐は単体・乱択検証が担当します。
検証状態とヘッダー全文はドキュメントビルド時に生成されます。
