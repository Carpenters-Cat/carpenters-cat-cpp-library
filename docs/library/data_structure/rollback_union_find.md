---
title: Rollback Union-Find
source: include/cp/data_structure/rollback_union_find.hpp
status: stable
tags:
  - data-structure
  - data-structure/union-find
  - graph/connectivity
aliases:
  - Rollback Union-Find
  - Rollback DSU
  - Undoable Union-Find
  - ロールバック Union-Find
  - 巻き戻し Union-Find
requires:
  - n >= 0、各頂点番号は 0 <= v < n
  - rollback の引数は現在の履歴の prefix に対する有効な snapshot
complexity:
  build: O(n)
  merge/leader/same/size: O(log(n + 1))
  snapshot/components: O(1)
  rollback: O(k + 1)、k は取り消す成功併合数
  memory: O(n)
pitfalls:
  - 失敗した併合は履歴に記録せず snapshot は変わらない
  - 経路圧縮を行わず union by size のみを使う
  - 巻き戻しで捨てた未来の snapshot は再利用できない
related:
  - data_structure/union_find
verification:
  unit:
    - verify/unit/data_structure/rollback_union_find.test.cpp
  stress:
    - verify/stress/data_structure/rollback_union_find.test.cpp
  online:
    - verify/online/data_structure/rollback_union_find.test.cpp
---

# Rollback Union-Find

成功した併合を履歴に保存し、snapshot の時点まで取り消せる Union-Find です。
探索の分岐ごとの状態復元、時間区間をセグメント木に載せるオフライン動的連結性、
バージョン木を DFS するオフラインの永続 Union-Find 等に使います。

## アルゴリズムの考え方

成功した併合で変更する 2 個の根の情報を履歴に保存し、逆順に復元する Union-Find です。
`snapshot()` は成功した併合の履歴長を返し、`rollback(s)` はその長さまで戻します。
例えば snapshot の後で 0 と 1、次に 1 と 2 を併合し、snapshot に戻すと、両方の
併合が取り消されます。同じ成分への併合は状態を変えず、履歴も増やしません。

通常の Union-Find の経路圧縮を行うと、検索でも多数の親を書き換えてしまいます。
この実装は経路圧縮をせず、サイズによる併合だけを使います。
小さい成分を大きい成分の下につけるので、ある頂点の深さが 1 増えるたび所属成分の
大きさは少なくとも倍になります。したがって木の高さと根探索は O(log(n + 1)) です。

復元は各併合で保存した親・子の元のサイズを戻し、成分数を 1 増やします。
変更を逆順に取り消すので、後の併合が参照した状態を先に復元でき、snapshot 時の
分割が正確に戻ります。k 回の成功併合を戻す時間は O(k+1) です。

用途の一つはオフラインの動的連結性です。辺が存在する時間区間を時間軸の木に載せ、
DFS で入るときに併合し、出るときに snapshot へ戻すと、各時刻の状態を共有できます。
時間軸の木と探索は利用側で用意します。この型自身に辺の削除や任意の過去版への
アクセスはありません。戻した後に捨てられた履歴の snapshot は再利用しないでください。

## API

| API | 動作 | 計算量 |
| --- | --- | --- |
| `cp::RollbackUnionFind uf(n = 0)` | n 個の独立した頂点を構築 | O(n) |
| `uf.merge(int a, int b) -> bool` | 成分を併合。実際に併合した時だけ true | O(log(n + 1)) |
| `uf.leader(int v) const -> int` | 成分の代表元 | O(log(n + 1)) |
| `uf.same(int a, int b) const -> bool` | 同一成分か | O(log(n + 1)) |
| `uf.size(int v) const -> int` | 成分の頂点数 | O(log(n + 1)) |
| `uf.components() const -> int` | 成分数 | O(1) |
| `uf.snapshot() const -> Snapshot` | 現在の履歴位置を保存 | O(1) |
| `uf.rollback(Snapshot target)` | target まで成功した併合を取り消す | O(k + 1) |

公開型 `cp::RollbackUnionFind::Snapshot` は `std::size_t` です。`k` は今回取り消す成功併合数です。
経路圧縮は行わず union by size だけを使うため木の高さは O(log(n + 1)) です。
各成功併合で両根の旧状態を保存するため、巻き戻し時は O(1) で 1 件を復元できます。
履歴は現在の成功併合だけなので最大 n - 1 件、メモリは O(n) です。

## 履歴と snapshot の規則

自己併合・既に同じ成分の重複併合は false を返し、**履歴には記録しません**。
`rollback(snapshot() - 1)` は直近の成功併合を取り消す操作であり、直近の merge 呼び出しを取り消す操作ではありません。
分岐探索では毎回 `snapshot()` を取ってから作業し、保存した位置へ戻す方式を使ってください。

`target` は同じインスタンスの現在の履歴の prefix を指す有効な snapshot に限ります。
現在と同じ snapshot への rollback は何もしません。0 は初期状態です。
巻き戻して捨てた未来の snapshot を保存しても、別の分岐の履歴や状態を復元することはできません。
後の分岐で同じ整数値が現れても、それは以前の snapshot の有効性を回復しません。
履歴長を超える位置は assert で検出しますが、別インスタンスや別分岐の token は検出できません。
`NDEBUG` 時を含め、token の有効性と頂点番号は呼び出し側が守ってください。

## Example

```cpp
#include <cp/data_structure/rollback_union_find.hpp>
#include <cassert>
int main() {
    cp::RollbackUnionFind uf(4);
    uf.merge(0, 1);
    const auto outer = uf.snapshot();
    uf.merge(1, 2);
    const auto inner = uf.snapshot();
    assert(!uf.merge(0, 2));
    assert(uf.snapshot() == inner);
    uf.merge(2, 3);
    assert(uf.size(0) == 4);
    uf.rollback(inner);
    assert(!uf.same(0, 3));
    uf.rollback(outer);
    assert(uf.same(0, 1) && !uf.same(0, 2));
    uf.rollback(0);
    assert(uf.components() == 4);
}
```

## Verification

空集合、単一頂点、重複・自己併合、入れ子の snapshot、多段階の巻き戻し、
別分岐、union by size の方向、20 万頂点を単体検証します。
固定 seed の乱択操作を成功併合ごとにコピーした愚直な成分ラベルと比較します。
オンライン検証は [Library Checker Persistent Unionfind](https://judge.yosupo.jp/problem/persistent_unionfind)
をバージョン木の DFS と rollback で解き、公開ケースをローカル実行します。
検証用 DFS は明示的なスタックを使い、長いバージョン列でも再帰のスタックを消費しません。
この API 自体はオンラインの完全永続データ構造ではありません。
検証状態とヘッダー全文はドキュメントビルド時に生成されます。
