Jubatus ベンチマークツール
=====================================

ビルド手順
===========

  $ ./waf configure
  $ ./waf build

実行手順
==========

コマンドライン
--------------
jubatus-bench-classifier [COMMON OPTIONS] [SPECIFIC OPTIONS]

オプション
-----------

  --host HOST:PORT      サーバ指定
  --name CLUSTER_NAME   クラスタ名指定（デフォルト: "test")
  --timeout #SEC        クライアント接続タイムアウト時間 (デフォルト: 1000)
  --dataset FILE        データセットパス名 ( SVM形式ファイルに対応 )
  --dataset-limit #SIZE データセット読み込みけんす上限 (デフォルト: 制限なし )
  --in FILE             --dataset オプションの別名
  --query-mode MODE     実行するクエリの種別。 "update" または "analyze" を指定
  --bulk-size #NUM      バルククエリが使える際の バルクサイズ ( デフォルト: 1 )
  --thread-num #NUM     クライアントスレッド数 ( デフォルト: 1 )
  --query-num #NUM      スレッドあたりのクエリ実行数 ( デフォルト: 10000 )
  --dump-raw-data       出力結果に、全クエリのレイテンシも含める
  --no-dump-summary     出力結果に、サマライズした統計情報を含めない
  --trim-percent N      上下あわせて N% のサンプルを平均/分散に含めない (デフォルト: 5 )
  --dump-path FILE      測定結果出力先 パス名 ( デフォルト: result.json )
  --out FILE            --dump-path オプションの別名
  --tag MESSAGE         出力結果にユーザ定義の追加タグを付与する
  --dump-cmdline        出力結果にコマンドラインを含める
  --slient              コンソールにメッセージを出力しない
  --verbose             冗長モードのレベルを1増する
  --version             バージョン情報
  --help                ヘルプメッセージ


出力書式
=========

形式
-----
  JSON

項目
------

{
  "sumamry": {                  // 測定全体での統計情報

    "program":                  // プログラム名
    "version":                  // プログラムバージョン
    "command_line":             // コマンドライン

    "dataset":                  // データセットパス名
    "dataset_description":      // データセットの説明. 現状 "SVM format dataset" 固定
    "dataset_limit":            // データセット件数上限
    "dataset_size":             // 実際に読み込んだデータセット中のデータ件数

    "thread_num":               // クライアントスレッド数
    "query_num_per_thread":     // スレッドあたりの実行クエリ数
    "bulk_size":                // バルクサイズ
    "query_mode":               // 実行したクエリの種別

    "start-datetime":           // 実行開始時間
    "stop-datetime":            // 実行終了時間
    "elapsed_time_msec":        // 実行時間 (msec)

    "total_query_num":          // 実行したクエリ数合計
    "total_success_num":        // 成功したクエリ件数合計
    "total_error_num":          // 全体のエラー件数

    "throughput_QPS":           // 全体のスループット ( query-per-sec )
    "latency_mean_msec":        // レイテンシ平均(msec)
    "latency_variance":         // レイテンシ分散

    "trim_percent":              // 平均/分散から除外するサンプル数( percent )
    "trimmed_latency_mean_msec": // 上下 <trim_percent>分サンプルを除隊したレイテンシ平均(msec)
    "trimmed_latency_variance":  // 上下 <trim_percent>分サンプルを除隊したレイテンシ分散
  }

  "thread_results:" [           // 全クエリのレイテンシ
                                //     ( --dump-raw-data オプション付与時のみ )
    {
     "query_num":               // クエリ実行数
      "id":                     // スレッド ID
      "success_num":            // 成功件数
      "error_num":              // エラー件数
      "records": [ ... ]        // 成功件数分のレイテンシを列挙 ( msec )
    }
    ....                        // スレッド分 列挙
   ]
}

※ 実際の項目の出力順序は、順不同となります（ pficommon::text::json 制約 )

出力例
=======

コンソール
-----------
$ ./build/classifier/jubatus-bench-classifier --dataset ../data/classifier/machine-learning-datasets_url/url_svmlight/Day45.svm --thread 4 --query-mode train
# dataset: loading: ../data/classifier/machine-learning-datasets_url/url_svmlight/Day45.svm
# dataset: loaded: 130 items
# perform queries: start with 4 threads, 10000 queries/thread
# perform queries: end
# dumping result: result.json
# dump result: end

測定結果
----------

{
  "summary": {
    "total_error_num": 0,
    "dataset_description": "SVM format dataset",
    "query_num_per_thread": 10000,
    "trim_percent": 5,
    "total_success_num": 40000,
    "thread_num": 4,
    "bulk_size": 1,
    "throughput_QPS": 4910.66216474,
    "trimmed_latency_mean_msec": 0.805608710526,
    "total_query_num": 40000,
    "query_mode": "train",
    "dataset_limit": -1,
    "program": "jubatus-bench\/classifier benchamrk",
    "trimmed_latency_variance": 0.0612475732347,
    "latency_variance": 0.120955462125,
    "stop-datetime": "2013-06-27T16:35:46.237809",
    "version": "1.0.0.20130623a",
    "dataset": "..\/data\/classifier\/machine-learning-datasets_url\/url_svmlight\/Day45.svm",
    "start-datetime": "2013-06-27T16:35:38.092268",
    "command_line": "\".\/build\/classifier\/jubatus-bench-classifier\" \"--dataset\" \"..\/data\/classifier\/machine-learning-datasets_url\/url_svmlight\/Day45.svm\" \"--thread\" \"4\" \"--query-mode\" \"train\"",
    "latency_mean_msec": 0.808336675,
    "elapsed_time_msec": 8145,
    "dataset_size": 130
  }
}

