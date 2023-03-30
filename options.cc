/*
 * @Date         : 2023-03-29 15:47:02
 * @Author       : huyutuo.hyt
 * @FilePath     : options.cc
 * @Description  : 
 */


#include <cstddef>
#include <cstring>
#include <getopt.h>
#include <ostream>
#include <string>
#include <iostream>

#include "options.h"

using std::cout;
using std::endl;

#define MYSQLSCT_VERSION "0.0.1"

char *user = nullptr;
char *password = nullptr;
char *database = nullptr;
std::string table_name_prefix = "sct";

char *host_ro = nullptr;
char *host_rw = nullptr;
char *host = nullptr;
uint port = 0;
uint port_rw = 0;
uint port_ro = 0;
uint sc_gap_us = 0;
uint table_cnt = 1;
uint64_t concurrency = 1;
uint64_t table_size = 1000;
uint64_t iterations = 100000;
uint64_t report_interval = 1; // s
uint detail_log = 0;
uint64_t sleep_after_sct_failed = 1000; // s
uint select_after_insert = 0;
uint short_connection = 0;

// test_mode contains "sct", "shortct"
char *test_mode = nullptr;

static const struct option long_options[] = {
    {"version", 0, nullptr, 'v'},          {"help", 0, nullptr, '?'},
    {"host-rw", 1, nullptr, 'h'},          {"host-ro", 1, nullptr, 'H'},
    {"database", 1, nullptr, 'D'},         {"port-rw", 1, nullptr, 'P'},
    {"port-ro", 1, nullptr, 'O'},          {"user", 1, nullptr, 'u'},
    {"password", 1, nullptr, 'p'},         {"iterations", 1, nullptr, 'i'},
    {"table-cnt", 1, nullptr, 'T'},        {"table-size", 1, nullptr, 't'},
    {"sc-gap-us", 1, nullptr, 's'},        {"report-interval", 1, nullptr, 'r'},
    {"detail-log", 1, nullptr, 'k'},       {"concurrency", 1, nullptr, 'c'},
    {"short-connection", 1, nullptr, 'S'}, {"test-mode", 1, nullptr, 'm'},
    {"port", 1, nullptr, 'R'}, {"host", 1, nullptr, 'o'},
};



bool parse_option(int argc, char *argv[]) {
  int opt = 0;
  if (argc == 1) {
    usage();
    return false;
  }

  while (
      (opt = getopt_long(argc, argv, "?vH:h:D:P:O:i:T:t:s:r:u:p:c:k:S:m:R:o:",
                         long_options, nullptr)) != -1) {
    switch (opt) {
    case 'S':
      short_connection = atoi(optarg);
      break;

    case '?':
      usage();
      return false;
    case 'v':
      cout << MYSQLSCT_VERSION << endl;
      return false;

    case 'h':
      host_rw = strdup(optarg);
      break;

    case 'H':
      host_ro = strdup(optarg);
      break;

    case 'D':
      database = strdup(optarg);
      break;

    case 'P':
      port_rw = atoi(optarg);
      break;

    case 'O':
      port_ro = atoi(optarg);
      break;

    case 'i':
      iterations = atoll(optarg);
      break;

    case 'T':
      table_cnt = atoi(optarg);
      break;

    case 't':
      table_size = atoi(optarg);
      break;

    case 's':
      sc_gap_us = atoi(optarg);
      break;

    case 'r':
      report_interval = atoi(optarg);
      break;

    case 'u':
      user = strdup(optarg);
      break;

    case 'p':
      password = strdup(optarg);
      break;

    case 'c':
      concurrency = atoi(optarg);
      break;

    case 'k':
      detail_log = atoi(optarg);
      break;
    
    case 'm':
      test_mode = strdup(optarg);
      break;
    
    case 'R':
      port = atoi(optarg);
      break;
    
    case 'o':
      host = strdup(optarg);
      break;

    default:
      cout << "parse argument failed" << endl;
      usage();
      return false;
    }
  }

  return true;
}

void usage() {
  cout << "Usage: mysqlsct [OPTIONS]" << endl;
  cout << "-?	--help		Display this help and exit.\n";
  cout << "-v	--version	Output version information and exit.\n";
  cout << "-h	--host-rw	mysql RW node host.\n";
  cout << "-H	--host-ro	mysql RO node host.\n";
  cout << "-D	--database	mysql database.\n";
  cout << "-P	--port-rw	mysql RW node port.\n";
  cout << "-o	--port-ro	mysql RO node port.\n";
  cout << "-u	--user	mysql user.\n";
  cout << "-p	--password	mysql password.\n";
  cout << "-i	--iterations	number of times to run the tests.\n";
  cout << "-T	--table-cnt	number of tables to test.\n";
  cout << "-t	--table-size	table size.\n";
  cout << "-s	--sc-gap-us	time(us) to sleep after write.\n";
  cout << "-r	--report-interval	periodically report intermediate "
          "statistics with a specified interval in seconds.\n";
  cout << "-k	--detail-log	print detail error log.\n";
  cout << "-c	--concurrency	number of threads to use.\n";
  cout << "-S --short-connection use short connection.\n";
  cout << "-m --test-mode choose test mode";
}

bool verify_variables() {
  bool res = true;
  cout << "Input parameters: " << endl;
  if (host_rw)
    cout << "host-rw: " << host_rw << endl;
  if (host_ro)
    cout << "host-ro: " << host_ro << endl;
  if (host)
    cout << "host" << host << endl;
  cout << "database: " << database << endl;
  cout << "port-rw: " << port_rw << endl;
  cout << "port-ro: " << port_ro << endl;
  cout << "user: " << user << endl;
  cout << "password: " << password << endl;
  cout << "iterations: " << iterations << endl;
  cout << "table-cnt: " << table_cnt << endl;
  cout << "table-size: " << table_size << endl;
  cout << "sc-gap-us: " << sc_gap_us << endl;
  cout << "report-interval: " << report_interval << endl;
  cout << "detail-log: " << detail_log << endl;
  cout << "concurrency: " << concurrency << endl;
  cout << "short-connection: " << short_connection << endl;
  cout << "test-mode: " << test_mode << endl;
  cout << "###########################################" << endl;

  if (strcmp(test_mode, "sct") == 0) {
    if (host_rw == nullptr) {
      std::cerr << "miss host_rw.\n";
      res = false;
    }

    if (host_ro == nullptr) {
      std::cerr << "miss host_ro.\n";
      res = false;
    }

    if (port_rw == 0) {
      std::cerr << "miss port_rw.\n";
      res = false;
    }
  } else if (strcmp(test_mode, "shortct") == 0) {
    if (host == nullptr) {
      std::cerr << "miss host \n";
      res = false;
    }
    if (port == 0) {
      std::cerr << "miss port.\n";
      res = false;
    }
  } else {
    std::cerr << "wrong test mode: " << test_mode << endl;
    std::cerr << "only suport sct, shotct mode now" << endl;
    res = false;
  }

  if (database == nullptr) {
    std::cerr << "miss database.\n";
    res = false;
  }

  return res;
}

void free_option() {
  if (host != nullptr) {
    free(host);
    host = nullptr;
  }

  if (host_rw != nullptr) {
    free(host_rw);
    host_rw = nullptr;
  }

  if (host_ro != nullptr) {
    free(host_ro);
    host_ro = nullptr;
  }

  if (user != nullptr) {
    free(user);
    user = nullptr;
  }

  if (password != nullptr) {
    free(password);
    password = nullptr;
  }

  if (database != nullptr) {
    free(database);
    database = nullptr;
  }
}