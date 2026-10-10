# Carpenter's Cat C++ Library

競技プログラミングのための C++23 ライブラリと、解法・失敗・学びのノートです。

各ページに、用途、アルゴリズムの考え方と正しさ、計算量の根拠、API の前提と注意点、
C++ の使用例をまとめています。まず解きたい問題から選び、使用例と API の契約を確認してください。

## データ構造

| ライブラリ | 用途 |
| --- | --- |
| [Coordinate Compression](library/data_structure/coordinate_compression.md) | 大小関係を保って値を配列の順位へ変換 |
| [Fenwick Tree](library/data_structure/fenwick_tree.md) | 一点加算と区間和、頻度表、転倒数 |
| [Segment Tree](library/data_structure/segment_tree.md) | 一点更新と結合的な区間積、境界探索 |
| [Lazy Segment Tree](library/data_structure/lazy_segment_tree.md) | 区間作用と区間積、遅延伝播 |
| [Persistent Segment Tree](library/data_structure/persistent_segment_tree.md) | 古い版を保持する一点更新と区間積 |
| [Sparse Table](library/data_structure/sparse_table.md) | 静的な区間最小値など、冪等な集約 |
| [Wavelet Matrix](library/data_structure/wavelet_matrix.md) | 静的な区間の順序統計・値域の個数 |
| [SWAG](library/data_structure/swag.md) | FIFO キューの積、スライド区間 |
| [Li Chao Tree](library/data_structure/li_chao_tree.md) | 登録座標上の直線・線分の最小値または最大値 |
| [Union-Find](library/data_structure/union_find.md) | 辺追加による連結成分、素集合の併合 |
| [Potential Union-Find](library/data_structure/potential_union_find.md) | 頂点間の差の制約と矛盾判定 |
| [Rollback Union-Find](library/data_structure/rollback_union_find.md) | 履歴を取り消す連結成分管理 |

区間和なら Fenwick Tree、一般の一点更新なら Segment Tree、区間更新なら Lazy Segment Tree が
候補です。更新しない最小値は Sparse Table、区間の k 番目の値は Wavelet Matrix を使います。

## グラフ

| ライブラリ | 用途 |
| --- | --- |
| [Weighted Graph](library/graph/weighted_graph.md) | 辺 ID、最短路の状態と経路復元、アルゴリズム選択 |
| [Dijkstra](library/graph/dijkstra.md) | 非負重みの単一始点最短路 |
| [0-1 BFS](library/graph/zero_one_bfs.md) | 重み 0/1 の線形時間の最短路 |
| [Bellman–Ford](library/graph/bellman_ford.md) | 負辺を含む最短路と負閉路の影響 |
| [Floyd–Warshall](library/graph/floyd_warshall.md) | 全頂点対の最短路 |
| [Kruskal](library/graph/kruskal.md) | 無向グラフの最小全域森 |
| [Lowlink](library/graph/lowlink.md) | 無向グラフの橋・関節点 |
| [Topological Sort](library/graph/topological_sort.md) | 有向非巡回グラフの順序と閉路検出 |
| [SCC](library/graph/scc.md) | 有向グラフの強連結成分と縮約 DAG |
| [TwoSat](library/graph/two_sat.md) | 2 リテラルの論理制約の充足判定 |
| [Max Flow](library/graph/max_flow.md) | 最大流・最小カット・二部マッチング |
| [Min Cost Flow](library/graph/min_cost_flow.md) | 容量制約下の最小費用と費用関数の折れ点 |

## 木

| ライブラリ | 用途 |
| --- | --- |
| [Tree / RootedTree](library/tree/tree.md) | 木の表現・妥当性検証・親・深さ・部分木サイズ |
| [Euler Tour](library/tree/euler_tour.md) | 部分木を一次元の半開区間へ変換 |
| [LCA](library/tree/lca.md) | 二分ジャンプ、最小共通祖先、辺数の距離 |
| [Heavy-Light Decomposition](library/tree/heavy_light_decomposition.md) | 経路を少数の区間へ分解、向きのある集約 |
| [Rerooting DP](library/tree/rerooting.md) | 全ての根について木 DP を共有して計算 |

## 数学

| ライブラリ | 用途 |
| --- | --- |
| [Modint](library/math/modint.md) | 剰余の四則演算・累乗・逆元 |
| [Math](library/math/math.md) | 二分累乗、拡張 Euclid、CRT、floor sum |
| [Combinatorics](library/math/combinatorics.md) | 階乗・逆階乗による組合せと順列 |
| [Primality](library/math/primality.md) | 64 bit 整数の Miller–Rabin 素数判定 |
| [Factorization](library/math/factorization.md) | Pollard's rho による 64 bit 素因数分解 |
| [Sieve](library/math/sieve.md) | 線形篩の最小素因数表と区間篩の素数列挙 |
| [Matrix](library/math/matrix.md) | 行列累乗、掃き出し、rank、行列式、逆行列、連立方程式 |
| [Convolution](library/math/convolution.md) | NTT による多項式積と整数畳み込み |
| [Formal Power Series](library/math/formal_power_series.md) | Newton 法、級数演算、多項式除算、多点評価と補間 |
| [Linear Recurrence](library/math/linear_recurrence.md) | 漸化式復元、母関数から遠い項の抽出 |
| [Subset Transform](library/math/subset_transform.md) | 部分集合・上位集合の zeta / Möbius 変換 |
| [Bitwise Convolution](library/math/bitwise_convolution.md) | AND / OR / XOR 畳み込み |

## 文字列

| ライブラリ | 用途 |
| --- | --- |
| [Trie](library/string/trie.md) | 共通接頭辞の共有、単語・接頭辞の個数 |
| [Aho–Corasick](library/string/aho_corasick.md) | 複数パターンの一括検索と出現数 |
| [Rolling Hash](library/string/rolling_hash.md) | 部分列のハッシュ比較・連結・LCP |
| [Manacher](library/string/manacher.md) | 全中心の最大回文半径と区間の回文判定 |
| [String Algorithms](library/string/string_algorithms.md) | Suffix Array、Kasai の LCP、Z Algorithm |

## 幾何・入出力・共通入口

| ライブラリ | 用途 |
| --- | --- |
| [Integer Geometry](library/geometry/integer_geometry.md) | 正確な内積・外積、向き、線分交差、凸包 |
| [Fast IO](library/io/fast_io.md) | バッファ付きの整数・文字列入出力 |
| [ACL-equivalent Library](library/utility/acl.md) | ACL 相当 API の対応表と組合せの入口 |

## 検索と実装

`kpro search "連結成分"` でライブラリとノートを検索できます。
Tags ページでは階層タグから関連する項目を探せます。

実装の正本は `include/cp/` にあり、各ページのソースコードは自動生成されます。
検証状態はテスト結果から算出され、ソース変更後は再検証が必要と表示されます。
