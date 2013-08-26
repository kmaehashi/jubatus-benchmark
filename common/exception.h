// -*- mode: c++; coding: utf-8 -*-

#ifndef JUBATUS_X_BENCH_EXCEPTION_
#define JUBATUS_X_BENCH_EXCEPTION_

#include <stdexcept>
#include <string>

namespace jubatus {
namespace bench {

class exception: public std::runtime_error {
public:
  explicit exception(const std::string& what_arg) : std::runtime_error(what_arg) {}
};

class app_error: public jubatus::bench::exception {
public:
  explicit app_error(const std::string& what_arg, int exit_code = 2) :
    exception(what_arg), exit_code_(exit_code) {}

  int exit_code() const { return exit_code_; }
private:
  int exit_code_;
};

class option_error: public app_error {
public:
  explicit option_error(const std::string& what_arg = "") :
    app_error(what_arg, 1) {}
};

} // namespace bench
} // namespace jubatus

#endif // JUBATUS_X_BENCH_EXCEPTION_
