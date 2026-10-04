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
`contests/YYYY/<id>/`、問題・サンプル・ノートを作ります。AtCoder の情報取得は
oj-ng の `oj-api` / `oj` アダプターで行います。ログインが必要な場合は事前に
`uv run oj login https://atcoder.jp/` を実行してください。

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
