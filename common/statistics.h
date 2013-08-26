// -*- mode: c++; coding: utf-8 -*-

#ifndef JUBATUS_X_BENCH_STATISTICS_
#define JUBATUS_X_BENCH_STATISTICS_

#include <vector>

namespace jubatus {
namespace bench {

template<typename T>
class SimpleStatistics {
public:
  SimpleStatistics(): mean(0), variance(0) {}
  
  void clear() {
    mean = variance = 0;
  }

  T mean;
  T variance;

public:
  static SimpleStatistics<T> get_stat(std::vector<T>& data);
  static SimpleStatistics<T> get_stat(typename std::vector<T>::iterator data_start,
                                      typename std::vector<T>::iterator data_end);
  static SimpleStatistics<T> get_trimmed_stat(std::vector<T>&data, size_t percent);
};

template<typename T>
inline SimpleStatistics<T> SimpleStatistics<T>::get_stat(std::vector<T>& data) {
  return SimpleStatistics<T>::get_stat(data.begin(), data.end());
}

template<typename T>
inline SimpleStatistics<T> SimpleStatistics<T>::get_stat(typename std::vector<T>::iterator data_start,
                                     typename std::vector<T>::iterator data_end) {
  SimpleStatistics<T> result;

  T sum = 0;
  size_t n = 0;
  for(typename std::vector<T>::iterator itr = data_start; itr != data_end; ++itr) {
    sum += *itr;
    ++n;
  }
  if ( n > 0 ) result.mean = sum/n;

  if ( n > 0 ) {
    T diff2_sum = 0;
    for(typename std::vector<T>::iterator itr = data_start; itr != data_end; ++itr) {
      T diff = *itr - result.mean;
      diff2_sum += diff * diff;
    }
    result.variance = diff2_sum/n;
  }
  return result;
}

template<typename T>
inline SimpleStatistics<T> SimpleStatistics<T>::get_trimmed_stat(std::vector<T>& data, size_t percent) {
  std::sort(data.begin(), data.end());

  size_t range = data.size() * percent / 100;
  if ( range >= data.size() ) return get_stat(data);

  typename std::vector<T>::iterator start = data.begin() + range/2;
  typename std::vector<T>::iterator end = data.end() - range/2;
  return get_stat(start, end);
}

} // bench
} // jubatus

#endif // JUBATUS_X_BENCH_STATISTICS_
