---
title: 2-SAT
source: include/cp/graph/two_sat.hpp
status: stable
tags:
  - graph
  - graph/connectivity
aliases:
  - TwoSat
  - 2 Sat
  - 充足可能性
  - ACL two_sat
related: ['graph/scc']
verification:
  unit:
    - verify/unit/graph/two_sat.test.cpp
  stress:
    - verify/stress/graph/two_sat.test.cpp
  online:
    - verify/online/graph/two_sat.test.cpp
---

# 2-SAT

## Summary / When to use

各節が 2 個のリテラルからなる論理式について充足可能性と解を求めます。
含意グラフを SCC で分解します。

## Preconditions / Pitfalls

変数数は `0 <= n <= INT_MAX/2`、変数番号は `[0,n)`。
`answer()` は直前の `satisfiable()` が true だった場合の解を使います。
節を追加したら再度 `satisfiable()` を呼びます。false の後の解の内容には依存しません。
節の値 f/g は「その変数がその値である」というリテラルです。

## アルゴリズムの考え方

2-SAT は各節が高々 2 個のリテラルの OR である論理式の充足判定です。
`add_clause(i,f,j,g)` は `(x_i==f) OR (x_j==g)` を追加します。
節 A∨B は「¬A なら B」「¬B なら A」と同値なので、2 本の含意辺を作ります。
各変数に false と true の 2 頂点を用意し、SCC で含意の循環を調べます。

同じ変数の false と true が同じ成分にあると、一方から他方も必須となり、どちらを
選んでも矛盾します。別成分なら、縮約 DAG の順序を用いて含意と矛盾しない真偽を選べます。
この実装では成分 ID がトポロジカル順なので、false の ID が true より小さい変数を
true にします。これにより全ての含意、したがって全ての元の節を満たす解になります。

x を true に固定するには `(x OR x)`、false には `(¬x OR ¬x)` を追加します。
「x なら y」は `(¬x OR y)`、「ちょうど一方が true」は `(x OR y)` と `(¬x OR ¬y)` の 2 節です。
1 節で 3 個以上の選択肢を OR する一般 SAT は直接表現できません。
変数数 n、節数 m ならグラフは 2n 頂点・2m 辺で、判定時間・メモリは O(n+m) です。
`answer()` は成功した判定後に読み、節を追加したら改めて判定してください。

## API / ACL correspondence

| API | 意味 |
| --- | --- |
| `cp::TwoSat(n = 0)` | ACL `two_sat` に対応 |
| `add_clause(i,f,j,g)` | `(x_i == f) OR (x_j == g)` を追加 |
| `satisfiable() -> bool` | 充足可能性を判定し、可能なら解を保持 |
| `answer() const -> vector<bool>` | 保持している解のコピー |

API はクラス名以外 ACL と同じです。複数の解がある場合の選択は一意ではありません。
同じ変数を両側に指定して強制値や恒真節を表現できます。
変数 0 個の式は充足可能です。

## Complexity

構築 O(n)、節追加償却 O(1)、判定 O(n+m)、解のコピー O(n)、メモリ O(n+m)。
m は節数です。

## Examples

```cpp
#include <cp/graph/two_sat.hpp>
#include <cassert>
int main() {
    cp::TwoSat sat(2);
    sat.add_clause(0,true,0,true); // x_0 を true にする
    sat.add_clause(0,false,1,true);
    assert(sat.satisfiable());
    assert(sat.answer()[0] && sat.answer()[1]);
}
```

## Verification

単体テストでは空の式、強制値、恒真節、矛盾、繰り返し判定を確認します。
乱択では全割り当ての列挙と充足可能性を比較し、返された解が全節を満たすことも確認します。
オンライン検証は Library Checker の 2 Sat です。
