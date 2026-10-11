# Carpenter's Cat C++ Library

C++23 の競プロライブラリ、検証、検索できるドキュメント、コンテスト作業環境を
一つの CLI `kpro` から扱います。macOS / Linux、Python 3.11 以上と C++ コンパイラが必要です。

## セットアップ

```sh
uv sync --locked
git config core.hooksPath .githooks
chmod +x .githooks/pre-push
uv run kpro doctor
```

`uv.lock` に依存バージョンを固定しています。以降は `uv run kpro ...` または
`.venv` を activate して `kpro ...` を使ってください。
macOS の `g++` は Apple Clang でも動作します。コンパイラは `kpro.toml` で変更できます。

## ライブラリを追加・検証する

```sh
uv run kpro lib new data_structure/example
# ヘッダー、Markdown の TODO、単体テストを実装
uv run kpro lib check data_structure/example
uv run kpro verify data_structure/union_find
uv run kpro verify
```

`lib new` は既存ファイルを上書きしません。テンプレートのテストは実装前に失敗します。
`status` は `experimental` / `stable` / `deprecated`。`stable` には実在する検証テスト、
タグ、別名が必要です。検証はメタデータ、単独ヘッダーコンパイル、単体・乱択・
オンラインテスト、ドキュメントの検証を行います。

Union-Find は `cp::UnionFind`。単体・固定 seed の乱択テストに加えて、
competitive-verifier で Library Checker の Unionfind 問題を検証します。
オンライン検証の初回はネットワークが必要です。未実行のオンライン検証を成功扱いにはしません。
結果は `.kpro/verify/data_structure/union_find.json` に保存されます。

```cpp
#include <cp/data_structure/union_find.hpp>

cp::UnionFind uf(10);
uf.merge(0, 1);
bool connected = uf.same(0, 1);
```

## ドキュメントと検索

```sh
uv run kpro docs                 # http://127.0.0.1:8000/
uv run kpro docs build           # .kpro/docs/site/
uv run kpro search "ユニオンファインド"
uv run kpro search "graph/connectivity"
```

Zensical によるローカル全文検索、階層タグページ、コードコピーを利用できます。
ドキュメント、別名、前提、計算量、注意点、コンテストノートを検索対象とします。
新しいタグは `docs/tags.toml` の `tags` 配列に追加します。
実装は `include/cp/` に一度だけ保存し、ページ内の完全なソースは自動生成します。
生成物・キャッシュは `.kpro/` と `build/` にあり Git では無視されます。

## コンテスト

ライブラリとツールをレビューして Git にコミットし、`main` の作業ツリーを clean にしてから開始します。

```sh
uv run kpro contest start https://atcoder.jp/contests/abc999
uv run kpro contest status
uv run kpro test a
uv run kpro run a
uv run kpro bundle a
uv run kpro submit a --language <AtCoder-language-id>
uv run kpro contest end
```

開始時に `HEAD` を `base_commit` として記録し、snapshot タグと contest ブランチ、
`contests/YYYY/<id>/`、問題・サンプル・ノートを作ります。
AtCoder・Codeforces・yukicoder の URL に対応しています。AtCoder は `abc350` のような
ID 単独でも指定でき、他サイトは `codeforces:1234` / `yukicoder:1` と明示します。
数字だけの指定はサイトを判別できないためエラーになります。

生成される `main.cpp` は `<bits/stdc++.h>` を優先します。設定されたコンパイラで
利用できない場合は、標準ヘッダーをまとめた `<cp/utility/template.hpp>` を使います。
`using namespace std;` と `ll`、`ull`、`ld`、`pii`、`pll`、`vi`、`vll`、`vvi`、`vvll`
の別名は、確認・編集しやすいよう `main.cpp` に直接置きます。
このヘッダーも基準コミットから提出用ソースへ展開されます。

| サイト | 問題一覧・サンプル取得 | 自動提出 | 内部 ID の例 |
| --- | --- | --- | --- |
| AtCoder | 対応 | 対応 | `abc350` |
| Codeforces | 通常の公開コンテスト・順位表が公開されているもの | 未対応（ブラウザから提出） | `codeforces-1234` |
| yukicoder | 対応 | `oj` のログインが必要 | `yukicoder-1` |

```sh
uv run kpro contest start https://codeforces.com/contest/1234
# または: uv run kpro contest start codeforces:1234
# yukicoder: uv run kpro contest start https://yukicoder.me/contests/1
uv run kpro test b1
uv run kpro bundle b1
# 表示された build/submit.cpp をブラウザから提出
uv run kpro contest end
```

Codeforces の `B1` / `B2` は `b1` / `b2` と別々に保存します。
他サイトの数値 ID はサイト名を付けて保存先・ブランチ・snapshot タグの衝突を防ぎます。
Codeforces の Gym・group・非公開コンテストには対応していません。
問題一覧は [Codeforces の公式 API](https://codeforces.com/apiHelp/methods#contest.standings)、
AtCoder・yukicoder の問題一覧と全サイトのサンプル取得は `oj-api` / `oj` を使います。
Codeforces の `submit` は `--force` を指定しても自動提出せず、bundle の生成方法を案内します。

標準入出力の通常問題を対象とし、比較はトークン一致です。対話型・専用チェッカー・
誤差判定が必要な問題の正しさは、このサンプルテストでは確認できません。
サンプルが取得できない、または空の場合をテスト成功にはしません。
取得コマンドが失敗した場合は開始を中断し、作成途中のブランチ・タグ・作業ディレクトリを
取り消します。特殊形式の問題を含むコンテストは、全問題のサンプルを取得できず開始できない
場合があります。

Git 管理するのは自分の解答（`.cpp` / `.hpp` / `.py`）、手書きノート（`.md`）、
再現・提出に必要な `contest.toml` / `problem.toml` と `.gitignore` です。
問題設定には ID と URL のみを保存し、取得した問題タイトルはコピーしません。
サンプルの `tests/` / `test/`、ビルド結果、その他の形式は Git から除外します。
問題文などの取得データは、Markdown でも各作業ディレクトリの `.local/` に置いてください。
コンテスト開始時にも除外設定を生成するため、`project.contest_root` を変更しても有効です。
push hook は送信対象のコミット履歴を調べ、除外対象のコンテストファイルが含まれれば
拒否します。`git add -f` で追加後、次のコミットで消した場合も拒否します。
終了後に解答やノートをコミット・push する際は内容を確認してください。

`contest.toml` がライブラリの基準コミットです。`run` / `test` / `bundle` / `submit` は
現在の解答と基準コミットのライブラリから同じ提出用ソースを生成します。
開始後にローカルのヘッダーを編集しても、その編集は提出へ混入しません。
依存循環・基準コミットにないヘッダーはエラーになります。
コンテスト外では、明示的なソースパスで working tree のヘッダーを使います。

`submit` は生成した `build/submit.cpp` をコンパイルし、同じ実行ファイルで
保存サンプルをすべてテストしてから提出します。空のサンプルも失敗扱いです。
`--force` はサンプルの失敗を明示的に迂回しますが、コンパイル失敗は迂回しません。
比較は空白区切りのトークン一致、実行制限は `verify.timeout` 秒です。

コンテスト中は `.git/kpro/contest-lock.toml` と pre-push hook で push を拒否します。
緊急解除は `KPRO_ALLOW_PUSH=1 git push`。通常は `contest end` で解除します。
終了してもブランチ・ノート・作業ファイルは残り、自動マージしません。
ノートの YAML front matter に `contest` / `problem` / `result` / `tags` を記録できます。

### AtCoder へのログイン

AtCoder のログインには Cloudflare Turnstile があり、ユーザー名・パスワードを
HTTP で送る CUI 方式は CAPTCHA を処理できません。固定環境の `oj-ng` は Selenium
未導入のため、通常の `oj login` は CUI 方式に切り替わります。
Selenium を導入しても、自動操作ブラウザが Turnstile に拒否される
[既知の報告](https://github.com/online-judge-tools/oj/issues/934) があります。

普段使うブラウザでログインし、セッションを読み込む方法を推奨します。

1. ブラウザで <https://atcoder.jp/login> にログインする。
2. 開発者ツールの Application / Storage → Cookies → `https://atcoder.jp` で
   `REVEL_SESSION` の **Value** をコピーする（HttpOnly のため `document.cookie` では取得できません）。
3. 次のコマンドをターミナルで実行し、非表示の入力欄へ値を貼り付ける。

```sh
uv run kpro login
uv run kpro login --check
```

Cookie はアカウントの認証情報です。チャット、コマンド引数、コミットには貼り付けないでください。
ログイン確認が成功した場合だけ `.kpro/auth/atcoder.cookie.jar`（権限 `0600`）に保存し、
`kpro` の情報取得・サンプル取得・提出で共通に使います。失敗時は既存のセッションを保持します。
期限切れならブラウザで再ログインし、`kpro login` を再実行してください。
読み込み前は従来の `oj` の標準 Cookie を引き続き利用します。
このリポジトリ内の Cookie は AtCoder 専用です。Codeforces・yukicoder では
`oj` の標準 Cookie を使い、Codeforces の公式 API には Cookie を送りません。

GUI 自動操作を試す場合は、固定バージョンに Selenium を追加して実行できます。

```sh
uv run --with 'online-judge-tools-ng[selenium]==12.0.1' oj login --use-browser always https://atcoder.jp/
uv run oj login --check https://atcoder.jp/
```

対応ブラウザと WebDriver が必要です。通常のブラウザだけで提出する場合は
`uv run kpro bundle a` で生成した `build/submit.cpp` を提出ページに貼り付けられます。
提出前に `uv run kpro test a` で同じスナップショットのソースを検証してください。

### yukicoder へのログイン・提出

`kpro login` は AtCoder 専用です。yukicoder は `oj` でセッションを作成・確認してください。
ブラウザを使う場合は Selenium と対応する WebDriver が必要です。

```sh
uv run --with 'online-judge-tools-ng[selenium]==12.0.1' oj login --use-browser always https://yukicoder.me/
uv run oj login --check https://yukicoder.me/
uv run kpro contest start yukicoder:1
uv run kpro submit a --language <yukicoder-language-id>
uv run kpro contest end
```

言語 ID はサイト固有です。必要なら対象問題について
`uv run oj-api get-problem --full <problem-url>` の `availableLanguages` を確認してください。
実際の提出には、bundle したソースのコンパイルと保存サンプルのテスト成功が必要です。

## 開発と仕様

```sh
uv run pytest
uv run ruff check tools tests
uv run ruff format --check tools tests
```

一時 Git リポジトリと fake JudgeBackend を使い、開始・スナップショット・
push 防止・提出ゲートをテストします。実際のアカウントへの提出は実行しません。
設計判断は [implementation notes](docs/implementation-notes.md)、要件は
[仕様書](competitive-programming-library-spec.md) に記録しています。
