---
title: Formal Power Series
source: include/cp/math/formal_power_series.hpp
status: stable
tags:
  - math
  - math/number-theory
aliases:
  - FPS
  - FormalPowerSeries
  - 形式的べき級数
  - 多項式
  - Newton法
  - 多点評価
  - 補間
  - Taylor shift
requires:
  - Mint は cp の固定法 modint、法 p は 3 <= p <= 2000001000 の素数
  - 全ての中間畳み込みについて出力長を切り上げた 2 の冪が p-1 を割る
  - log と exp と pow と sqrt の要求係数数は 0 <= n < p
complexity:
  arithmetic: 加減算とスカラー演算は O(N)、乗算は M(N) = O(N log N)
  inv/divide/log/exp/pow: O(M(N))、pow の定数項累乗は追加 O(log exponent)
  sqrt: O(M(N)) と定数項の Tonelli-Shanks 法
  divmod/taylor_shift: O(M(N))
  multipoint/interpolate: O(M(N) log N)、メモリ O(N log N)
  memory: 単独の FPS 演算は O(N)
pitfalls:
  - inv と FPS 除算は分母の定数項が非零、log は定数項 1、exp は定数項 0
  - 多項式の divmod と FPS の divide は別の演算
  - interpolate の点は法 p において相異なる
related:
  - math/modint
  - math/convolution
  - math/combinatorics
  - math/linear_recurrence
verification:
  unit:
    - verify/unit/math/formal_power_series.test.cpp
  stress:
    - verify/stress/math/formal_power_series.test.cpp
  online:
    - verify/online/math/inv_of_formal_power_series.test.cpp
    - verify/online/math/log_of_formal_power_series.test.cpp
    - verify/online/math/exp_of_formal_power_series.test.cpp
    - verify/online/math/pow_of_formal_power_series.test.cpp
    - verify/online/math/sqrt_of_formal_power_series.test.cpp
    - verify/online/math/division_of_polynomials.test.cpp
    - verify/online/math/multipoint_evaluation.test.cpp
    - verify/online/math/polynomial_interpolation.test.cpp
    - verify/online/math/polynomial_taylor_shift.test.cpp
---

# Formal Power Series

NTT 畳み込み、Newton 法、積木で形式的べき級数と多項式を扱います。
係数は低次から並び、`cp::FormalPowerSeries<Mint>` と `cp::FPS<Mint>` は同じ型です。
`std::vector<Mint>` を継承しており、添字・反復・size・resize・push_back 等は通常の
vector と同じです。動的 modint は対応しません。

## Precision and field

空列は零多項式です。列の長さを保持し、末尾の 0 は自動削除しません。
`F(n)` は n 個の 0、`F{1,2}` は 1+2x です。vector からのコピー・移動もできます。
`truncated(n)` は x^n 未満の係数を **ちょうど n 個** 返し、不足分を 0 で補います。
級数演算も要求係数数 n 個を返します。`n=0` は空列を返し、定数項の条件を問いません。
`trim()` は末尾の 0 を除去し、`degree()` は零多項式で -1 を返します。

係数体は 3 <= p <= 2000001000 の素数 p の固定法 modint を使います。積分では 1,...,size の逆元が必要なため
size < p、log/exp/pow/sqrt は要求 n < p とします。NTT の長さ制約は全ての中間積にも
適用されます。最大の入力長・要求精度・点数+1 を N とすると、中間長に 4N の余裕を
取るのが十分な目安です。998244353 では N <= 2^21 の範囲ならこの余裕を確保できます。
実際の使用可能な長さは演算と入力によって異なり、各積で convolution の assert を
検査します。汎用法や最大サイズへ達する場合は中間長も確認してください。

## API

以下で `F=cp::FPS<Mint>`、n は要求係数数です。

| API | 結果・前提 |
| --- | --- |
| `f.coefficient(int i)` | 非負添字の係数。保存範囲外は 0 |
| `f.truncated(int n)`, `f.trim()`, `f.degree()` | 精度指定・末尾削除・数学的次数 |
| `f+g`, `f-g`, `f*g`, `-f`, 複合代入 | 加減は長い方の長さ、乗算は空列があれば空、それ以外は n+m-1 |
| `f+c`, `f-c`, `f*c`, `f/c`, 複合代入 | c は Mint。加減は定数項を更新し、空列は長さ 1 に。除算は c 非零 |
| `c+f`, `c-f`, `c*f` | スカラーを左に置いた演算 |
| `f.derivative()` | 微分。空列は空、それ以外は長さ size-1 |
| `f.integral()` | 積分定数 0 の積分。長さ size+1、空列では `{0}` |
| `f.inv(int n)` | f の逆元 mod x^n。n>0 なら f[0] 非零 |
| `f.divide(const F& g, int n)` | f/g mod x^n。n>0 なら g[0] 非零 |
| `f/g`, `f/=g` | 精度を f.size() に固定した FPS 除算 |
| `f.divmod(F g)` | 多項式の (商,余り)。零除数不可。両出力を trim し、余りの次数は g より小さい |
| `f.log(int n)` | log(f) mod x^n。n>0 なら f[0]=1 |
| `f.exp(int n)` | exp(f) mod x^n。n>0 なら f[0]=0 |
| `f.pow(uint64_t e, int n)` | 非負整数乗 f^e mod x^n。0^0=1、先頭の 0 に対応 |
| `f.sqrt(int n)` | `optional<F>`。平方根が存在しなければ nullopt |
| `f.evaluate(Mint x)` | Horner 法で f(x)。O(size) |
| `f.taylor_shift(Mint c)` | f(x+c) の係数。同じ長さ、size<=p |
| `cp::multipoint_evaluate(f, vector<Mint> points)` | 各点の値。重複した点も可、空の点列は空 |
| `cp::interpolate(vector<Mint> points, vector<Mint> values)` | 長さ n の次数 n 未満の補間多項式。両入力同長で点は相異なる |

平方根では x^n 未満の最初の非零次数が偶数で、その係数が体の平方剰余である必要が
あります。最初の非零次数が奇数、または係数が非平方剰余なら nullopt、零級数なら
n 個の 0 を返します。定数項の平方根に Tonelli–Shanks 法を使います。p=3 mod 4
なら指数法、その他は小さい方の平方根を選びます。返された解を符号反転しても解です。
打ち切りのため自由な高次係数が生じる場合、Newton 法で一つの解を選びます。

定数項・次数・点の重複等の不正な前提は assert で検出し、NDEBUG 時は呼び出し側で
守ります。全ての演算は剰余体上の演算であり、実数の解析的収束を扱いません。

## Examples

```cpp
#include <cp/math/formal_power_series.hpp>
#include <cassert>
int main() {
    using M = cp::modint998244353;
    using F = cp::FPS<M>;
    F f{1,2,3};
    assert(f.log(8).exp(8) == f.truncated(8));
    assert((f * f.inv(8)).truncated(8) == F{1}.truncated(8));
    auto [q,r] = F{1,3,2}.divmod(F{1,1});
    assert(q == F({1,2}) && r.empty());
    auto values = cp::multipoint_evaluate(f, std::vector<M>{0,1,2});
    assert(cp::interpolate(std::vector<M>{0,1,2}, values) == f);
    F square = F{1,1}.pow(2,3);
    assert(square == F({1,2,1}));
    auto root = square.sqrt(3);
    assert(root && (*root * *root).truncated(3) == square);
    assert(f.taylor_shift(2) == F({17,14,3}));
}
```

## Algorithms and verification

inv/log/exp/sqrt は倍増する Newton 法、pow は先頭の 0 と定数倍を除いて log/exp、
多項式除算は逆順の級数除算、Taylor shift は階乗を掛けた一回の畳み込みです。
多点評価は積木の剰余伝播、補間は根多項式の微分値を重みとする積木の集約です。
Tonelli–Shanks の定数項計算は O(log^2 p) の体演算に加え、非平方剰余の探索を行います。
大きな入力にも同じ高速アルゴリズムを使います。

単体テストは空列、零、定数、多項式除算と級数除算、平方根がない場合、uint64 最大の
指数、先頭の 0、精度 0 を確認します。固定 seed の比較テストは愚直積・二項展開・
各点の直接評価と照合し、逆元、exp/log、除算再構築、平方根の二乗、補間を検証します。
4096 係数で Newton/NTT と積木の経路も検証します。対応する Library Checker の逆元、
log、exp、pow、sqrt、多項式除算、多点評価、補間、Taylor shift を全ケース実行します。
