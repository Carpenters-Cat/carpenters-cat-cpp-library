---
title: Union-Find
source: include/cp/data_structure/union_find.hpp
status: stable
tags:
  - data-structure
  - data-structure/union-find
  - graph/connectivity
aliases:
  - Union-Find
  - ユニオンファインド
  - 素集合
  - 連結成分
  - DSU
  - disjoint set union
requires:
  - n >= 0、各頂点番号は 0 <= v < n
complexity:
  build: O(n)
  merge/same/leader/size: 償却 O(α(n))
  components: O(1)
  groups: O(n α(n))
  memory: O(n)
pitfalls:
  - 辺の削除・分離・ロールバックには対応しない
  - leader の代表元は merge によって変わり得る
  - leader/same/size は経路圧縮で内部状態を更新する
  - 不正な頂点番号は assert で検出する。NDEBUG 時は呼び出し側が前提を守る
related: []
verification:
  unit:
    - verify/unit/data_structure/union_find.test.cpp
  stress:
    - verify/stress/data_structure/union_find.test.cpp
  online:
    - verify/online/data_structure/union_find.test.cpp
---

# Union-Find

集合の併合と「同じ連結成分か」の判定を高速に行うデータ構造です。
union by size と経路圧縮を使います。C++23、名前空間は `cp` です。

## When to use

無向グラフの辺追加による連結性の管理、Kruskal 法、同値関係の管理に使います。
辺を削除したり過去の状態へ戻したりする場合は別のデータ構造が必要です。

## アルゴリズムの考え方

互いに重ならない集合を、親ポインタの木で表します。根がその集合の代表元です。
最初は各頂点が単独の根であり、2 頂点の根が同じかを調べれば同じ集合か分かります。
無向辺 `(a,b)` を追加するたびに `merge(a,b)` すれば、集合が連結成分に対応します。
Union-Find の親は連結性を表す管理用の木であり、元のグラフの辺や経路ではありません。

この実装は一つの配列 `parent_or_size_` を使います。負の値なら根で、絶対値が
集合のサイズです。非負なら親の頂点番号です。異なる根を併合するときは、サイズの
小さい木の根を大きい木の子にし、サイズを加え、成分数を 1 減らします。
同じ根の併合は何も変えず false を返すため、重複した辺や自己ループでも成分数は減りません。

`leader` は根を探した後、たどった頂点の親を全てその根に変更します。これを経路圧縮と
呼びます。同じ根につなぎ直すだけなので集合の所属は変わらず、次の検索が短くなります。
例えば併合後の親が `3 -> 2 -> 0` なら、`leader(3)` の後は `3 -> 0` となります。

サイズによる併合だけでも、深さが増えるたび所属集合が倍以上になるので高さは O(log n)
です。経路圧縮を組み合わせた操作列では、1 操作の償却時間が O(α(n)) になります。
α は非常にゆっくり増える逆 Ackermann 関数で、実用上はほぼ定数ですが、全ての
単一操作が最悪 O(1) という意味ではありません。`groups()` は全頂点の根を求めて
振り分けるので O(n α(n))、保持する配列と出力の大きさは O(n) です。

頂点番号は 0-indexed です。入力が 1-indexed なら変換します。代表元が最小の頂点とは
限らず、併合でも変わるため固定 ID にしないでください。差の制約には Potential
Union-Find、取り消しには Rollback Union-Find を選びます。

## API

| API | 動作 |
| --- | --- |
| `cp::UnionFind uf(n)` | n 個の独立した頂点を作る。省略時は空 |
| `uf.merge(a, b)` | 成分を併合し、実際に併合した場合だけ true |
| `uf.same(a, b)` | 同じ成分に所属するか |
| `uf.leader(v)` | v の所属成分の代表元 |
| `uf.size(v)` | v の所属成分の頂点数 |
| `uf.components()` | 現在の成分数 |
| `uf.groups()` | 各成分の頂点一覧。空成分は含めない |

`groups()` の頂点は昇順で返されます。成分の並び順には依存しないでください。

## Examples

```cpp
#include <cp/data_structure/union_find.hpp>
#include <cassert>

int main() {
    cp::UnionFind uf(4);
    assert(uf.merge(0, 1));
    assert(uf.merge(1, 2));
    assert(!uf.merge(0, 2));
    assert(uf.same(0, 2));
    assert(uf.size(1) == 3);
    assert(uf.components() == 2);
}
```

## Verification

空集合、単一頂点、重複併合、成分一覧を単体テストで確認します。
固定 seed の乱択テストでは、単純な成分ラベル管理との比較を行います。
オンライン検証は Library Checker の Unionfind 問題の全テストケースをローカル実行します。

## ACL DSU correspondence

既存の `cp::UnionFind` が ACL の `atcoder::dsu` 相当の機能を提供します。
同等の DSU を別途実装しません。

| ACL | cp | 対応と注意点 |
| --- | --- | --- |
| `dsu(n)` | `UnionFind(n)` | n 頂点を構築、省略時は空 |
| `leader(v)` | `leader(v)` | 同じ代表元機能 |
| `same(a,b)` | `same(a,b)` | 同一成分判定 |
| `size(v)` | `size(v)` | 成分サイズ |
| `groups()` | `groups()` | 成分ごとの頂点列、成分間の順序は保証しない |
| `merge(a,b) -> int` | `merge(a,b) -> bool` | cp は併合が新規かを返す。代表元が必要なら併合後 `leader(a)` |
| なし | `components()` | cp 独自の O(1) 成分数取得 |

両実装とも union by size と経路圧縮を用い、各操作は償却 O(α(n)) です。
ACL に対応する全機能は既存の単体・乱択・Library Checker 検証の対象です。
