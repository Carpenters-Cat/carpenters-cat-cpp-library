---
title: Linear recurrence and BMBM
source: include/cp/math/linear_recurrence.hpp
status: stable
tags:
  - math
  - math/number-theory
aliases:
  - Berlekamp-Massey
  - Bostan-Mori
  - BMBM
  - 線形漸化式
  - 漸化式の復元
requires:
  - Berlekamp-Massey の係数は素数法の modint、その他は FPS 対応の固定法 modint
  - Bostan-Mori の分母の定数項は非零
  - 全ての中間畳み込みが convolution の長さ制約を満たす
complexity:
  berlekamp_massey: 長さ N の prefix に O(N^2)、メモリ O(N)
  bostan_mori: 長さ D の多項式で O(M(D) log(k+1))、メモリ O(D)
  linear_recurrence_nth: 位数 D で O(M(D) log(k+1))
  bmbm: O(N^2 + M(D) log(k+1))
pitfalls:
  - 有限 prefix に一致する漸化式だけでは未知の将来の一致は保証できない
  - 復元対象が位数 D 以下の線形漸化式なら先頭 2D 項で十分
related:
  - math/formal_power_series
verification:
  unit:
    - verify/unit/math/linear_recurrence.test.cpp
  stress:
    - verify/stress/math/linear_recurrence.test.cpp
  online:
    - verify/online/math/find_linear_recurrence.test.cpp
    - verify/online/math/kth_term_of_linearly_recurrent_sequence.test.cpp
---

# Linear recurrence and BMBM

漸化式の規約は `s[i] = c[0]*s[i-1] + ... + c[D-1]*s[i-D]`、項の添字は 0 始まりです。
Berlekamp–Massey は有限 prefix と矛盾しない最小の位数 D の係数 c を返します。
Bostan–Mori は有理級数から項を抽出し、BMBM はこれらを組み合わせます。

## アルゴリズムの考え方

### 漸化式の復元

Berlekamp–Massey は数列の prefix を一項ずつ見て、現在の漸化式による予測との差
（discrepancy）を計算します。差が 0 なら式を維持し、非零なら以前の式をずらして
適切な倍数を引くことで、その差を消します。必要な場合は漸化式の次数も増やします。
処理済みの prefix に一致する最短の線形漸化式を維持し、最後にその係数を返します。
例えば `[0,1,1,2,3,5]` から Fibonacci の `s[i]=s[i-1]+s[i-2]` を復元できます。
長さ N の各位置で高々 O(N) 個の係数を扱うため、時間は O(N²) です。

有限の prefix に合うことだけでは、未知の将来の値は保証できません。
元の無限列の位数が D 以下と分かっているなら、十分な長さ（少なくとも 2D）の prefix を
与えることで復元を将来へ使えます。この前提を確認せず bmbm で外挿しないでください。

### 遠い項を求める

既知の漸化式 `s[i]=sum_j c[j]*s[i-1-j]` を母関数 P(x)/Q(x) に変換します。
`Q=1-c[0]x-c[1]x²-...` とし、初期列に Q を掛けた低次部分を P とします。
これにより欲しい s[k] は有理関数の x^k の係数です。

Bostan–Mori は分子・分母へ Q(-x) を掛けます。分母 Q(x)Q(-x) は偶数次数だけなので、
分子の偶数次数または奇数次数を k の偶奇で選び、分母の偶数次数と一緒に添字を半分にします。
同じ係数抽出問題で k を半分にでき、k=0 なら定数項の比で答えが出ます。
多項式の長さを D、畳み込み時間を M(D) とすると O(M(D) log(k+1)) です。
係数の向きは「直前の項から順」であり、行列遷移の係数順との取り違えに注意します。

## API

| API | 結果・前提 |
| --- | --- |
| `cp::berlekamp_massey(const vector<Mint>& prefix)` | 最小位数の係数 c。空列・全零列では空 |
| `cp::bostan_mori(FPS<Mint> p, FPS<Mint> q, uint64_t k)` | p/q の x^k の係数。q[0] 非零、p の次数は q 以上でも可 |
| `cp::linear_recurrence_nth(const vector<Mint>& initial, const vector<Mint>& c, uint64_t k)` | 位数 D の第 k 項。initial と c は同長 D |
| `cp::bmbm(const vector<Mint>& prefix, uint64_t k)` | prefix 内ならその値、以降は復元した漸化式で第 k 項 |

全て uint64_t 全域の添字に対応します。位数 0 の漸化式、空の BMBM prefix は零列と
みなします。Bostan–Mori は Q(-x) を掛けて偶数・奇数次数を抽出し k を半減します。
通常の位数 D の漸化式は Q=1-c[0]x-...-c[D-1]x^D、P=(initial*Q) mod x^D に変換します。

Berlekamp–Massey の比較は体上で行い、非零 discrepancy の逆元が必要です。
Bostan–Mori 以下は FPS の固定奇素数法・NTT 制約を継承します。998244353 では多項式の
長さに対して NTT 中間長の余裕を取り、FPS ドキュメントの範囲を守ってください。

BMBM が未知の項を正しく返すには、無限列が体上の定数係数線形漸化式に従い、その
位数の上界 D が既知で、先頭 2D 項以上を渡すことが十分条件です。位数の上界がない
任意の有限列から将来の値を保証することはできません。返された小さな位数を見て
prefix が十分だと判断することもできません。短い prefix では矛盾しない一つの延長を
選ぶだけです。先頭だけ特殊な列にも注意してください。

```cpp
#include <cp/math/linear_recurrence.hpp>
#include <cassert>
int main() {
    using M = cp::modint998244353;
    std::vector<M> fibonacci{0,1,1,2,3,5,8,13};
    auto c = cp::berlekamp_massey(fibonacci);
    assert(c == std::vector<M>({1,1}));
    assert(cp::linear_recurrence_nth(std::vector<M>{0,1}, c, 10) == M(55));
    assert(cp::bmbm(fibonacci, 10) == M(55));
}
```

単体テストは Fibonacci、空・全零列、有限の一項だけ非零な列、定数分母、uint64 最大
添字を確認します。乱択の漸化式で直接計算と BM/BMBM/Bostan–Mori を照合し、一般の
有理級数も級数除算と比較します。Library Checker の Find Linear Recurrence と
Kth Term of Linearly Recurrent Sequence を全ケース実行します。

背景は [AtCoder Algorithm Lectures: 線形漸化式の復元](https://info.atcoder.jp/entry/algorithm_lectures/linearly_recurrent_sequence_reconstruction)
を参照してください。この実装の復元は古典的な O(N^2) Berlekamp–Massey 法です。
