---
title: ACL-equivalent library overview
source: include/cp/utility/acl.hpp
status: stable
tags:
  - data-structure
  - graph
  - math
  - string
aliases:
  - ACL
  - AtCoder Library
  - ACL 相当
requires:
  - 各ライブラリの型・添字・法・数値範囲・演算の前提に従う
complexity:
  operations: 各ライブラリのページを参照
pitfalls:
  - 名前空間は atcoder ではなく cp
  - UnionFind の merge は代表元ではなく併合が行われたかを返す
related:
  - data_structure/fenwick_tree
  - data_structure/segment_tree
  - data_structure/lazy_segment_tree
  - data_structure/union_find
  - graph/max_flow
  - graph/min_cost_flow
  - graph/scc
  - graph/two_sat
  - math/modint
  - math/math
  - math/convolution
  - string/string_algorithms
verification:
  unit:
    - verify/unit/utility/acl.test.cpp
---

# ACL-equivalent library overview

[AtCoder Library の公開機能](https://atcoder.github.io/ac-library/production/document_ja/index.html)
を `cp` 名前空間で提供します。`#include <cp/utility/acl.hpp>` で全対象を読み込めます。
個別のヘッダーも単独で利用でき、提出用ソースへの展開では依存が重複除去されます。
API・利用例・計算量・前提・注意点・検証は以下の個別ページを参照してください。

| ACL | このライブラリ | 対応・差異 |
| --- | --- | --- |
| `fenwick_tree<T>` | [FenwickTree](../data_structure/fenwick_tree.md) | `cp::fenwick_tree<T>` の別名、線形時間のベクトル構築を追加 |
| `segtree<S, op, e>` | [SegmentTree](../data_structure/segment_tree.md) | `cp::segtree` の別名、区間積・両方向境界探索 |
| `lazy_segtree` | [LazySegmentTree](../data_structure/lazy_segment_tree.md) | `cp::lazy_segtree` の別名、点・区間作用・両方向境界探索 |
| `dsu` | [UnionFind](../data_structure/union_find.md) | 既存の実装を使用。`merge` は bool、代表元は `leader` で取得 |
| `static_modint`, `dynamic_modint` | [Modint](../math/modint.md) | 同名の `cp` 型、PascalCase の別名も提供 |
| `pow_mod`, `inv_mod`, `crt`, `floor_sum` | [Math](../math/math.md) | 同名の `cp` 関数 |
| `convolution`, `convolution_ll` | [Convolution](../math/convolution.md) | 固定法の NTT と正確な符号付き整数畳み込み |
| `mf_graph<Cap>` | [MaxFlow](../graph/max_flow.md) | 辺の追加・参照・変更、最大流、最小カット |
| `mcf_graph<Cap, Cost>` | [MinCostFlow](../graph/min_cost_flow.md) | 辺の追加・参照、flow と slope。呼び出しは1回 |
| `scc_graph` | [SccGraph](../graph/scc.md) | 反復 DFS で深いグラフに対応、トポロジカル順の成分 |
| `two_sat` | [TwoSat](../graph/two_sat.md) | 節の追加、充足判定、解 |
| `suffix_array`, `lcp_array`, `z_algorithm` | [String Algorithms](../string/string_algorithms.md) | 同名の `cp` 関数、文字列の byte は unsigned 扱い |

数学・フロー・文字列の一部は固定した ACL の CC0 ソースを本リポジトリへ取り込み、
ヘッダーの依存・名前空間・複数翻訳単位への対応を整えています。
[数学の由来](../../third_party/acl-math.md)と[グラフ・文字列の由来](../../acl-provenance.md)を参照してください。
ヘッダー以外にコードの正本は置きません。

## アルゴリズムの選び方と組合せ

このヘッダーは複数の公開ヘッダーをまとめて読み込む入口です。独立したアルゴリズムや
追加の実行時処理は持たず、計算量と前提は選んだ各構造・関数に従います。
ライブラリ全体を全て読み込むものではなく、上の表の ACL 相当の機能を対象にします。

| 解きたい問題 | 組合せの例 |
| --- | --- |
| 剰余で一点加算・区間和 | Modint を Fenwick Tree の値型にする |
| 区間の関数合成 | Segment Tree の結合を順序のある関数合成にする |
| 区間アフィン更新と和 | Lazy Segment Tree に和・長さとアフィン作用を渡す |
| 条件付きの真偽の割当て | TwoSat の節から含意グラフを作り SCC で判定する |
| 二部マッチング | Max Flow の辺容量を 1 にして始点・終点を加える |
| 多項式の係数計算 | Modint の列を Convolution で掛ける |

組み合わせるときは、演算の代数的前提も一致させます。
例えば Modint の除算は非零なら常にできるとは限らず、逆元が存在する必要があります。
Fenwick Tree は逆演算を使う加法可換群、Segment Tree は結合則と単位元、
Lazy Segment Tree は集約と更新の分配・合成が必要です。
ACL から移すコードでは名前空間と型名に加え、UnionFind::merge の bool 戻り値や
MinCostFlow の一回呼び出し制約も確認します。
必要な個別ヘッダーだけを読み込む使い方もでき、kpro の展開は依存を一度だけ含めます。

## Example

```cpp
#include <cp/utility/acl.hpp>
#include <cassert>
#include <vector>

int main() {
    using Mint = cp::modint998244353;
    cp::FenwickTree<Mint> sums(std::vector<Mint>{-1, 2, 3});
    assert(sums.sum(0, 3).val() == 4);
    cp::UnionFind uf(3);
    assert(uf.merge(0, 1));
    assert(uf.same(0, 1));
}
```

## Verification

このエントリの単体テストは全ヘッダーの同時利用、固定・動的 Modint と Fenwick Tree、
Modint と Segment Tree／Lazy Segment Tree の組合せを確認します。
個別エントリでは単体・乱択比較・Library Checker／AOJ の公開ケースを検証し、
`kpro verify` が全エントリの構成と結果を確認します。
