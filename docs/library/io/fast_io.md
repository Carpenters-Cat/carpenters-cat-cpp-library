---
title: Fast IO
source: include/cp/io/fast_io.hpp
status: stable
tags:
  - optimization
aliases:
  - fastIO
  - 高速入出力
  - buffered input output
requires:
  - コンストラクタに渡す FILE* は非 null で、オブジェクトの寿命中は開いている
  - 整数は十進表記、区切りは ASCII 空白文字
complexity:
  read/write: 読み書きする文字数に対して O(n)
  memory: 各オブジェクト O(BufferSize)、文字列の読み込みは追加で O(文字列長)
pitfalls:
  - bool、浮動小数点、128 bit 整数の変換には対応しない
  - 同じ FILE* を別の入出力方式や複数オブジェクトと同時に使わない
  - 対話型問題の入力には FastInput<1> を使い、相手の入力を待つ前に出力を flush() する
  - ファイルの所有権は呼び出し側にあり、オブジェクトより先に fclose しない
related: []
verification:
  unit:
    - verify/unit/io/fast_io.test.cpp
  stress:
    - verify/stress/io/fast_io.test.cpp
  online:
    - verify/online/io/fast_io.test.cpp
---

# Fast IO

`fread`・`fwrite` によるバッファ付き入出力です。標準入力・標準出力を既定とし、
任意の `FILE*` にも使用できます。コピー・ムーブはできません。
入力と出力のバッファサイズはそれぞれテンプレート引数で指定し、既定値は 65536 byte です。
バッファサイズは正でなければなりません。

## アルゴリズムの考え方

入力は fread でまとまったバイト列を読み、バッファ内の位置を進めながらトークンを解析します。
出力は文字をバッファへ追加し、満杯時または flush 時に fwrite でまとめて書きます。
文字ごとの形式処理や呼び出しを減らすことで、大量の整数の入出力に向けた処理になります。
バッファ境界で補充するので、符号や数値が境界をまたいでも同じトークンとして読めます。

整数の解析は絶対値を符号なし型へ `magnitude=10*magnitude+digit` で蓄積します。
計算前に `magnitude>limit/10` または同値で `digit>limit%10` を確認すると、
オーバーフローを発生させずに範囲外と判断できます。
符号付き最小値は正の同型に収まらないため、負数用の上限と変換手順を別にします。
例えば int64 の最小値でも、同型の符号反転を行わずに扱えます。

不正なトークンは最後まで消費し、解析が成功した場合だけ出力引数へ代入します。
そのため失敗時には元の値が残り、次の読み込みでは次のトークンへ進めます。
EOF とエラーの区別、戻り値の確認は利用側で行います。
全処理は読み書きするバイト数に線形で、固定バッファの領域は O(BufferSize) です。

改行は出力を自動 flush しません。対話型では相手の応答を待つ前に明示的に flush し、
入力には FastInput<1> を使います。入力の先読みと出力の保留があるため、同じ FILE* で
cin/scanf や cout/printf を混ぜないでください。数値の計算自体の範囲は利用側の責任です。

## API

| API | 動作 |
| --- | --- |
| `cp::FastInput<BufferSize> in(stream = stdin)` | 入力ストリームを借用 |
| `in.read(T& x)` | 符号付き・符号なし標準整数型の十進整数を読む。成功時 true |
| `in.read(char& x)` | ASCII 空白を読み飛ばして1文字を読む |
| `in.read(std::string& s)` | 空白区切りの文字列を読む |
| `in.eof()` | 次の byte が EOF かを調べる。読み取りエラーは false |
| `in.error()` | C ストリームで読み取りエラーが発生したか |
| `cp::FastOutput<BufferSize> out(stream = stdout)` | 出力ストリームを借用 |
| `out.write(T x)` | 整数を十進表記で出力。区切りは追加しない |
| `out.write(char c)` / `out.write(std::string_view s)` | 文字 / 文字列をそのまま出力 |
| `out.flush()` | 独自バッファを出力し、`fflush` する。成功時 true |
| `out.error()` | バッファの書き出しまたは C ストリームのエラーがあるか |

整数型は `signed char` から `long long` までとそれらの符号なし型です。
`char` は常に文字として読み書きします。符号は `+` と `-` を扱い、
負の数を符号なし型に読み込む場合は失敗します。型の最小値・最大値を扱えます。
空白は space、tab、LF、CR、vertical tab、form feed です。ロケールには依存しません。

入力の EOF、読み取りエラー、不正な整数、範囲外の整数では `read` は false を返します。
失敗時には引数の値を変更しません。不正な整数はそのトークン全体を消費して次へ進みます。
最終トークンの後に改行や空白がなくても読み込めます。
`eof()` は空白を読み飛ばさないため、末尾に空白がある場合には `read` が
空白を消費するまで false です。通常は `while (in.read(x))` で読み込みます。
出力 API は書き出しエラー時に false を返し、以後の書き込みも失敗します。

出力バッファが満杯になると `fwrite` します。改行は自動 flush を行いません。
デストラクタも `flush()` を呼びますが、エラーを確かめる必要がある場合は
明示的に `flush()` の返り値を確認します。入力側は先読みしているため、
同じストリームの `scanf`・`cin` 等と混ぜると順序が壊れます。
出力側も `printf`・`cout` 等との同時使用を避けてください。

対話型問題では `cp::FastInput<1>` を使ってください。既定サイズの入力バッファは
`fread` が 65536 byte または EOF を待つことがあり、相手が短い応答を送って次の出力を
待っていると読み込みが止まります。整数・文字列の応答は空白や改行で区切り、
相手からの入力を待つ前には必ず `out.flush()` を呼んでください。

## Examples

```cpp
#include <cp/io/fast_io.hpp>

int main() {
    cp::FastInput<> in;
    cp::FastOutput<> out;
    long long a, b;
    if (!in.read(a) || !in.read(b)) return 1;
    if (!out.write(a + b) || !out.write('\n')) return 1;
    return out.flush() ? 0 : 1;
}
```

加算が整数型の範囲内に収まることは呼び出し側で保証してください。

## Verification

単体テストは空入力、全種類の空白、末尾改行なし、整数の型ごとの最小値・最大値、
不正・範囲外トークン、文字列、明示 flush とデストラクタでの flush を確認します。
1～4 byte のバッファを使い、符号や数値の途中を含む境界をまたぎます。
固定 seed の比較テストでは10万個の整数を `std::to_chars` と照合し、
出力 byte 列の一致と読み戻しを確認します。
Library Checker の Many A + B で大量入力の読み書きを検証します。

## Performance

手動測定用の `verify/stress/io/fast_io_benchmark.cpp` は、100万個の整数
`48271 * i - 25000000000` を一時ファイルに用意し、読み込みと出力をそれぞれ3回測定します。
生成・出力検証は計測から除外し、出力計測には flush を含めます。
標準 C 入出力の `fscanf`・`fprintf` と比較し、読み込みの合計値と出力全文を照合します。
絶対時間は OS のキャッシュ・ストレージ・CPU に依存します。

```sh
g++ -std=c++23 -O2 -Wall -Wextra -Wpedantic -Iinclude \
  verify/stress/io/fast_io_benchmark.cpp -o /tmp/kpro-fast-io-benchmark
/tmp/kpro-fast-io-benchmark
```

assert 内で I/O を呼ぶ測定プログラムなので `-DNDEBUG` を付けないでください。

2026-10-04 の測定：macOS / arm64、Apple clang 21.0.0
(`clang-2100.1.1.101`)、上記 C++23 / `-O2` オプション。
入力は 12057546 byte、バッファは 65536 byte、3回の平均は次のとおりでした。

| 処理 | 標準 C 入出力 | このライブラリ |
| --- | --- | --- |
| 100万整数の入力 | `fscanf`: 0.129147 秒 | `FastInput`: 0.045580 秒 |
| 100万整数の出力 | `fprintf`: 0.105281 秒 | `FastOutput`: 0.023013 秒 |

同じ入力で合計値、同じ出力で全文の一致を確認しています。速度比は環境ごとに変わります。
