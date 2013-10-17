// -*- mode: c++; coding: utf-8 -*-

#include <pficommon/lang/bind.h>
#include <pficommon/lang/function.h>

#include "query_runner.h"

using namespace pfi::text::json;

namespace jubatus {
namespace bench {

QueryRunner::QueryRunner(size_t _id, size_t _query_num):
  id(_id),
  query_num(_query_num),
  results(),
  thread_(NULL) {
}

QueryRunner::~QueryRunner() {
  if ( thread_ ) delete thread_;
  thread_ = NULL;
}

namespace {
void call_execute_wrapper(QueryRunner *runner) {
  runner->execute_wrapper();
}
} // namespace <anonymous>

void QueryRunner::start() {
  thread_ = new pfi::concurrent::thread( pfi::lang::bind( call_execute_wrapper, this ) );
  thread_->start();
}

void QueryRunner::execute_wrapper() {
  exec_time.start();
  try {
    execute();
    exec_time.stop();
  } catch(...) {
    exec_time.stop();
    throw;
  }
}

void QueryRunner::execute() {
}

void QueryRunner::join() {
  if ( thread_ ) thread_->join();
}

const std::vector<QueryResult>& QueryRunner::get_results() {
  return results;
}

size_t QueryRunner::get_success_num() {
  size_t success_num = 0;
  for(size_t i = 0; i < results.size(); ++i ) {
    if ( results[i].err_code == 0  ) ++success_num;
  }
  return success_num;
}

json QueryRunner::get_result_as_json() {
  json result(new json_object());
  result["id"] = new json_integer(id);
  result["query_num"] = new json_integer(results.size());
  
  size_t success_count = 0;
  json records(new json_array());
  for(size_t i = 0; i < results.size(); ++i ) {
    if ( results[i].err_code == 0  ) {
      records.add( new json_float( results[i].query_time.elapsed_time_msec()) );
      ++success_count;
    }
  }
  result["success_num"] = new json_integer(success_count);
  result["error_num"] = new json_integer(results.size() - success_count);
  result["records"] = records;

  return result;
}

} // namespace bench
} // namespace jubatus
