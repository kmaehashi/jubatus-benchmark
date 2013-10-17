// -*- mode: c++; coding: utf-8 -*-

#ifndef JUBATUS_X_BENCH_TIME_SPAN_
#define JUBATUS_X_BENCH_TIME_SPAN_

#include <time.h>
#include <string>
#include <unistd.h>
#include <pficommon/system/time_util.h>

namespace jubatus {
namespace bench {

class TimeSpan {
public:
  typedef pfi::system::time::clock_time time_type;
  typedef pfi::system::time::calendar_time datetime_type;
  
  TimeSpan()
    : start_(0, 0), end_(0, 0) {
  }
  void clear() { start_ = end_ = time_type(0,0); }
  void start() { start_ = TimeSpan::get_time(); }
  void stop() { end_ = TimeSpan::get_time(); }
  double elapsed_time_msec() const {
    time_type diff = end_ - start_;
    return diff.sec * 1000 + double(diff.usec)/1000.0;
  }

  time_type start_clocktime() const { return start_; }
  time_type end_clocktime() const { return end_; }
  datetime_type start_datetime() const { return datetime_type(start_); }
  datetime_type stop_datetiem() const { return datetime_type(end_); }
  std::string start_datetime_string() const { return format_datetime(start_); }
  std::string stop_datetime_string() const { return format_datetime(end_); }

private:
  static time_type get_time() {
    struct timespec ts;
    ::clock_gettime( CLOCK_REALTIME, &ts );
    return time_type( ts.tv_sec, ts.tv_nsec/1000 );
  }
  static std::string format_datetime( const time_type& t);

  time_type start_;
  time_type end_;
};

} // namespace bench
} // namespace jubatus

#endif //  JUBATUS_X_BENCH_TIME_SPAN_
