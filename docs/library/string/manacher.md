---
title: Manacher
source: include/cp/string/manacher.hpp
status: stable
tags:
  - string
aliases:
  - マナカー法
  - 回文半径
  - Palindrome Radii
related: []
verification:
  unit:
    - verify/unit/string/manacher.test.cpp
  stress:
    - verify/stress/string/manacher.test.cpp
  online:
    - verify/online/string/manacher.test.cpp
---

# Manacher

## Summary / When to use

各文字を中心とする奇数長回文と、文字間を中心とする偶数長回文の最大半径を
線形時間で求めます。前計算後は任意の半開区間が回文かを O(1) で判定できます。
ハッシュを用いず、判定は確定的です。

## Preconditions / Pitfalls

入力は `std::string_view` のバイト列です。NUL、128..255 を含め利用でき、
UTF-8 のコードポイント分解や正規化は行いません。
元の view は保存せず、構築後に元文字列を破棄できます。
入力長 n は `INT_MAX` 以下で、配列を確保できる必要があります。
長さ超過は `std::length_error`、確保失敗は標準コンテナの例外を伝播します。
添字は `int` で、範囲の前提は assert で確認します (`NDEBUG` では呼び出し側が保証します)。

**奇数半径は中心文字を含みます。** `odd[c]=k` は最大区間
`[c-k+1,c+k)`、長さ `2*k-1` を表します。非空入力の各値は 1 以上です。
**偶数中心は文字の直前の隙間です。** `even[g]=k` は最大区間
`[g-k,g+k)`、長さ `2*k` を表します。g=0 は先頭、g=n は末尾の隙間です。
偶数配列は端の隙間を含む **n+1 要素** で、両端の値は 0 です。
例えば `"abacaba"` の odd[3]=4 は `[0,7)`、`"abba"` の even[2]=2 は `[0,4)` です。

## API

| API | 意味 |
| --- | --- |
| `cp::Manacher(string_view text)` | 奇数・偶数半径を前計算 |
| `m.size()` | 入力長 n |
| `m.odd_radii()` | `const vector<int>&`、n 個の奇数半径 |
| `m.even_radii()` | `const vector<int>&`、n+1 個の偶数半径 |
| `m.odd_radius(center)` | 中心 `0 <= center < n` の奇数半径 |
| `m.even_radius(gap)` | 隙間 `0 <= gap <= n` の偶数半径 |
| `m.odd_interval(center)` | 奇数中心の最大回文の半開区間 `pair<int,int>` |
| `m.even_interval(gap)` | 偶数中心の最大回文の半開区間 `pair<int,int>` |
| `m.is_palindrome(left,right)` | `0 <= left <= right <= n` の区間の回文判定 |

空文字列では奇数配列は空、偶数配列は `{0}` です。
空区間は常に回文と判定し、両端の偶数中心の最大区間は `[0,0)` と `[n,n)` です。
半径が表す最大回文と同じ中心を持つ、より短い区間も回文です。
半径から全ての回文を列挙すると、出力総数が O(n²) になる場合があります。
半径配列への参照はその `Manacher` オブジェクトの生存中に利用してください。

## Complexity

構築は時間 O(n)、メモリ O(n)。各取得・区間変換・回文判定は O(1) です。
全操作は再帰を使いません。

## Examples

```cpp
#include <cp/string/manacher.hpp>
#include <cassert>
#include <utility>

int main() {
    cp::Manacher odd("abacaba"), even("abba");
    assert(odd.odd_radius(3) == 4);
    assert((odd.odd_interval(3) == std::pair{0, 7}));
    assert(even.even_radius(2) == 2);
    assert((even.even_interval(2) == std::pair{0, 4}));
    assert(even.is_palindrome(1, 3)); // "bb"
    assert(even.is_palindrome(4, 4)); // empty
    assert(!even.is_palindrome(0, 3));
}
```

## Verification

単体テストでは空・単一文字・奇数／偶数回文・端の隙間・バイナリ入力を確認し、
30 万文字の同一文字列では全中心の半径を既知の値と比較します。
固定 seed の乱択では各中心を愚直に拡張し、全ての半開区間を直接照合して比較します。
Library Checker の Enumerate Palindromes では奇数中心と内部の偶数中心を交互に並べ、
公開ケースの回文長と比較します。
