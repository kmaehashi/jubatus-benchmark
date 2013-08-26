// -*- mode: c++; coding: utf-8 -*-

#include <stdio.h>
#include <string>

#include "time_span.h"

namespace jubatus {
namespace bench {

std::string TimeSpan::format_datetime(const time_type& t) {
  datetime_type dt(t);
  char buf[32];

  snprintf(buf, sizeof(buf)/sizeof(buf[0]), 
           "%04d-%02d-%02dT%02d:%02d:%02d.%06d",
           dt.year, dt.mon, dt.mday, dt.hour, dt.min, dt.sec, dt.usec);
  return std::string(buf);
}

} // bench
} // jubatus
