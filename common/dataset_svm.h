// -*- mode: c++; coding: utf-8 -*-

#ifndef JUBATUS_X_BENCH_DATASET_SVM_
#define JUBATUS_X_BENCH_DATASET_SVM_

#include <fstream>
#include <tr1/memory>
#include <jubatus/util/lang/cast.h>

#include <jubatus/client/classifier_types.hpp>

#include "dataset.h"
#include "exception.h"

namespace jubatus {
namespace bench {

template<typename DATUM>
class DatasetSVM : public Dataset {
public:
  typedef DATUM datum_type;
  typedef jubatus::classifier::labeled_datum label_datum_type;
  typedef std::tr1::shared_ptr<label_datum_type> label_datum_ptr;

public:
  virtual ~DatasetSVM() {}

public:
  const label_datum_type &get(size_t i) {
    return *(data_[i%data_.size()]);
  }
  virtual size_t size() const {
    return data_.size();
  }
  virtual std::string description() const {
    return "SVM format dataset";
  }

  virtual void load(const std::string& path, size_t limit = (size_t)-1) {
    std::ifstream in_file( path.c_str(), std::ios::in );
    if ( !in_file )
      throw app_error( std::string(path) + ": coudln't open file", 2);

    std::string line;
    size_t line_no = 0;
    while( limit == (size_t)-1 || line_no < limit ) {
      if ( !std::getline(in_file, line) ) break;

      ++line_no;
      label_datum_ptr d = parse_line(line, path, line_no);
      if ( d ) data_.push_back(d);
    }
  }

private:
  label_datum_ptr parse_line(const std::string& line, 
                             const std::string& src_path, size_t line_no) {
    // following lines are derived from jubatus/core/fv_converter/lisvm_convert.cpp
    
    std::stringstream in(line);

    std::string label;
    in >> label;

    std::vector<std::pair<std::string, double> > num_values;

    std::string s;
    while (in) {
      in >> s;
      if (!in) break;

      size_t p = s.find(':');
      if (p == std::string::npos) {
        std::stringstream ss;
        ss << src_path << ":" << line_no << ": invalid libsvm format: " << s;
        throw app_error(ss.str(), 2);
      }

      std::string id = s.substr(0, p);
      float val = jubatus::util::lang::lexical_cast<float>(s.substr(p + 1));
      num_values.push_back(make_pair(id, val));
    }

    label_datum_ptr result( new label_datum_type() );
    result->label.swap(label);
    result->data.string_values.clear();
    result->data.num_values.swap(num_values);

    return result;
  }

  std::vector<label_datum_ptr> data_;
};

} // namespace bench
} // namespace jubatus

#endif // JUBATUS_X_BENCH_DATASET_SVM_
