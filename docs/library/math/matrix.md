---
title: Matrix and exact linear algebra
source: include/cp/math/matrix.hpp
status: stable
tags:
  - math
aliases:
  - Matrix
  - Gaussian elimination
  - Gauss-Jordan
  - 線形方程式
  - 行列
  - 行列式
  - 掃き出し法
requires:
  - 行列積・累乗は単位的半環。各演算が型 T で正しく表現できる
  - 階数・行列式・逆行列・方程式は非零で除算できる厳密な体
  - modint の法は素数、動的法は演算中変更しない
complexity:
  multiply: m*k と k*n の積に O(mkn)、メモリ O(mn)
  pow: n*n の行列で O(n^3 log exponent)
  gauss_jordan/rank: m*n の行列で O(mn min(m,n))
  solve: O(m(n+1)(min(m,n)+1) + n(n-rank) + n)、基底の出力を含む
  determinant/inverse: O(n^3)
  memory: 掃き出しと解に O(mn+n^2)
pitfalls:
  - 整数型の除算や合成数法は体ではない
  - 浮動小数点の誤差許容・安定な数値解法は扱わない
related:
  - math/modint
verification:
  unit:
    - verify/unit/math/matrix.test.cpp
  stress:
    - verify/stress/math/matrix.test.cpp
  online:
    - verify/online/math/matrix_product.test.cpp
    - verify/online/math/pow_of_matrix.test.cpp
    - verify/online/math/matrix_rank.test.cpp
    - verify/online/math/matrix_det.test.cpp
    - verify/online/math/inverse_matrix.test.cpp
    - verify/online/math/system_of_linear_equations.test.cpp
---

# Matrix and exact linear algebra

行優先の連続配列を持つ行列です。構築時の行数と列数を保持するので 0×n や m×0 も
表現できます。各操作は入力を変更せず、結果を値で返します。`operator*=` は例外です。

## アルゴリズムの考え方

### 行列積と累乗

m×k と k×n の積は各出力に k 個の積を足し、O(mkn) で求めます。
実装は i,k,j の順で走査し、左の要素が 0 ならその行への寄与を飛ばします。
正方行列の累乗は二分累乗を使い、単位行列を初期値として O(log(exponent+1)) 回の
乗算で求めます。d×d なら O(d³ log(exponent+1)) です。
例えば Fibonacci 数列の状態 `(F[t+1],F[t])` は行列 `[[1,1],[1,0]]` による遷移で、
行列累乗により遠い時刻へまとめて進めます。向きは列ベクトルとして解釈します。

### 掃き出し法と連立方程式

Gauss–Jordan 法は非零の pivot を探し、その行を pivot で割り、他の行の同じ列を消します。
行交換、非零倍、他の行の倍を加える操作は解集合を変えません。
各 pivot 列に一つだけ 1 がある形へ変換すると、pivot 数が rank です。
右辺列は行操作には含めますが、pivot の候補に含めません。

係数行が全て 0 なのに右辺が非零なら解なしです。そうでなければ自由変数を 0 として
一つの解を作り、各自由変数を単独で 1 にした同次方程式の基底も返します。
例えば x+y=3 の全解は `(3,0)+t*(-1,1)` で、特解と基底で表せます。
逆行列は `[A|I]` を `[I|A^-1]` へ変換し、rank が足りない場合は存在しません。
行列式は下側だけを消し、pivot の積と行交換の符号から求めます。

消去は O(mn min(m,n))、正方行列の逆行列と行列式は O(n³) です。
乗算は半環で使えますが、消去は非零で割れる正確な体を要求します。
整数の切り捨て除算や誤差を含む浮動小数点を、そのままこの消去へ渡さないでください。

## API

| API | 動作 |
| --- | --- |
| `cp::Matrix<T> a(int rows, int columns, T value = T(0))` | 指定サイズを同じ値で構築。既定構築は 0×0 |
| `Matrix<T>(const vector<vector<T>>& rows)` | 長方形の配列から構築。空の配列は 0×0 |
| `a.rows()`, `a.columns()` | 行数と列数 |
| `a(r,c)` | 0 始まりの要素アクセス。const と書き換え用の両方 |
| `a.data()` | 行優先の const vector<T>& |
| `Matrix<T>::identity(int n)` | n×n の単位行列 |
| `a*b`, `a*=b` | a.columns()=b.rows() を満たす行列積 |
| `a.pow(uint64_t e)` | 正方行列の非負累乗。指数 0 は単位行列 |
| `a==b` | 次元と全要素を比較 |
| `cp::gauss_jordan(a, int coefficient_columns = -1)` | (RREF 行列, 昇順の pivot 列)。-1 は全列を pivot の対象にする |
| `cp::matrix_rank(a)` | 体上の階数 |
| `cp::determinant(a)` | 正方行列の行列式。特異なら 0、0×0 は 1 |
| `cp::inverse(a)` | optional<Matrix<T>>。特異なら nullopt、0×0 は空の単位行列 |
| `cp::solve_linear_system(a, const vector<T>& b)` | optional<LinearSystemSolution<T>>。a.rows()=b.size()、解なしなら nullopt |

`gauss_jordan` は最初の coefficient_columns 列から非零の pivot を選び、pivot を 1 に
正規化して上下の行から消去します。残りの列にも同じ行操作を適用するので拡大行列を
扱えます。行交換が必要な場合も処理します。pivot 数が階数です。

`LinearSystemSolution<T>` のフィールドは `particular` (一つの解)、`basis` (零空間の基底)、
`pivot_columns` と `free_columns` (各変数の列番号) です。全解は particular に各 basis の
任意のスカラー倍を加えたものです。particular の自由変数は 0、各基底は一つの自由変数を
1 にします。自由変数列は昇順、基底の数は n-rank です。方程式が 0 本なら全変数が自由、
変数が 0 個なら右辺が全て 0 の場合だけ解があります。

方程式を解く時間には拡大行列の構築・走査と掃き出しに加え、
長さ n の基底を n-rank 本返す O(n(n-rank)) を含めます。
行数 m=0 の場合も基底は n×n の単位行列に相当するため時間・出力領域は O(n²) です。

## Type and dimension contracts

積と累乗は T(0)、T(1)、等値判定、加算・乗算を持つ単位的半環で使えます。
T の既定構築値が T(0) である必要はありません。
整数から T への暗黙変換も不要で、T(0) と T(1) を明示構築できれば使えます。
普通の整数型では中間の積と和を含めて型に収まることを保証してください。
消去・階数・行列式・逆行列・方程式には非零での除算が必要です。推奨は素数法の
`cp::static_modint` または `cp::dynamic_modint` で、後者は法を固定して使います。
素数 2 の体も対応します。法の素数性は行列側で検査しません。

整数の切り捨て除算や合成数法は使えません。数値的な pivot 選択や許容誤差を使わないため、
浮動小数点の安定な数値線形代数は対象外です。厳密な有理数型等も型が契約を満たせば
使えますが、その場合のビット計算量は上記の体演算回数とは別です。

次元は非負の int、アクセス・積の次元・正方行列の条件は assert で確認します。
逆行列では列数 2n、方程式では列数 n+1 が int に収まる必要があり assert します。
行数×列数の要素をメモリへ確保できる必要があります。NDEBUG 時は呼び出し側で守ります。

```cpp
#include <cp/math/matrix.hpp>
#include <cp/math/modint.hpp>
#include <cassert>
int main() {
    using M = cp::modint998244353;
    cp::Matrix<M> a(std::vector<std::vector<M>>{{0,1},{1,0}});
    assert(cp::determinant(a) == M(-1));
    assert(a.pow(2) == cp::Matrix<M>::identity(2));
    auto solution = cp::solve_linear_system(a, std::vector<M>{2,3});
    assert(solution && solution->particular == std::vector<M>({3,2}));
}
```

単体テストは空の次元、行交換、特異、解なし、複数解、自由変数、素数 2 を確認します。
乱択で愚直積、反復累乗、置換の全列挙による行列式と比較します。小さい F_5 上では
全変数の列挙で解の個数を確認し、行ベクトルの span の全列挙で階数を比較します。
この F_5 型は既定構築値を 1 にして、解と基底の零初期化も検証します。
整数からの構築を explicit にし、行列式・掃き出し・逆行列・方程式で
暗黙変換を要求しないことも検証します。
解と基底は元の方程式へ代入します。Library Checker の行列積、累乗、階数、行列式、
逆行列、線形方程式の全ケースをローカルで実行します。
