---
title: Contest standard-library template
source: include/cp/utility/template.hpp
status: stable
tags:
  - utility
aliases:
  - 競プロテンプレート
  - 標準ヘッダー一括インクルード
requires:
  - C++23
complexity:
  runtime: 追加の実行時処理なし
pitfalls:
  - bits/stdc++.h の全ヘッダーを再現するものではない
  - using と型の別名は main.cpp に直接記述する
verification:
  unit:
    - verify/unit/utility/template.test.cpp
---

# Contest standard-library template

`#include <cp/utility/template.hpp>` の1行で、競技プログラミングでよく使う
標準ヘッダーを読み込みます。コンテナ、アルゴリズム、入出力、数値計算、乱数、
bit 操作、ranges などを含みます。ヘッダーには `using` やマクロを定義しません。
用途に応じて、このヘッダーに含まれない標準ヘッダーは個別に追加してください。

`kpro contest start` は設定されたコンパイラとフラグで `<bits/stdc++.h>` の有無を
確認し、利用できる場合は生成する `main.cpp` でそちらを直接インクルードします。
利用できない場合はこのヘッダーをインクルードします。どちらの場合も、
`using namespace std;` と `ll`、`ull`、`ld`、`pii`、`pll`、`vi`、`vll`、`vvi`、
`vvll` の別名は `main.cpp` に直接記述され、そこで確認・編集できます。

## Example

```cpp
#include <cp/utility/template.hpp>

using namespace std;
using ll = long long;
using vll = vector<ll>;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    vll values{3, 1, 2};
    ranges::sort(values);
    cout << accumulate(values.begin(), values.end(), 0LL) << '\n';
}
```

## Verification

単体テストでは追加の標準インクルードなしで、コンテナ、アルゴリズム、数値計算、
bit 操作、乱数、文字列入出力を組み合わせて利用できることを確認します。
CLI のテストでは両方のテンプレート生成と、基準コミットのヘッダーから展開した
提出用ソースのコンパイル・実行を確認します。
