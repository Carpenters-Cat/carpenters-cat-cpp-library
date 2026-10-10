---
title: Suffix Array, LCP Array, Z Algorithm
source: include/cp/string/string_algorithms.hpp
status: stable
tags:
  - string
aliases:
  - SA-IS
  - Suffix Array
  - 接尾辞配列
  - LCP
  - 最長共通接頭辞
  - Z Algorithm
  - Z アルゴリズム
  - ACL string
related: []
verification:
  unit:
    - verify/unit/string/string_algorithms.test.cpp
    - verify/unit/string/string_algorithms_upper.test.cpp
  stress:
    - verify/stress/string/string_algorithms.test.cpp
  online:
    - verify/online/string/suffix_array.test.cpp
    - verify/online/string/lcp_array.test.cpp
    - verify/online/string/z_algorithm.test.cpp
---

# Suffix Array, LCP Array, Z Algorithm

## Summary / When to use

接尾辞の辞書順、隣接接尾辞の LCP、各位置からの文字列と全体の接頭辞の LCP を求めます。
部分文字列数、文字列検索、接頭辞の比較に使います。
ACL と同等の SA-IS・Kasai・Z Algorithm を提供します。

## Preconditions / Pitfalls

長さは `int` に収まる必要があります。`std::string` は Unicode の文字ではなく
**符号なしのバイト列** として扱います（0..255、NUL を含め利用できます）。
文字列の正規化や UTF-8 のコードポイント分解は呼び出し側が行います。
整数値域を指定するときは `upper >= 0`、全要素は `[0,upper]`。
大きな `upper` はその分のメモリと時間を使います。
`upper=INT_MAX` も値域として許されますが、必要なメモリを確保できなければ
通常の `std::bad_alloc` などの割当て例外が発生します。

`lcp_array` の `sa` は同じ列の正しい接尾辞配列である必要があります。
サイズ・添字範囲を assert で確認しますが、整列と重複なしは呼び出し側の前提です。
空入力は全 API で空配列を返します。空列への LCP 対応は ACL からの拡張です。

## アルゴリズムの考え方

### Suffix Array

接尾辞 `s[i..]` を辞書順に並べ、その開始位置だけを保存します。
`banana` なら `[5,3,1,0,4,2]` で、`a,ana,anana,banana,na,nana` の順です。
接頭辞が指定パターンと一致する接尾辞は連続するので、二分探索で出現範囲を求められます。
空の接尾辞は含みません。

長い入力は SA-IS で構築します。接尾辞を、次の接尾辞より小さい S 型と大きい L 型へ
分類し、L→S の境界（LMS）の部分列を並べます。それらに順位を付けた小さい問題を
再帰的に解き、LMS の正しい順から文字ごとの bucket 内へ他の接尾辞を誘導します。
同じ先頭文字の接尾辞の順は後続の接尾辞順で決まるので、L 型を前から、S 型を後ろから
走査して前の文字を付けた接尾辞を配置できます。LMS は隣り合わず個数が高々半分なので、
線形走査と縮小する再帰を合わせて O(n+upper) です。
短い列は愚直比較と doubling（比較長を 2 倍にして順位を更新）に切り替えます。
汎用値の入力では先にソートで座標圧縮するため O(n log n) です。

### LCP Array

辞書順で隣接する接尾辞の共通接頭辞長を Kasai 法で求めます。
元の開始位置を順に処理し、直前の共通長 h から次の比較を高々 1 小さい長さで始めます。
先頭を一文字除くと一致部分も一文字短くなる性質により、比較を一からやり直しません。
h の減少は各位置で高々 1、成功した比較による増加も全体 O(n) なので時間は O(n) です。
SA と区間最小値を組み合わせれば、任意の 2 接尾辞の LCP にも利用できます。

### Z Algorithm

`z[i]` は文字列全体の接頭辞と `s[i..]` の共通長です。既知の一致区間の右端を持ち、
その内側では接頭辞側の z 値を再利用し、右端より先だけ比較します。
右端の増加と各位置の失敗比較を数えると O(n) です。
`ababa` では `[5,0,3,0,1]` です。`pattern + 区切り + text` の z 値がパターン長以上なら
その位置に一致があります。区切りは入力に現れない値にし、必要なら整数列を使います。
3 API とも文字列は unsigned byte として扱い、UTF-8 の文字境界は呼び出し側で管理します。

## API / ACL correspondence

| API | 戻り値 |
| --- | --- |
| `cp::suffix_array(const string& s)` | 接尾辞開始位置の辞書順配列 |
| `cp::suffix_array(const vector<int>& s, int upper)` | 値域を指定した SA-IS |
| `cp::suffix_array(const vector<T>& s)` | 要素を座標圧縮した接尾辞配列 |
| `cp::lcp_array(const string& s,const vector<int>& sa)` | `sa[i]` と `sa[i+1]` の LCP |
| `cp::lcp_array(const vector<T>& s,const vector<int>& sa)` | 同上 |
| `cp::z_algorithm(const string& s)` | 接頭辞と `s[i..]` の LCP、`z[0]=n` |
| `cp::z_algorithm(const vector<T>& s)` | 同上 |

関数名・オーバーロードは ACL と同じです。
SA は空の接尾辞を含みません。非空入力の LCP 長は n-1、Z 長は n です。
汎用 SA の T は `<` と `!=` による整合する全順序が必要です。
LCP と Z の T は等値比較が必要です。

## Complexity

文字列 SA は O(n)、値域指定 SA は O(n+upper)、汎用 SA は O(n log n)。
LCP・Z は O(n)。メモリは SA が O(n+upper)、汎用 SA・LCP・Z が O(n)。
短い入力では愚直法・doubling に切り替えます。

## Examples

```cpp
#include <cp/string/string_algorithms.hpp>
#include <cassert>
#include <string>
#include <vector>
int main() {
    std::string s="banana";
    auto sa=cp::suffix_array(s);
    assert((sa==std::vector<int>{5,3,1,0,4,2}));
    assert((cp::lcp_array(s,sa)==std::vector<int>{1,3,0,0,2}));
    assert((cp::z_algorithm(std::string("aaaa"))==std::vector<int>{4,3,2,1}));
}
```

## Verification

単体テストでは空・単一要素・負整数の圧縮・汎用型・0/128/255 のバイト値を確認します。
乱択では接尾辞を直接比較するソート、LCP と Z の愚直走査と比較します。
30 万文字の同一文字列でも各位置の値を確認します。
SA-IS の値域境界では `upper=INT_MAX` の割当てサイズを確認し、巨大な割当てを
テスト内で拒否することで、メモリを消費せずに整数オーバーフローの回帰を防ぎます。
Library Checker の Suffix Array、Number of Substrings（SA と LCP）、
Z Algorithm の公開ケースをオンライン検証に使います。

## Provenance

[ACL の利用元・ライセンス](../../acl-provenance.md)。
