---
title: Modint
source: include/cp/math/modint.hpp
status: stable
tags:
  - math
  - math/number-theory
aliases:
  - modint
  - StaticModint
  - DynamicModint
  - 固定法
  - 動的法
requires:
  - 固定法は 1 <= mod <= INT_MAX、動的法は 1 <= mod <= 2000001000
  - 除算と inv は値と法が互いに素、pow の指数は非負
complexity:
  arithmetic: O(1)、除算は O(log mod)
  pow: O(log exponent)
  memory: 値ごとに O(1)
pitfalls:
  - 同じ dynamic_modint ID の法を変更した後は古い値を使用できない
  - raw の引数は 0 <= value < mod
related:
  - math/math
  - math/convolution
verification:
  unit:
    - verify/unit/math/modint.test.cpp
  stress:
    - verify/stress/math/modint.test.cpp
  online:
    - verify/online/math/convolution.test.cpp
---

# Modint

整数を法で正規化し、剰余演算を行います。ACL の `static_modint` / `dynamic_modint`
と同じ API を `cp` 名前空間で提供します。

## API

| API | 動作 |
| --- | --- |
| `cp::static_modint<mod>` / `cp::StaticModint<mod>` | コンパイル時に法を指定する型 |
| `cp::dynamic_modint<id>` / `cp::DynamicModint<id>` | ID ごとに法を共有する型。後者の ID 既定値は -1 |
| `cp::modint998244353`, `cp::modint1000000007` | よく使う固定法の別名 |
| `cp::modint` | `dynamic_modint<-1>` の別名 |
| `Mint()`, `Mint(integer)` | 0 または符号付き・符号なし整数を正規化 |
| `Mint::mod()`, `x.val()` | 法と正規化済みの値を int で取得 |
| `Dynamic::set_mod(m)` | 法を変更する。既定の法は 998244353 |
| `Mint::raw(v)` | 正規化を省略する構築。`0 <= v < mod` 必須 |
| `+ - * /`, `+= -= *= /=` | 演算・複合代入。整数を暗黙変換できる |
| 前置・後置 `++ --`, 単項 `+ -`, `== !=` | 正規化を維持する演算・比較 |
| `x.pow(long long n)` | n >= 0 の累乗。0^0 は 1 mod mod |
| `x.inv()` | gcd(x, mod) = 1 の乗法逆元 |

法は素数でなくても構いません。固定法が素数なら逆元に Fermat の小定理を使い、
それ以外は拡張 Euclid 法を使います。法 1 では全ての値と逆元が 0 です。
除算・逆元・指数の不正な前提は assert で検出しますが、`raw` の範囲は検査しません。
動的な法の変更は既存の値を再正規化せず、並列アクセス時の同期も行いません。

## Example

```cpp
#include <cp/math/modint.hpp>
#include <cassert>
int main() {
    using Mint = cp::modint998244353;
    Mint x = -1;
    assert((x + 2).val() == 1);
    assert((Mint(12) / 3).val() == 4);
    using Dynamic = cp::DynamicModint<0>;
    Dynamic::set_mod(12);
    assert(Dynamic(5).inv().val() == 5);
}
```

## Verification and provenance

整数型の境界、法 1、合成数、動的法の ID 分離を単体テストし、乱択で通常の整数演算と
照合します。オンライン検証では畳み込みを通して固定法を検証します。
[ACL 公式 modint API](https://atcoder.github.io/ac-library/production/document_ja/modint.html)
との違いは名前空間と追加の PascalCase 別名です。
[CC0 ソースの由来と変更点](../../third_party/acl-math.md)も参照してください。
