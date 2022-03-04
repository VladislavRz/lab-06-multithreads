// Copyright 2022 VladislavRz <rzhevskii_vladislav@mail.ru>

#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <future>
#include <nlohmann/json.hpp>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

#include "../third-party/PicoSHA2/picosha2.h"

#ifndef INCLUDE_BRUTEFORCE_HPP_
#define INCLUDE_BRUTEFORCE_HPP_

#define HASH_SIZE 60
#define ENTRY_SIZE 5

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
using json = nlohmann::json;

// Init entry string and check hash
bool last_4_zero(const std::string& str);
std::vector<unsigned char> randomiser(std::string& str);

// Multithreads functions
void get_0000();
void get_0000_json();

// Log setup
void log_init();

// Create n threads
void parse_args(int argc, char* argv[]);
std::vector<std::future<void>> create_threads(unsigned int n);
std::vector<std::future<void>> create_threads(long n);
std::vector<std::future<void>> create_threads_json(long n);

// Catch Ctrl + C
void sigint(int sig);

#endif // INCLUDE_BRUTEFORCE_HPP_
