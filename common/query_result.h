// -*- mode: c++; coding: utf-8 -*-

#ifndef JUBATUS_X_BENCH_QUERY_RESULT_
#define JUBATUS_X_BENCH_QUERY_RESULT_

#include "time_span.h"

namespace jubatus {
namespace bench {

class QueryResult {
public:
  QueryResult(size_t _data_index = 0) :
    data_index(_data_index),
    err_code(0) {
  }
  virtual ~QueryResult() {}

  size_t data_index;
  int err_code;
  TimeSpan query_time;

  enum {
    ERR_SUCCESS = 0,
    ERR_WRONG_RESULT = 1,
    ERR_RPC_ERROR = 2,
    ERR_EXCEPTION = 3,
  };

};

} // namespace bench
} // namespace jubatus

#endif // JUBATUS_X_BENCH_QUERY_RESULT_

