
#include <iostream>

#include <jubatus/client.hpp>

#include "dataset_svm.h"
#include "exception.h"
#include "main.h"
#include "query_runner.h"

using pfi::system::time::get_clock_time;

namespace jubatus {
namespace bench {

class ClassifierBench;

class ClassifierQueryRunner: public QueryRunner {
public:
  ClassifierQueryRunner(ClassifierBench *bench, size_t id, size_t query_num):
    QueryRunner(id, query_num), bench_(bench) {
  }
  virtual ~ClassifierQueryRunner() {}
  virtual void execute();

private:
  void execute_train();
  void execute_classify();

  ClassifierBench* bench_;
};

class ClassifierBench: public Main {
public:
  typedef jubatus::classifier::datum datum_type;
  typedef std::vector<std::pair<std::string, double> > num_values_type;

public:
  virtual const char* program_name() {
    return "jubatus-bench-classifier";
  }
  
  virtual const char* short_name() {
    return "jubatus-bench-classifier";
  }

  virtual const char* long_name() {
    return "jubatus-bench/classifier benchamrk";
  }

  virtual const char* version_name() {
    return "1.0.0.20130623a";
  }

  virtual std::string usage_query_mode() {
    return "\"update\" or \"train\", \"analyze\" or \"classify\"";
  }

  virtual void create_datasets() {
    if ( verbose ) std::clog << "# dataset: loading: " << dataset_path << std::endl;
    dataset_.load( dataset_path, dataset_limit );
    if ( verbose ) std::clog << "# dataset: loaded: " << dataset_.size() << " items" << std::endl;

#if 0
    for(size_t i = 0; i < dataset_.size(); ++i ) {
      const DatasetSVM<datum_type>::label_datum_type &ld = dataset_.get(i);
      std::clog << "[" << i << "] label=" << ld.first << std::endl;
      const num_values_type& nv = ld.second.num_values;
      for(size_t j = 0; j < nv.size(); ++j ) {
        std::clog << "    " << nv[j].first << ": " << nv[j].second << std::endl;
      }
      std::clog << "----" << std::endl;
    }
#endif
  }
  virtual size_t dataset_size() const {
    return dataset_.size();
  }
  virtual std::string dataset_description() const {
    return dataset_.description();
  }

  virtual Main::query_runner_ptr create_query_runner(size_t id, size_t query_num) {
    return query_runner_ptr( new ClassifierQueryRunner( this, id, query_num ));
  }

private:
  DatasetSVM<datum_type> dataset_;

  friend class ClassifierQueryRunner;
};

void ClassifierQueryRunner::execute() {
  if ( bench_->query_mode == "train" || bench_->query_mode == "update" || 
       bench_->query_mode == "")
    execute_train();
  else if ( bench_->query_mode == "classify" || bench_->query_mode == "analyze" )
    execute_classify();
  else
    throw app_error( "invalid query-mode" );
}

void ClassifierQueryRunner::execute_train() {
  typedef ClassifierBench::datum_type datum_type;
  typedef DatasetSVM<ClassifierBench::datum_type> dataset_type;

  jubatus::classifier::client::classifier client( bench_->host, bench_->port, bench_->timeout_sec );

  size_t i = 0;
  size_t idx = 0;
  bool looped = false;
  while ( true ) {

    // stop by run-time
    if ( bench_->run_time > 0 &&
          get_clock_time() - bench_->exec_time.start_clocktime() > bench_->run_time ) {
      if ( bench_->verbose ) { 
        std::clog << "# perform queries: stoped by run_time: "
            << bench_->run_time << " sec" << std::endl;
        }
      break;
    }

    // stop by query_num
    if ( query_num > 0 && i == query_num ) break;

    std::vector<dataset_type::label_datum_type> train_data;
    for(size_t j = 0; j < bench_->bulk_size; ++j ) {
      if ( bench_->dataset_.size() == idx + bench_->bulk_size ) {
        idx = 0;
        looped = true;
      }
      const dataset_type::label_datum_type &ld = bench_->dataset_.get(idx);
      train_data.push_back(ld);
      ++idx;
    }

    int err_code = 0;
    TimeSpan t;
    t.start();
    BEGIN_SAFE_RPC_CALL() {
      client.train( bench_->cluster_name, train_data);
    }
    END_SAFE_RPC_CALL( client, bench_->verbose, err_code );
    t.stop();

    QueryResult result = QueryResult(i);
    result.err_code = err_code;
    result.query_time = t;
    results.push_back(result);

    ++i;

    if ( bench_->verbose && i % 1000 == 0 ) {
      std::clog << "# perform queries: " << i << std::endl;
    }

    // set no run_time or query_num
    if ( bench_->run_time == 0 && query_num == 0 && looped ) break;
  }
}

void ClassifierQueryRunner::execute_classify() {
  using jubatus::classifier::estimate_result;
  typedef ClassifierBench::datum_type datum_type;
  typedef DatasetSVM<ClassifierBench::datum_type> dataset_type;

  jubatus::classifier::client::classifier client( bench_->host, bench_->port, bench_->timeout_sec );

  size_t i = 0;
  size_t idx = 0;
  bool looped = false;
  while ( true ) {

    // stop by run-time
    if ( bench_->run_time > 0 &&
          get_clock_time() - bench_->exec_time.start_clocktime() > bench_->run_time ) {
      if ( bench_->verbose ) {
        std::clog << "# perform queries: stoped by run_time: "
            << bench_->run_time << " sec" << std::endl;
        }
      break;
    }

    // stop by query_num
    if ( query_num > 0 && i == query_num ) break;

    std::vector<dataset_type::datum_type> classify_data;
    for(size_t j = 0; j < bench_->bulk_size; ++j ) {
      if ( bench_->dataset_.size() == idx + bench_->bulk_size ) {
        idx = 0;
        looped = true;
      }
      const dataset_type::label_datum_type &ld = bench_->dataset_.get(idx);
      classify_data.push_back(ld.second);
      ++idx;
    }

    int err_code = 0;
    TimeSpan t;
    t.start();
    BEGIN_SAFE_RPC_CALL() {
      std::vector<std::vector<estimate_result> > result
          = client.classify( bench_->cluster_name, classify_data);
      for (size_t i = 0; i < result.size(); ++i) {
        if ( result[i].empty() ) err_code = QueryResult::ERR_WRONG_RESULT;
      }
    }
    END_SAFE_RPC_CALL(client, bench_->verbose, err_code);
    t.stop();

    QueryResult result = QueryResult(i);
    result.err_code = err_code;
    result.query_time = t;
    results.push_back(result);

    ++i;

    if ( bench_->verbose && i % 1000 == 0 ) {
      std::clog << "# perform queries: " << i << std::endl;
    }

    // set no run_time or query_num
    if ( bench_->run_time == 0 && query_num == 0 && looped ) break;
  }
}

} // namespace bench
} // namespace jubatus

int main(int argc, char **argv) {
  jubatus::bench::ClassifierBench bench;
  return bench.start(argc, argv);
}
