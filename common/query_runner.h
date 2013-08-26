// -*- mode: c++; coding: utf-8 -*-

#ifndef JUBATUS_X_BENCH_QUERY_RUNNER_
#define JUBATUS_X_BENCH_QUERY_RUNNER_

#include <vector>

#include <pficommon/concurrent/thread.h>
#include <pficommon/text/json.h>

#include "query_result.h"
#include "time_span.h"

namespace jubatus {
namespace bench {

class QueryRunner {
public:
  QueryRunner(size_t id, size_t query_num);
  virtual ~QueryRunner();
    
  virtual void start();
  virtual void execute();
  virtual void join();

  virtual const std::vector<QueryResult>& get_results();
  virtual size_t get_success_num();
  virtual pfi::text::json::json get_result_as_json();
  
  virtual void execute_wrapper();

public:
  // result
  size_t id;
  size_t query_num;
  std::vector<QueryResult> results;
  TimeSpan exec_time;

protected:
  pfi::concurrent::thread* thread_;
};

#define BEGIN_SAFE_RPC_CALL() try {
#define END_SAFE_RPC_CALL(client, verbose, errcode)     \
  } catch( msgpack::rpc::rpc_error& e) {                               \
    if ( (verbose) > 1 ) {                                              \
      std::cerr << "Thread[" << id << "]: " << e.what() << std::endl;   \
    }                                                                   \
    (client).get_client().close();                                      \
    errcode = QueryResult::ERR_RPC_ERROR;                               \
  } catch ( std::exception &e) {                                        \
    if ( (verbose) > 1 ) {                                              \
      std::cerr << "Thread[" << id << "]: " << e.what() << std::endl;   \
    }                                                                   \
    (client).get_client().close();                                      \
    errcode = QueryResult::ERR_EXCEPTION;                               \
  }
  

} // namespace bench
} // namespace jubatus

#endif // JUBATUS_X_BENCH_QUERY_RUNNER_
