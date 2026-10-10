---
title: Pollard Rho factorization
source: include/cp/math/factorization.hpp
status: stable
tags:
  - math
  - math/number-theory
  - randomized
aliases:
  - PollardRho
  - Pollard-Rho
  - Brent
  - factorize_u64
  - 素因数分解
requires:
  - 入力は 1..UINT64_MAX、負数は受け取らない
  - GCC/Clang の __uint128_t を利用する
complexity:
  factorize: 最小素因数 p の探索に期待 O(sqrt p) の剰余演算、最悪時間の保証なし
  sort: 因子数 K に O(K log K)
  memory: 再帰と出力に O(log n)、乱数器は固定サイズ
pitfalls:
  - 0 は invalid_argument、1 は空列
  - seed は探索の再現性を制御し、出力の昇順因子は seed に依存しない
related:
  - math/primality
  - math/sieve
verification:
  unit:
    - verify/unit/math/factorization.test.cpp
  stress:
    - verify/stress/math/factorization.test.cpp
  online:
    - verify/online/math/factorization.test.cpp
---

# Pollard Rho factorization

Miller–Rabin で素数を判定し、Brent 方式の Pollard–Rho で合成数を分割します。
uint64_t 全域を対象とし、2^63 以上にも対応します。

## アルゴリズムの考え方

Pollard's rho は `f(x)=x²+c (mod n)` の列を生成し、異なる位置の差と n の gcd から
因子を探します。素因数 p で見た列が先に同じ値へ衝突すると、その差が p の倍数になり、
gcd が 1 と n の間の非自明な因子になる可能性があります。
得た因子と商を再帰的に分け、Miller–Rabin で素数と分かった値を出力します。

実装は Brent 型の区間倍増と、最大 128 個の差の積をまとめた gcd を使います。
毎回 gcd を計算するより回数を減らせます。まとめた gcd が n になったら保存位置から
差を一つずつ調べ直します。退化した列や反復上限に達した試行では、初期値と c を
選び直します。試行上限は各試行のもので、全体の最悪実行時間を制限するものではありません。

例えば 91 を非自明な因子 7 で分けられれば、7 と 13 が素数と確認できて終了します。
gcd で得た因子は必ず n の約数なので、最後の全因子の積は元の n です。
出力をソートし、`factor_powers` は同じ素数を指数へまとめます。1 の素因数列は空です。

ランダムな列とみなした衝突の目安から、最小素因数 p の探索は期待 O(sqrt p) の剰余演算
と考えられます。最悪時間の保証はなく、素数判定・再帰分割・ソートも必要です。
seed を固定すると試行を再現できますが、試行回数が一定になるわけではありません。
0 には素因数分解がなく例外になります。小さい範囲の反復分解なら SPF 表の方が向きます。

## API

| API | 動作 |
| --- | --- |
| `cp::PollardRho rho(uint64_t seed = default_seed)` | 探索用の mt19937_64 を構築 |
| `rho.reseed(uint64_t seed)` | 乱数状態を初期化 |
| `rho.factorize(uint64_t n)` | 重複を含む昇順の vector<uint64_t> |
| `rho.factor_powers(uint64_t n)` | 昇順の vector<pair<uint64_t,int>>、素数と指数 |
| `cp::factorize_u64(uint64_t n, uint64_t seed = PollardRho::default_seed)` | 一回の分解用の便利関数 |
| `cp::PollardRho::default_seed` | 固定の既定 seed、0x9e3779b97f4a7c15 |

1 は空列を返し、0 は有限の素因数分解を持たないため `std::invalid_argument` を投げます。
負数は対象外です。入力を unsigned へ暗黙変換せず、正の整数であることを確認します。

128 bit の剰余積と、桁あふれしない剰余加算で f(x)=x²+c を計算します。
128 ステップの積をまとめて gcd を計算します。gcd が n になったら最後のバッチを
一歩ずつ確認し、退化した周期や 2^20 ステップで解けない試行は次のパラメーターへ
再試行します。全体の再試行数と最悪時間には上限を設けません。半素数でよく引用する
期待 O(n^(1/4)) は最小素因数の探索に期待 O(sqrt p) を要することによります。

`mt19937_64` の出力を直接剰余でパラメーターへ写すので、同じ seed・同じ呼び出し列で
探索を再現できます。オブジェクトの状態は分解ごとに進みます。時間や乱数により誤った
因子を返すことはなく、分割後の各因子を決定的な素数判定で確認します。

```cpp
#include <cp/math/factorization.hpp>
#include <cassert>
int main() {
    cp::PollardRho rho(123);
    assert((rho.factor_powers(72) == std::vector<std::pair<uint64_t,int>>{{2,3},{3,2}}));
    assert((cp::factorize_u64(1000000007ULL*1000000009ULL) ==
            std::vector<uint64_t>{1000000007ULL,1000000009ULL}));
}
```

小さい整数は試し割りと比較し、乱択 uint64 では全因子の素数性と 128 bit での積の復元を
確認します。2^63、2^64-1、約 2^32 の素数の平方と半素数、複数 seed をテストします。
Library Checker Factorize の全ケースをローカルで実行します。
