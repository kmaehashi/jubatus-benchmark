// -*- mode: c++; coding: utf-8 -*-

#ifndef JUBATUS_X_BENCH_MAIN_
#define JUBATUS_X_BENCH_MAIN_

#include <getopt.h>
#include <fstream>
#include <map>
#include <tr1/memory>
#include <string>
#include <sstream>
#include <vector>

#include <pficommon/lang/cast.h>
#include <pficommon/text/json.h>

#include "time_span.h"

namespace jubatus {
namespace bench {

class QueryRunner;

class Main {
public:
  typedef std::tr1::shared_ptr<QueryRunner> query_runner_ptr;

public:
  Main();
  virtual ~Main() {}

  virtual const char *program_name() = 0;
  virtual const char *short_name() = 0;
  virtual const char *long_name() = 0;
  virtual const char *version_name() = 0;

  virtual void setup();
  virtual void create_datasets();
  virtual void perform_queries();
  virtual void dump_result();

  virtual int start(int argc, char **argv);

  virtual query_runner_ptr create_query_runner(size_t id, size_t query_num);

  virtual size_t dataset_size() const = 0;
  virtual std::string dataset_description() const = 0;
  
  virtual pfi::text::json::json get_summary_as_json();
  virtual pfi::text::json::json get_summary_statistics_as_json(pfi::text::json::json result);
  virtual pfi::text::json::json get_result_as_json();
  virtual pfi::text::json::json get_timespan_statistics_as_json();

  virtual void show_usage(std::ostream& out);
  virtual void show_specific_options_usage(std::ostream& out); 
  virtual std::string usage_query_mode();
  virtual void add_specific_options();
  virtual bool parse_specific_option(int opt, const char *optarg);
  virtual void validate_specific_options();
  
public:
  /// common options

  // connection
  std::string host;
  int port;
  std::string cluster_name;
  unsigned int timeout_sec;

  // dataset
  std::string dataset_path;
  size_t dataset_limit;

  // query method
  std::string query_mode;
  size_t bulk_size;

  // client threads
  unsigned int thread_num;
  unsigned int query_num_per_thread;

  // result dump
  int dump_info;
  enum {
    DUMP_INFO_SUMMARY = 0x1,
    DUMP_INFO_RAW_DATA = 0x2,
    DUMP_INFO_ALL = 0xff
  };
  int trim_percent;
  std::string dump_path;
  std::string tag;
  bool dump_cmdline;
  unsigned int time_unit;

  // run time
  unsigned int run_time;

  // misc.
  int verbose;

public:
  /// result
  std::vector<query_runner_ptr> runners;
  TimeSpan exec_time;

protected:
  typedef std::map<std::string, int> OptionKeywordMap;

  void show_version(std::ostream& out);
  void add_common_options();
  void add_option(const char *name, int has_arg, int *flag, int val);
  enum {
    OPT_SPECIFIC = 0x100,
  };
  bool parse_option(int argc, char* const argv[]);
  template<typename T>
  void parse_positive_number(const char *opt_name, const char *opt_val, T& store);
  template<typename T>
  void parse_positive_or_zero_number(const char *opt_name, const char *opt_val, T& store);
  void parse_keyword(const char* opt_name, const OptionKeywordMap& keywords,
                     const char *opt_val, int& store);
  const char *reverse_keyword(const OptionKeywordMap& keywords, int val);
  void parse_hostspec( const char *opt_name, const char* opt_val,
                       std::string& host_store, int& port_store);
  std::vector<struct option> option_defs_;

private:
  void save_cmdline(char *const argv[]);

  OptionKeywordMap query_mode_map_;
  OptionKeywordMap dump_method_map_;

  std::string cmdline_;
};

template<typename T>
inline void Main::parse_positive_number(const char *opt_name, const char *opt_val, T& store) {
  int v = strtol(opt_val, NULL, 10);
  if ( v <= 0 ) {
    std::stringstream ss;
    ss << "--" << opt_name << " option requires number > 0";
    throw option_error( ss.str());
  }
  store = static_cast<unsigned int>(v);
}

template<typename T>
inline void Main::parse_positive_or_zero_number(const char *opt_name, const char *opt_val, T& store) {
  int v = strtol(opt_val, NULL, 10);
  if ( v < 0 ) {
    std::stringstream ss;
    ss << "--" << opt_name << " option requires number >= 0";
    throw option_error( ss.str());
  }
  store = static_cast<unsigned int>(v);
}

} // namespace bench
} // namespace jubatus

#endif // JUBATUS_X_BENCH_MAIN_
