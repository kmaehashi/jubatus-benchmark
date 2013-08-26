// -*- mode: c++; coding: utf-8 -*-

#ifndef JUBATUS_X_BENCH_DATASET_
#define JUBATUS_X_BENCH_DATASET_

#include <fstream>
#include <tr1/memory>

namespace jubatus {
namespace bench {

class Dataset {
public:
  virtual ~Dataset() {}
  virtual size_t size() const = 0;
  virtual std::string description() const = 0;
};

} // namespace bench
} // namespace jubatus

#endif // JUBATUS_X_BENCH_DATASET_

