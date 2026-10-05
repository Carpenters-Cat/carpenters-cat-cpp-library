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
