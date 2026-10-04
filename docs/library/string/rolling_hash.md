---
title: Rolling Hash
source: include/cp/string/rolling_hash.hpp
status: stable
tags:
  - string
  - randomized
aliases:
  - ローリングハッシュ
  - Substring Hash
  - Hash LCP
related:
  - string/string_algorithms
verification:
  unit:
    - verify/unit/string/rolling_hash.test.cpp
  stress:
    - verify/stress/string/rolling_hash.test.cpp
  online:
    - verify/online/string/rolling_hash.test.cpp
---

# Rolling Hash

## Summary / When to use

部分文字列のハッシュ取得、比較、連結、最長共通接頭辞 (LCP) を扱います。
二つの素数を使う多項式ハッシュです。空区間や、異なる元文字列から切り出した区間も
同じ基数を共有すれば比較・連結できます。

## Preconditions / Pitfalls

**ハッシュ衝突は起こり得ます。** 二つの剰余と長さが一致しても、文字列の一致を保証しません。
`equal` は衝突により偽陽性を返し、`lcp` は実際より長い値を返す可能性があります。
暗号学的なハッシュではありません。公開された固定 seed に対する敵対的入力も考慮してください。
厳密な判定が必要なら元の列で照合するか、Suffix Array / Z Algorithm などを使います。
ランダムな seed は呼び出し側で選べますが、衝突を排除しません。

比較・連結する値には **同一の基数の組** を使います。
同じ seed は環境に依存せず同じ基数の組を生成します。
異なる seed が必ず異なる組を生成するとは限りません。
`from_seed` は固定の SplitMix64 手順を二回適用して基数を選びます。
既定 seed は `0`、その基数は `(505946798, 306193453)` です。
プロセスごとに暗黙に乱数を生成することはありません。
再現可能な実行には seed を記録し、複数の `RollingHash` に共有してください。

入力は `std::string_view` の **バイト列** です。NUL、128..255 も利用でき、
各バイトを符号なし整数にして 1..256 に写します。
UTF-8 のコードポイント、正規化、大小文字の同一視は行いません。
入力の view は保存しないため、構築後に元文字列を破棄できます。

区間は半開 `[left,right)`、位置と長さは `std::size_t` です。
範囲の前提は assert で検査します (`NDEBUG` では呼び出し側が保証します)。
入力長は `SIZE_MAX` より小さく、配列を確保できる必要があります。
入力長が `SIZE_MAX` なら `std::length_error`、確保失敗は標準コンテナの例外を伝播します。
連結後の長さは `SIZE_MAX` まで表現でき、超過は `std::length_error` です。
剰余の乗算は `uint64_t` で正確に計算でき、拡張整数型は使いません。

## API

`P` は `cp::RollingHashParameters`、`V` は `cp::RollingHashValue`、`H` は `cp::RollingHash` です。

| API | 意味 |
| --- | --- |
| `P::from_seed(uint64_t seed=0)` | 再現可能な基数の組を生成 |
| `P{base_first, base_second}` | 基数を明示指定。各基数は `[257,modulus-2]` |
| `P::modulus_first`, `P::modulus_second` | 固定素数 `1000000007`, `1000000009` |
| `p.validate()` | 範囲外の基数なら `std::invalid_argument` |
| `p == q` | 基数の組の一致 |
| `H(string_view text, P p=P::from_seed())` | 前計算。基数を検査 |
| `H(string_view text, uint64_t seed)` | seed から基数を生成して前計算 |
| `h.size()`, `h.parameters()` | 入力長と基数の組 |
| `h.get(left,right)` | `[left,right)` のハッシュ。`0 <= left <= right <= size()` |
| `h.whole()` | 入力全体のハッシュ |
| `H::concat(const V& left,const V& right)` | left の後に right を連結したハッシュ |
| `h.equal(left,right,other,other_left,other_right)` | 二つの区間の長さとハッシュを比較 |
| `h.lcp(other,first_pos=0,second_pos=0,limit=SIZE_MAX)` | 二つの位置からの LCP。両方の残り長と limit 以下 |
| `V(P p=P::from_seed())` | 指定基数の空列のハッシュ (連結の単位元) |
| `v.length()`, `v.parameters()` | 長さと基数の組 |
| `v.fingerprint()` | 二つの剰余の `pair<uint32_t,uint32_t>` |
| `v.compatible(other)` | 基数の組が一致するか |
| `v == other` | 基数、長さ、二つの剰余が全て一致するか |

`lcp` の位置は `0..size()` (末尾を含む) で、末尾同士や limit=0 の結果は 0 です。
基数の異なる値の `operator==` は false です。
`concat`、`equal`、`lcp` は、空区間の場合も基数不一致なら `std::invalid_argument` を投げます。
空ハッシュの剰余は `(0,0)` で、同じ基数の空値は連結の単位元です。
`V` は剰余と長さに加え `base^length` も保持します。
任意の値を連結でき、元の文字列の長さや生存期間には依存しません。
`fingerprint()` だけを比較すると長さや基数の検査を失うため、通常は `operator==` を使います。

## Complexity

長さ n の構築は時間・メモリ O(n)。`get`、`whole`、`equal`、`concat` は O(1)。
`lcp` は比較する最大長を L として O(log(L+1)) 時間、追加メモリ O(1)。
`RollingHashValue` は O(1) メモリです。全操作は再帰を使いません。

## Examples

```cpp
#include <cp/string/rolling_hash.hpp>
#include <cassert>

int main() {
    auto parameters = cp::RollingHashParameters::from_seed(26);
    cp::RollingHash a("banana", parameters), b("bandana", parameters);
    assert(a.lcp(b) == 3);
    assert(a.equal(1, 4, a, 3, 6)); // "ana"
    auto joined = cp::RollingHash::concat(a.get(0, 3), a.get(3, 6));
    assert(joined == a.whole());
    cp::RollingHashValue empty(parameters);
    assert(cp::RollingHash::concat(empty, joined) == joined);
}
```

## Verification

単体テストでは seed=0 の既知の基数、空・単一・反復・全域バイト値、互換性エラー、
連結長のオーバーフロー、30 万バイトの入力を確認します。
固定 seed の乱択では直接の部分文字列比較、連結した列の再ハッシュ、LCP の直接走査と比較し、
連結の結合則と単位元も検査します。
Library Checker の Z Algorithm を各位置の `lcp` で解き、公開ケースを検証します。
