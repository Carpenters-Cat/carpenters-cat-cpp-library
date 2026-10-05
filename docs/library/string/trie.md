---
title: Trie
source: include/cp/string/trie.hpp
status: stable
tags:
  - string
aliases:
  - Prefix Tree
  - トライ木
related:
  - string/aho_corasick
verification:
  unit:
    - verify/unit/string/trie.test.cpp
  stress:
    - verify/stress/string/trie.test.cpp
  online:
    - verify/online/string/trie_dictionary.test.cpp
---

# Trie

## Summary / When to use

文字列の多重集合を接頭辞ごとに共有します。完全一致・接頭辞の有無と登録数を
調べる辞書です。終端と経路を区別するため、ある単語が他の単語の接頭辞でも扱えます。
Aho–Corasick はこの同じ Trie 表現を利用します。

## Preconditions / Pitfalls

8-bit の符号なしバイト（0..255）を文字として扱い、NUL、128..255 も利用できます。
UTF-8 のコードポイントや Unicode 正規化は処理しません。入力は `string_view` ですが
内容を木にコピーするので呼び出し後の入力の寿命には依存しません。

空文字列も登録できます。重複登録は別の出現として数えます。
`count("")` は空文字列の登録数、`prefix_count("")` は全登録数です。
`find_node("")` は空の辞書でも根 0 を返しますが、`has_prefix("")` は
辞書が非空のときだけ true です。`find_node` の存在は単語の終端を意味しません。
削除は提供しません。

ノード ID は `int`、根は 0、新しい接頭辞は初めて現れた順に採番し、追加しても変わりません。
ノード数は `int` の最大値以下、登録数・各カウンタは `size_t` に収まる必要があります。
ノード指定 API の範囲は assert で確認します。`children` の参照は追加で無効になることが
あるため、走査しながら `insert` しないでください。根の `parent` は -1、`symbol` は 0
ですが、後者はラベルではなくダミー値です。

## API

すべて `cp::Trie` のメンバです。`NodeId=int`、`Edge` は `symbol: unsigned char` と
`child: NodeId` を持ちます。

| API | 意味 |
| --- | --- |
| `Trie()` | 根のみの空辞書 |
| `NodeId insert(string_view word)` | 1 出現を追加し、終端ノードを返す |
| `size_t count(string_view word)` | 完全一致の出現数 |
| `bool contains(string_view word)` | 完全一致の単語があるか |
| `size_t prefix_count(string_view prefix)` | 指定接頭辞で始まる登録の数、重複を含む |
| `bool has_prefix(string_view prefix)` | 指定接頭辞で始まる登録があるか |
| `optional<NodeId> find_node(string_view prefix)` | 終端でなくても接頭辞のノード、なければ nullopt |
| `size_t size()` / `size_t node_count()` | 全出現数 / 根を含むノード数 |
| `optional<NodeId> transition(NodeId node,unsigned char byte)` | 1 バイトの辺を進む、なければ nullopt |
| `const vector<Edge>& children(NodeId node)` | バイト昇順の辺 |
| `NodeId parent(NodeId node)` / `unsigned char symbol(NodeId node)` | 親 / 親からの辺のラベル |
| `size_t terminal_count(NodeId node)` | 指定ノードで終わる単語の出現数 |

## Complexity

バイトアルファベットを固定すると、長さ m の追加は償却 O(m+1)、
検索・接頭辞検索は O(m+1)、
1 辺の検索は O(1)、その他の参照 API は O(1)。可変次数 d の実際の辺検索は
二分探索 O(log(d+1))、新規辺追加には O(d) のシフトがあり、d は最大 256 です。
ノード数 V に対して辺とノードのメモリは O(V)。各ノードは存在する辺だけを持ち、
256 要素の遷移表は確保しません。木全体のコピーは O(V) です。

## Examples

```cpp
#include <cp/string/trie.hpp>
#include <cassert>
int main() {
    cp::Trie words;
    auto terminal = words.insert("apple");
    assert(words.insert("apple") == terminal);
    words.insert("app");
    words.insert("");
    assert(words.count("apple") == 2);
    assert(words.prefix_count("app") == 3);
    assert(words.has_prefix("ap") && !words.contains("ap"));
    assert(words.count("") == 1 && words.prefix_count("") == 4);
    assert(words.terminal_count(*words.find_node("app")) == 1);
}
```

## Verification

単体検証では重複、空文字列、単語の接頭辞、全 256 バイト、追加後の ID とコピーを確認します。
固定 seed の多重集合の完全一致・接頭辞比較と、20 万文字の単語の検証を行います。
[AOJ Dictionary](https://onlinejudge.u-aizu.ac.jp/problems/ALDS1_4_C) の公開ケースでは
完全一致の insert/find を検証します。
