---
title: Aho–Corasick
source: include/cp/string/aho_corasick.hpp
status: stable
tags:
  - string
aliases:
  - Aho Corasick
  - AC Automaton
  - エイホ・コラシック法
related:
  - string/trie
verification:
  unit:
    - verify/unit/string/aho_corasick.test.cpp
  stress:
    - verify/stress/string/aho_corasick.test.cpp
  online:
    - verify/online/string/aho_corasick.test.cpp
    - verify/online/string/aho_corasick_search.test.cpp
---

# Aho–Corasick

## Summary / When to use

複数のパターンを登録して failure link を構築し、テキストを 1 回走査して
パターン ID と一致位置を報告します。同じパターンの複数登録、接尾辞としての一致、
重なる一致をすべて扱います。報告不要なら出現数だけを集計し、大量の一致を列挙せずに済みます。
内部の木は `cp::Trie` で、接尾辞の全 ID リストを各ノードにコピーしません。

## Preconditions / Pitfalls

パターン・テキストは符号なし 8-bit バイト列であり、埋め込み NUL や 128..255 も
利用できます。Unicode の文字単位の位置ではありません。ノード数・カウンタの上限は Trie と同じです。
テキストの長さ + 1 と各出現数は `size_t` に収まる必要があります。

`add_pattern` の返す ID は登録順の 0..P-1。同じ文字列でも別の ID を返し、
一致・出現数を各 ID に返します。空パターンは空テキストでも 1 回、
長さ T のテキストでは 0..T の各境界で T+1 回一致します。
`Match` の `begin,end` は **バイトの半開区間 [begin,end)** です。
重なる一致を除外したり、最長一致だけに限定したりしません。

初期状態および追加直後は未構築です。空の辞書でも走査前に `build()` が必要です。
追加は既存のパターン ID / ノード ID を保持しますが failure link を無効化するため、
再度 build してから走査します。繰り返し build は構築済みなら何もしません。
走査・failure link の取得の構築条件と添字範囲を assert で確認します。
登録文字列のビューは保存しません。走査中のコールバックで辞書を変更しないでください。

## アルゴリズムの考え方

全パターンを Trie へ登録し、テキストを一度走査して複数パターンの出現を求めます。
現在の状態は「ここまで読んだテキストの接尾辞で、Trie に存在する最長の接頭辞」です。
次のバイトの辺がなければ、より短い候補を指す failure link をたどります。
根でも辺がなければ根へ戻ります。この遷移で最長候補の不変条件を維持します。

failure link は真の接尾辞へ向かうので文字列長が短くなります。
build は浅いノードから BFS し、既に求めた親の failure から子の候補を探します。
終端への output link を別に持ち、短い一致パターンの一覧を各ノードへコピーしません。
例えば `he`,`she`,`hers` を登録して `ushers` を読むと、`she` とその接尾辞の `he`、
続いて `hers` を報告できます。

テキスト走査で辺を進むと深さは 1 増え、failure をたどると減ります。
増える回数がテキスト長 T 以下なので、failure 遷移も全体で O(T) です。
固定バイトアルファベットでは、一致の報告数を M として列挙は O(T+M+1) になります。
疎な遷移を使う構築は登録文字列の総長 L を含めて O(L+V+P) で評価します。

個数だけが欲しければ、各状態の訪問数を failure の逆 BFS 順で親へ集計します。
その状態が出現した回数は failure 上の全接尾辞にも寄与するため、終端の集計が各パターンの
一致数になります。O(T+V+P) で、巨大な M を列挙する必要がありません。
重複パターンも別 ID、空パターンは先頭を含む全境界で一致します。
パターンを追加したら build をやり直してから検索してください。

## API

すべて `cp::AhoCorasick` のメンバです。`PatternId=size_t`、`NodeId=Trie::NodeId`。
`Match` は `pattern_id,begin,end` を持ち、等値比較できます。

| API | 意味 |
| --- | --- |
| `AhoCorasick()` | 未構築の空辞書 |
| `PatternId add_pattern(string_view pattern)` | 新しい ID でパターンを登録 |
| `void build()` | failure / terminal output link を構築 |
| `bool is_built()` | 現在のパターンに対し構築済みか |
| `template<class Callback> void for_each_match(string_view text,Callback callback)` | 一致ごとに `callback(Match)` を実行 |
| `vector<Match> matches(string_view text)` | 同じ順序の一致を配列で返す |
| `vector<size_t> count_matches(string_view text)` | 各パターン ID の出現数を返す |
| `size_t pattern_count()` / `size_t node_count()` | 登録数 / 根を含むノード数 |
| `NodeId pattern_node(PatternId id)` / `size_t pattern_length(PatternId id)` | 終端ノード / バイト長 |
| `NodeId failure_link(NodeId node)` | 最長の真の接尾辞のノード、根は 0 |
| `const Trie& trie()` | 共有する木の読み取り専用参照 |

報告順は `end` の昇順。同じ end では長いパターンから接尾辞順、同じパターン文字列
の ID は登録順です。空パターンは同じ境界の最後に登録順で報告します。
failure link は、登録パターンの接頭辞のうちノード文字列の最長の真の接尾辞を指します。

## Complexity

L は登録したパターンの総バイト数、V はノード数、P は登録数、T はテキスト長、
M は重複 ID・空パターンを含む報告数です。アルファベットを 256 バイトに固定します。
長さ m の登録は償却 O(m+1)、構築は O(L+V+P)、保存領域は O(V+P)。
疎な遷移で failure link を辿る構築の償却上界は登録パターンの根から終端までの経路で
評価するため、構築を無条件に O(V) とはしません。

`for_each_match` は O(T+M+1) 時間、走査自身の追加メモリは O(1)。
`matches` は同じ時間と O(M) の結果領域を使います。
`count_matches` は状態訪問数を failure link 上で逆 BFS 順に集計し、
O(T+V+P) 時間、O(V+P) 追加メモリです。一致数 M が大きくても列挙しません。
その他の参照 API は O(1)。`build` の一時 BFS 領域は O(V) です。

## Examples

```cpp
#include <cp/string/aho_corasick.hpp>
#include <cassert>
#include <vector>
int main() {
    cp::AhoCorasick ac;
    auto a = ac.add_pattern("a");
    auto aa = ac.add_pattern("aa");
    auto duplicate = ac.add_pattern("a");
    auto empty = ac.add_pattern("");
    ac.build();
    assert((ac.count_matches("aaa") == std::vector<std::size_t>{3, 2, 3, 4}));
    std::size_t reported = 0;
    ac.for_each_match("aaa", [&](cp::AhoCorasick::Match m) {
        assert(m.end <= 3);
        if (m.pattern_id == aa) assert(m.end - m.begin == 2);
        if (m.pattern_id == a || m.pattern_id == duplicate) assert(m.end - m.begin == 1);
        if (m.pattern_id == empty) assert(m.begin == m.end);
        ++reported;
    });
    assert(reported == 12 && ac.matches("").size() == 1);
}
```

## Verification

単体・固定 seed の検証は、全パターンを各位置で直接比較して ID・半開区間・数を照合します。
空、重複、共通接頭辞/接尾辞、重なり、全バイト、追加後の再構築を含めます。
failure link も愚直な最長接尾辞と比較します。4000 個の反復接頭辞に対する 10 万文字の
数の集計で、巨大な出力リストのコピーや全一致の列挙を必要としないことを確認します。

[Library Checker Aho Corasick](https://judge.yosupo.jp/problem/aho_corasick) は
テキスト検索ではなく、挿入順ノードの親・failure link・終端 ID を検証する問題です。
[AOJ String Search](https://onlinejudge.u-aizu.ac.jp/problems/ALDS1_14_D) では
各パターンの出現有無を `count_matches` で検証します。
