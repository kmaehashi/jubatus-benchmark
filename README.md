# Jubatus ベンチマークツール


## 概要

Jubatus ヘ対してリクエストを発生させ、レイテンシー・スループットなどの統計を取得することができます。統計結果は JSON形式 で出力します。

リクエストに利用するデータファイル・リクエストの種類・リクエスト数・同時実行数・実行時間などの条件を設定することができます。

## ビルド手順

```
$ ./waf configure
$ ./waf build
```

### 必要とするもの

* g++
* pkg-config
* pficommon
* jubatus-client

## 実行手順

### コマンドライン

```
$ jubatus-bench-classifier [OPTIONS] [SPECIFIC OPTIONS]
```

### オプション

* `--host` *HOST:PORT*

  * リクエスト先のRPCサーバーを指定する
  * デフォルト: "localhost:9199"

* `--name` *CLUSTER_NAME*

  * クラスタ名を指定する
  * デフォルト: "test"

* `--timeout` *SEC*

  * クライアントのリクエストタイムアウト時間 (秒)
  * デフォルト: 1000

* `--dataset` *PATH*

  * データセットファイルのパス
  * SVM形式のみに対応
  * 必須

* `--dataset-limit` *SIZE*

  * データセットファイルから読み込み件数の上限値
  * データセットファイルの先頭から指定された *SIZE* までの行数を読み込み、利用する
  * デフォルト: 制限なし

* `--in` *PATH*

  * `--dataset` オプションの別名

* `--query-mode` *MODE*

  * 実行するクエリの種類
  * "update" または "train" か "analyze" または "classify" 
  * デフォルト: "update"

* `--bulk-size` *NUM*

  * バルクサイズ
  * バルクサイズ > データセットファイルのサイズ の場合は、バルクサイズになるまでデータセットの先頭からセットする
  * デフォルト: 1

* `--thread-num` *NUM*

  * クライアントスレッド数
  * デフォルト: 1

* `--query-num` *NUM*

  * スレッドあたりのクエリ実行数
  * *NUM* が 0 の場合 かつ `--run-time` オプションが 0 の場合は、読み込んだデータセットファイルをすべて処理する
  * *NUM* が 0 の場合 かつ `--run-time` オプションが 0 ではない場合は、`--run-time` オプションに設定した時間になるまで処理を行う
  * *NUM* が 0 ではない場合　かつ `--run-time` オプションが 0 ではない場合は、`--run-time` オプションに設定した時間になるか *NUM* を処理するまで処理を行う
  * デフォルト: 0

* `--dump-raw-data`

  * 出力結果に、全クエリのレイテンシーを含める
  * デフォルト: 出力なし

* `--no-dump-summary`

  * 出力結果に、サマライズした統計情報を含めない
  * デフォルト: 出力しない

* `--trim-percent` *N*

  * サマライズしたレイテンシーの統計情報に含めないサンプル数
  * 上下あわせて *N* % をレイテンシーの平均/分散に含めない
  * デフォルト: 5

* `--dump-path` *PATH*

  * 測定結果出力ファイル
  * デフォルト: result.json

* `--out` *PATH*

  * `--dump-path` オプションの別名

* `--tag` *MESSAGE*

  * 出力結果にユーザ定義の追加タグを付与する
  * デフォルト: タグを設定しない

* `--dump-cmdline`

  * 出力結果にコマンドラインを含める
  * デフォルト: 出力しない

* `--time-unit` *SEC*

  * 単位時間あたりの集計を行う単位 (秒)
  * 0 の場合は、単位時間あたりの集計を行わない
  * デフォルト: 0

* `--run-time` *SEC*

  * 実行時間 (秒)
  * 設定された *SEC* の間リクエストを投げ続ける
  * *SEC* が 0 の場合 かつ `--query-num` オプションが 0 の場合は、読み込んだデータセットファイルをすべて処理する
  * *SEC* が 0 の場合 かつ `--query-num` オプションが 0 ではない場合、`--query-num` オプションに設定した件数になるまで処理を行う
  * *SEC* が 0 ではない場合 かつ `--query-num` オプションが 0 の場合は、*SEC* 経過するまでデータセットの先頭に戻り、リクエストを処理を行う
  * デフォルト: 0

* `--slient`

  * コンソールにメッセージを出力しない
  * デフォルト: 出力する

* `--verbose`

  * 冗長モードのレベルを1増やす
  * レベルを増やすことにより、コンソールにエラーメッセージなどが表示される

* `--version`

  * バージョン情報をコンソールに表示する

* `--help`

  * ヘルプメセージをコンソールに表示する


## 出力結果

Key              | 説明
-----------------|-----
summary          | 測定全体での統計情報
thread_results   | 全クエリのレイテンシー
timespan_results | 単位時間あたりの統計情報

### summary

Key                       | 説明
--------------------------|-----
tag                       | `--tag` オプションで指定した値
program                   | プログラム名
version                   | プログラムのバージョン
command_line              | コマンドライン
dataset                   | データセットファイルのパス
dataset_size              | 読み込んだデータセットの件数
dataset_description       | データセットの説明 ("SVM format dataset" 固定)
thread_num                | 単位時間あたりの統計情報
trim_percent              | 平均/分散から除外するサンプル数
query_mode                | 実行したクエリの種別
bulk_size                 | バルクサイズ
total_query_num           | 測定全体でのクエリ件数
total_success_num         | 測定全体での正常終了のクエリ件数
total_error_num           | 測定全体での正常終了以外のクエリ件数
start-datetime            | 測定開始時間
stop-datetime             | 測定終了時間
elapsed_time_msec         | 測定の実行時間 (msec)
latency_mean_msec         | 測定全体での平均レイテンシー (msec)
latency_variance          | 測定全体でのレイテンシーの分散
trimmed_latency_mean_msec | `--trim-percent` オプションで指定した量のサンプルを除外した測定全体での平均レイテンシー (msec)
trimmed_latency_variance  | `--trim-percent` オプションで指定した量のサンプルを除外した測定全体でのレイテンシーの分散
throughput_QPS            | 測定全体でのスループット

```
"summary": {
  "total_error_num": 0,
  "dataset_description": "SVM format dataset",
  "trim_percent": 5,
  "total_success_num": 7374,
  "thread_num": 2,
  "bulk_size": 1,
  "throughput_QPS": 3686.11164709,
  "trimmed_latency_mean_msec": 0.534808021696,
  "total_query_num": 7374,
  "query_mode": "update",
  "dataset_limit": 10,
  "program": "jubatus-bench\/classifier benchamrk",
  "trimmed_latency_variance": 0.00231881427194,
  "latency_variance": 0.00303155407801,
  "stop-datetime": "2013-10-23T14:25:56.781162",
  "version": "1.0.0.20130623a",
  "dataset": "url_svmlight\/Day0.svm",
  "start-datetime": "2013-10-23T14:25:54.780680",
  "latency_mean_msec": 0.53615934364,
  "elapsed_time_msec": 2000,
  "dataset_size": 10
}
```

### thread_results

以下をスレッド数繰り返す。

Key         | 説明
------------|-----
query_num   | スレッドで実行したのクエリ件数
id          | スレッドID
success_num | スレッドで実行した正常終了のクエリ件数
error_num   | スレッドで実行した正常終了以外のクエリ件数
records     | 正常終了したクエリ全件のレイテンシーを列挙 (msec)

```
"thread_results": [
  {
    "query_num": 2,
    "id": 0,
    "success_num": 2,
    "error_num": 0,
    "records": [
      1.13,
      0.639
    ]
  },
  {
    "query_num": 2,
    "id": 1,
    "success_num": 2,
    "error_num": 0,
    "records": [
      0.848,
      0.69
    ]
  }
]
```


### timespan_results

以下をスレッド数繰り返す。

Key          | 説明
-------------|-----
スレッド番号 | 単位時間あたりの結果

単位時間あたりの結果は UNIXTIME を Key に以下を出力する。

Key           | 説明
--------------|-----
num           | 単位時間あたりの実行したクエリ数
success_num   | 単位時間あたりの正常終了のクエリ数
exception_num | 単位時間あたりのRPC例外が発生したクエリ数
error_num     | 単位時間あたりのタイムアウトなどの通信エラーが発生したクエリ数
wrong_num     | 単位時間あたりの分析結果のラベルが存在しなかったクエリ数
mean          | 単位時間あたりの平均レイテンシー (msec)
variance      | 単位時間あたりのレイテンシーの分散

```
"timespan_results": {
  "0": {
    "1382505956": {
      "num": 1437,
      "success_num": 1437,
      "exception_num": 0,
      "error_num": 0,
      "wrong_num": 0,
      "mean": 0.537241475296,
      "variance": 0.0013137586695
    },
    "1382505954": {
      "num": 2250,
      "success_num": 2250,
      "exception_num": 0,
      "error_num": 0,
      "wrong_num": 0,
      "mean": 0.535444,
      "variance": 0.00136739708622
    }
  }
}
```

## 実行例

* `url_svmlight/Day0.svm` をデータセットファイルに 2分間の間 "update" クエリを実行し、10秒単位での統計結果を取得する場合:

```
$ ./jubatus-bench-classifier --in url_svmlight/Day0.svm --run-time 120 --time-unit 10
```
