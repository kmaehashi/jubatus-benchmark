// -*- mode: c++; coding: utf-8 -*-

#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "default.h"
#include "exception.h"
#include "main.h"
#include "query_runner.h"
#include "query_result.h"
#include "statistics.h"

using namespace pfi::text::json;

namespace jubatus {
namespace bench {

Main::Main():
  host(OPT_DEFAULT_HOST),
  port(OPT_DEFAULT_PORT),
  cluster_name(OPT_DEFAULT_CLUSTER_NAME),
  timeout_sec(OPT_DEFAULT_TIMEOUT_SEC),
  dataset_path(OPT_DEFAULT_DATASET_PATH),
  dataset_limit(OPT_DEFAULT_DATASET_LIMIT),
  query_mode(OPT_DEFAULT_QUERY_MODE),
  bulk_size(OPT_DEFAULT_BULK_SIZE),
  thread_num(OPT_DEFAULT_THREAD_NUM),
  query_num_per_thread(OPT_DEFAULT_QUERY_NUM_PER_THREAD),
  dump_info(OPT_DEFAULT_DUMP_INFO),
  trim_percent(OPT_DEFAULT_TRIM_PERCENT),
  dump_path(OPT_DEFAULT_DUMP_PATH),
  tag(OPT_DEFAULT_TAG),
  dump_cmdline(OPT_DEFAULT_DUMP_CMDLINE),
  time_unit(OPT_DEFAULT_TIME_UNIT),
  run_time(OPT_DEFAULT_RUN_TIME),
  verbose(1) {
}

void Main::show_specific_options_usage(std::ostream& out) {
  out << "  (none)" << std::endl;
}

void Main::show_usage(std::ostream& out) {
  out << "Usage: " << program_name() << " [COMMON OPTIONS] [SPECIFIC OPTIONS]" << std::endl
      << "Common Options:" << std::endl
    //    01234567012345670123456701234567
      << "  --host HOST:PORT      server host" << std::endl
      << "  --name CLUSTER_NAME   cluster name" << std::endl
      << "  --timeout #SEC        client timeout(sec)" << std::endl
      << "  --dataset FILE        dataset name or path" << std::endl
      << "  --dataset-limit #SIZE dataset limit size" << std::endl
      << "  --in FILE             alias of --dataset" << std::endl
      << "  --query-mode MODE     query-mode" << std::endl
      << "                        (" << usage_query_mode() << ")" << std::endl
      << "  --bulk-size #NUM      bulk size" << std::endl
      << "  --thread-num #NUM     number of client threads" << std::endl
      << "  --query-num #NUM      number of query per thread" << std::endl
      << "                        if set 0, size of FILE specified by --in" << std::endl
      << "  --dump-raw-data       dump raw data" << std::endl
      << "  --no-dump-summary     do not dump summary" << std::endl
      << "  --trim-percent N      trim N% result" << std::endl
      << "  --dump-path FILE      result file pathname" << std::endl
      << "  --out FILE            alias of --dump-path" << std::endl
      << "  --tag MESSAGE         dump additional tag" << std::endl
      << "  --dump-cmdline        dump command-line" << std::endl
      << "  --time-unit #SEC      statistics time unit(sec)" << std::endl
      << "  --run-time #SEC       run time(sec)" << std::endl
      << "                        if set 0, enable only --query-num" << std::endl
      << "  --slient              silent mode" << std::endl
      << "  --verbose             increase verbose level" << std::endl
      << "  --version             show version" << std::endl
      << "  --help                show help" << std::endl
      << "Specific Options:" << std::endl;

  show_specific_options_usage(out);
}

std::string Main::usage_query_mode() {
  return "none";
}

void Main::show_version(std::ostream& out) {
  out << program_name() << " version " << version_name() << std::endl;
}

void Main::add_option( const char *name, int has_arg, int *flag, int val) {
  struct option opt;
  opt.name = name;
  opt.has_arg = has_arg;
  opt.flag = flag;
  opt.val = val;

  option_defs_.push_back( opt );
}

void Main::add_specific_options() {
}

enum {
  OPT_HOST,
  OPT_CLUSTER_NAME,
  OPT_TIMEOUT_SEC,
  OPT_DATASET_PATH,
  OPT_DATASET_LIMIT,
  OPT_QUERY_MODE,
  OPT_BULK_SIZE,
  OPT_THREAD_NUM,
  OPT_QUERY_NUM_PER_THREAD,
  OPT_DUMP_RAW_DATA,
  OPT_NO_DUMP_SUMMARY,
  OPT_TRIM_PERCENT,
  OPT_DUMP_PATH,
  OPT_TAG,
  OPT_DUMP_CMDLINE,
  OPT_TIME_UNIT,
  OPT_RUN_TIME,
  OPT_SILENT,
  OPT_VERBOSE,
  OPT_VERSION,
  OPT_HELP,
};

void Main::add_common_options() {
  add_option( "host",           required_argument, NULL, OPT_HOST);
  add_option( "name",           required_argument, NULL, OPT_CLUSTER_NAME);
  add_option( "timeout",        required_argument, NULL, OPT_TIMEOUT_SEC);
  add_option( "dataset",        required_argument, NULL, OPT_DATASET_PATH);
  add_option( "dataset-limit",  required_argument, NULL, OPT_DATASET_LIMIT);
  add_option( "in",             required_argument, NULL, OPT_DATASET_PATH);
  add_option( "query-mode",     required_argument, NULL, OPT_QUERY_MODE);
  add_option( "bulk-size",      required_argument, NULL, OPT_BULK_SIZE);
  add_option( "thread-num",     required_argument, NULL, OPT_THREAD_NUM);
  add_option( "query-num",      required_argument, NULL, OPT_QUERY_NUM_PER_THREAD);
  add_option( "dump-raw-data",  no_argument,       NULL, OPT_DUMP_RAW_DATA);
  add_option( "no-dump-summary",no_argument,       NULL, OPT_NO_DUMP_SUMMARY);
  add_option( "trim-percent",   required_argument, NULL, OPT_TRIM_PERCENT);
  add_option( "dump-path",      required_argument, NULL, OPT_DUMP_PATH);
  add_option( "out",            required_argument, NULL, OPT_DUMP_PATH);
  add_option( "tag",            required_argument, NULL, OPT_TAG);
  add_option( "dump-cmdline",   no_argument,       NULL, OPT_DUMP_CMDLINE);
  add_option( "time-unit",      required_argument, NULL, OPT_TIME_UNIT);
  add_option( "run-time",       required_argument, NULL, OPT_RUN_TIME);
  add_option( "silent",         no_argument,       NULL, OPT_SILENT);
  add_option( "verbose",        no_argument,       NULL, OPT_VERBOSE);
  add_option( "version",        no_argument,       NULL, OPT_VERSION );
  add_option( "help",           no_argument,       NULL, OPT_HELP );
}

bool Main::parse_specific_option(int opt, const char* optarg) {
  return true;
}

void Main::validate_specific_options() {
}

void Main::parse_keyword(const char *opt_name, const OptionKeywordMap& keywords,
                         const char* opt_val, int& store) {
  OptionKeywordMap::const_iterator found = keywords.find(opt_val);
  if ( found == keywords.end() ) {
    std::stringstream ss;
    ss << "--" << opt_name << " option requires keyword: ";
    bool first = true;
    for( OptionKeywordMap::const_iterator itr = keywords.begin(), end = keywords.end();
         itr != end; ++itr ) {
      if ( !first ) ss << ", ";
      ss << itr->first;
      first = false;
    }
    throw option_error( ss.str());
  }
  store = found->second;
}

const char* Main::reverse_keyword(const OptionKeywordMap& keywords, int val) {
  for( OptionKeywordMap::const_iterator itr = keywords.begin(), end = keywords.end();
       itr != end; ++itr ) {
    if ( itr->second == val ) return itr->first.c_str();
  }
  return "(unknown)";
}

void Main::parse_hostspec(const char* opt_name, const char* opt_val,
                          std::string& host_store, int& port_store) {
  const char *colon = strchr(opt_val, ':');
  if ( !colon ) {
    host_store = opt_val;
    return;
  }
  int port = strtol( colon+1, NULL, 10);
  if ( port <= 0 ) {
    std::stringstream ss;
    ss << "--" << opt_name << " option requires port-number > 0";
    throw option_error(ss.str());
  }
  host_store = std::string(opt_val, (size_t)(colon-opt_val));
  port_store = port;
}

void Main::save_cmdline(char *const argv[] ) {
  std::stringstream ss;
  for(size_t i = 0; argv[i]; ++i ) {
    if ( i > 0 ) ss << " ";
    ss << '"' << argv[i] << '"';
  }
  cmdline_ = ss.str();
}

bool Main::parse_option(int argc, char * const argv[]) {
  save_cmdline(argv);

  add_common_options();
  add_specific_options();
  add_option(0, 0, 0, 0);

  int opt = 0;
  while( (opt = getopt_long_only( argc, argv, "", &option_defs_[0], NULL)) != -1 ) {
    if ( opt >= OPT_SPECIFIC ) {
      if ( !parse_specific_option(opt, optarg) ) return false;
      continue;
    }

    switch(opt) {
    case OPT_HOST:
      parse_hostspec("host", optarg, host, port);
      break;
    case OPT_CLUSTER_NAME:
      cluster_name = optarg;
      break;
    case OPT_TIMEOUT_SEC:
      parse_positive_number("timeout", optarg, timeout_sec);
      break;
    case OPT_DATASET_PATH:
      dataset_path = optarg;
      break;
    case OPT_DATASET_LIMIT:
      parse_positive_number("dataset-limit", optarg, dataset_limit);
      break;
    case OPT_QUERY_MODE:
      query_mode = optarg;
      break;
    case OPT_BULK_SIZE:
      parse_positive_number("bulk_size", optarg, bulk_size);
      break;
    case OPT_THREAD_NUM:
      parse_positive_number("thread-num", optarg, thread_num);
      break;
    case OPT_QUERY_NUM_PER_THREAD:
      parse_positive_or_zero_number("query-num", optarg, query_num_per_thread);
      break;
    case OPT_DUMP_RAW_DATA:
      dump_info |= DUMP_INFO_RAW_DATA;
      break;
    case OPT_NO_DUMP_SUMMARY:
      dump_info &= ~DUMP_INFO_SUMMARY;
      break;
    case OPT_TRIM_PERCENT:
      parse_positive_or_zero_number("trim-percent", optarg, trim_percent);
      break;
    case OPT_DUMP_PATH:
      dump_path = optarg;
      break;
    case OPT_TAG:
      tag = optarg;
      break;
    case OPT_DUMP_CMDLINE:
      dump_cmdline = true;
      break;
    case OPT_TIME_UNIT:
      parse_positive_or_zero_number("time-unit", optarg, time_unit);
      break;
    case OPT_RUN_TIME:
      parse_positive_or_zero_number("run-time", optarg, run_time);
      break;
    case OPT_SILENT:
      verbose = 0;
      break;
    case OPT_VERBOSE:
      ++verbose;
      break;

    case OPT_VERSION:
      show_version(std::cout);
      return false;
    case OPT_HELP:
      show_usage(std::cout);
      return false;

    default:
      std::cerr << "Invalid option: " << optarg << std::endl;
      show_usage(std::cerr);
      throw option_error();
    }
  }

  validate_specific_options();

  return true;
}

void Main::setup() {
}

void Main::create_datasets() {
}

Main::query_runner_ptr Main::create_query_runner(size_t id, size_t query_num) {
  return query_runner_ptr();
}

void Main::perform_queries() {
  if ( verbose ) {
    std::clog << "# perform queries: start with " 
              << thread_num << " threads";
    if ( query_num_per_thread > 0 ) {
      std::clog << ", " << query_num_per_thread << " queries/thread";
    }
    std::clog << std::endl;
  }

  runners.clear();
  exec_time.clear();

  for(size_t i = 0; i < thread_num; ++i ) {
    query_runner_ptr qr = create_query_runner(i, query_num_per_thread);
    if ( qr ) runners.push_back(qr);
  }

  // start runners
  exec_time.start();
  for(size_t i = 0; i < runners.size(); ++i ) runners[i]->start();

  // wait runners
  for(size_t i = 0; i < runners.size(); ++i ) runners[i]->join();
  exec_time.stop();

  if ( verbose ) {
    std::clog << "# perform queries: end" << std::endl;
  }
}

json Main::get_summary_statistics_as_json(json result_json) {
  std::vector<double> latencies;
  for(size_t i = 0; i < runners.size(); ++i) {
    const std::vector<QueryResult>& results = runners[i]->get_results();
    for(size_t j = 0; j < results.size(); ++j) {
      if ( results[j].err_code == 0 ) {
        latencies.push_back( results[j].query_time.elapsed_time_msec() );
      }
    }
  }

  SimpleStatistics<double> simple_latency = 
    SimpleStatistics<double>::get_stat(latencies);
  SimpleStatistics<double> trimmed_latency = 
    SimpleStatistics<double>::get_trimmed_stat(latencies, trim_percent);

  result_json["latency_mean_msec"] = new json_float(simple_latency.mean);
  result_json["latency_variance"] = new json_float(simple_latency.variance);
  result_json["trimmed_latency_mean_msec"] = new json_float(trimmed_latency.mean);
  result_json["trimmed_latency_variance"] = new json_float(trimmed_latency.variance);

  double queries_per_sec = 0;
  double elapsed_time = exec_time.elapsed_time_msec();
  if ( elapsed_time > 1.0d ) queries_per_sec = latencies.size()/(elapsed_time/1000.0d);
  result_json["throughput_QPS"] = new json_float(queries_per_sec);
  
  return result_json;
}

json Main::get_summary_as_json() {
  json summary( new json_object() );
  
  if ( !tag.empty() )
    summary["tag"] = new json_string(tag);

  summary["program"] = new json_string(long_name());
  summary["version"] = new json_string(version_name());
  summary["command_line"] = new json_string(cmdline_);
  summary["dataset"] = new json_string(dataset_path);
  if ( dataset_limit == (size_t)-1 )
    summary["dataset_limit"] = new json_integer(-1);
  else
    summary["dataset_limit"] = new json_integer(dataset_limit);
  summary["dataset_size"] = new json_integer(dataset_size());
  summary["dataset_description"] = new json_string(dataset_description());
  summary["thread_num"] = new json_integer(thread_num);
  summary["trim_percent"] = new json_integer(trim_percent);
  summary["query_mode"] = new json_string(query_mode);
  summary["bulk_size"] = new json_integer(bulk_size);

  size_t total_num = 0;
  size_t total_success_num = 0;
  for(size_t i = 0; i < runners.size(); ++i ) {
    total_num += runners[i]->get_results().size();
    total_success_num += runners[i]->get_success_num();
  }

  summary["total_query_num"] = new json_integer(total_num);
  summary["total_success_num"] = new json_integer(total_success_num);
  summary["total_error_num"] = new json_integer(total_num - total_success_num);

  summary["start-datetime"] = new json_string(exec_time.start_datetime_string());
  summary["stop-datetime"] = new json_string(exec_time.stop_datetime_string());
  summary["elapsed_time_msec"] = new json_integer(exec_time.elapsed_time_msec());

  return summary;
}

json Main::get_result_as_json() {
  json toplevel(new json_object());
  toplevel["summary"] = get_summary_as_json();

  if ( dump_info & DUMP_INFO_SUMMARY )
    get_summary_statistics_as_json(toplevel["summary"]);

  if ( dump_info & DUMP_INFO_RAW_DATA ) {
    json thread_results( new json_array() );
    for(size_t i = 0; i < runners.size(); ++i ) {
      thread_results.add(runners[i]->get_result_as_json());
    }
    toplevel["thread_results"] = thread_results;
  }

  if (time_unit) toplevel["timespan_results"] = get_timespan_statistics_as_json();

  return toplevel;
}


json Main::get_timespan_statistics_as_json() {
  using pfi::system::time::clock_time;

  json timespan_statistics(new json_object());

  for(size_t i = 0; i < runners.size(); ++i) {
    const std::vector<QueryResult>& results = runners[i]->get_results();

    json thread_result(new json_object());
    clock_time basetime(0, 0);
    size_t success_num = 0;
    size_t wrong_num = 0;
    size_t error_num = 0;
    size_t exception_num = 0;
    std::vector<double> latencies;

    for(size_t j = 0; j < results.size(); ++j) {
      if ( results[j].query_time.start_clocktime() - basetime > time_unit) {
        if ( !latencies.empty() ) {
          json stat(new json_object());

          SimpleStatistics<double> unit_stat =
              SimpleStatistics<double>::get_stat(latencies);

          stat["mean"] = new json_float(unit_stat.mean);
          stat["variance"] = new json_float(unit_stat.variance);
          stat["num"] = new json_float(latencies.size());

          stat["success_num"] = new json_integer(success_num);
          stat["wrong_num"] = new json_integer(wrong_num);
          stat["error_num"] = new json_integer(error_num);
          stat["exception_num"] = new json_integer(exception_num);

          thread_result[pfi::lang::lexical_cast<std::string>(basetime.sec)] = stat;
        }
        basetime.sec = results[j].query_time.start_clocktime().sec;
        success_num = 0;
        wrong_num = 0;
        error_num = 0;
        exception_num = 0;
        latencies.clear();
      }

      // ratency and "num" includes success and wrong result
      switch (results[j].err_code) {
        case QueryResult::ERR_SUCCESS:
          ++success_num;
          latencies.push_back( results[j].query_time.elapsed_time_msec() );
          break;
        case QueryResult::ERR_WRONG_RESULT:
          ++wrong_num;
          latencies.push_back( results[j].query_time.elapsed_time_msec() );
          break;
        case QueryResult::ERR_RPC_ERROR:
          ++error_num;
          break;
        case QueryResult::ERR_EXCEPTION:
          ++exception_num;
          break;
      }
    }

    // last unit
    json stat(new json_object());

    SimpleStatistics<double> unit_stat =
        SimpleStatistics<double>::get_stat(latencies);

    stat["mean"] = new json_float(unit_stat.mean);
    stat["variance"] = new json_float(unit_stat.variance);
    stat["num"] = new json_float(latencies.size());

    stat["success_num"] = new json_integer(success_num);
    stat["wrong_num"] = new json_integer(wrong_num);
    stat["error_num"] = new json_integer(error_num);
    stat["exception_num"] = new json_integer(exception_num);

    thread_result[pfi::lang::lexical_cast<std::string>(basetime.sec)] = stat;
    timespan_statistics[pfi::lang::lexical_cast<std::string>(runners[i]->id)] = thread_result;
  }
  return timespan_statistics;
}

void Main::dump_result() {
  if ( verbose ) {
    std::clog << "# dumping result: " << dump_path << std::endl;
  }

  std::ofstream out( dump_path.c_str());
  if ( !out ) throw app_error( dump_path + ": couldn't open file");

  json result = get_result_as_json();
  result.pretty(out, /*escape=*/ true);
  out.close();

  if ( verbose ) {
    std::clog << "# dump result: end" << std::endl;
  }
}

int Main::start(int argc, char **argv) {
  try {
    // parse option
    if ( !parse_option(argc, argv) ) return 0;

    setup();
    create_datasets();
    perform_queries();
    dump_result();

  } catch ( jubatus::bench::app_error& e ) {
    const char *err_mesg = e.what();
    if ( err_mesg && strlen(err_mesg) > 0 ) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
    return e.exit_code();
  } catch ( std::exception& e ) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 2;
  }

  return 0;
}

} // namespace bench
} // namesapce jubatus
