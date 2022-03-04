// Copyright 2022 VladislavRz <rzhevskii_vladislav@mail.ru>

#include <BruteForce.hpp>

unsigned char alphabet[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                            'A', 'B', 'C', 'D', 'E', 'F'};

std::mutex th_mute;
bool stop_flag = false;
json j_arr;

bool last_4_zero(const std::string& str) {
  if (str.substr(HASH_SIZE) != "0000") {
    return false;
  }
  return true;
}

std::vector<unsigned char> randomiser(std::string& str) {
  std::vector<unsigned char> data;
  unsigned char c;

  if (!str.empty()) {
    str.clear();
  }

  for (size_t i = 0; i < ENTRY_SIZE; ++i) {
    c = static_cast<unsigned char>(alphabet[rand()%15]);
    data.push_back(c);
    str.push_back(c);
  }
  return data;
}

void get_0000() {
  bool exit_flag = false;
  std::string hash;
  std::string entry_str;
  while (!exit_flag) {
    hash = picosha2::hash256_hex_string(randomiser(entry_str));
    if (!last_4_zero(hash)) {
      th_mute.lock();
      BOOST_LOG_TRIVIAL(trace) << "[" << entry_str << "] " << hash;
      th_mute.unlock();
    } else {
      th_mute.lock();
      BOOST_LOG_TRIVIAL(info) << "[" << entry_str << "] " << hash;
      th_mute.unlock();
    }

    exit_flag = stop_flag;
  }
}

void get_0000_json() {
  json j;
  bool exit_flag = false;
  std::string hash;
  std::string entry_str;
  time_t start = clock();
  time_t end;
  while (!exit_flag) {
    hash = picosha2::hash256_hex_string(randomiser(entry_str));
    if (!last_4_zero(hash)) {
      th_mute.lock();
      BOOST_LOG_TRIVIAL(trace) << "[" << entry_str << "] " << hash;
      th_mute.unlock();
    } else {
      end = clock();
      j["timestamp"] = static_cast<double>((end - start) / CLOCKS_PER_SEC);
      j["hash"] = hash;
      j["data"] = entry_str;

      th_mute.lock();
      BOOST_LOG_TRIVIAL(info) << "[" << entry_str << "] " << hash;
      j_arr.push_back(j);
      th_mute.unlock();
      j.clear();
    }

    exit_flag = stop_flag;
  }
}

void log_init() {
  logging::add_common_attributes();
  logging::add_file_log
      (
          keywords::file_name = "trace_%N.log",
          keywords::target_file_name = "trace_%N.log",
          keywords::rotation_size = 5 * 1024 * 1024,
          keywords::time_based_rotation =
              sinks::file::rotation_at_time_point(0, 0, 0),
          keywords::filter =
              logging::trivial::severity <= logging::trivial::trace,
          keywords::format = "[%ThreadID%] %Message%");

  logging::add_common_attributes();
  logging::add_file_log
      (
          keywords::file_name = "info_%N.log",
          keywords::target_file_name = "info_%N.log",
          keywords::rotation_size = 5 * 1024 * 1024,
          keywords::time_based_rotation =
              sinks::file::rotation_at_time_point(0, 0, 0),
          keywords::filter =
              logging::trivial::severity >= logging::trivial::info,
          keywords::format = "[%ThreadID%] %Message%");

  logging::add_console_log(std::cout,
                           keywords::format = "[%ThreadID%]"
                               "[%Severity%] %Message%");
}

void parse_args(int argc, char* argv[]) {
  std::vector<std::future<void>> threads;
  if (argc == 1) {
    create_threads(std::thread::hardware_concurrency());
  } else if (argc == 2) {
    create_threads(strtol(argv[1], nullptr, 10));
  } else if (argc == 3) {
    create_threads_json(strtol(argv[1], nullptr, 10));
  } else {
    throw std::runtime_error("incorrect arguments");
  }

  if (!j_arr.empty()) {
    std::string str = argv[2];
    str += ".json";
    std::ofstream f(str);
    if (f.is_open()) {
      f << j_arr.dump(4);
      f.close();
    }
    j_arr.clear();
  } else {
    throw std::runtime_error("error while open file");
  }
}

std::vector<std::future<void>> create_threads(unsigned int n) {
  std::vector<std::future<void>> threads(n);
  for (size_t i = 0; i < n; ++i) {
    threads.push_back(std::async(std::launch::async, get_0000));
  }

  return threads;
}

std::vector<std::future<void>> create_threads(long n) {
  if (n <= 0) throw std::runtime_error("incorrect number of threads");

  std::vector<std::future<void>> threads(n);
  for (long i = 0; i < n; ++i) {
    threads.push_back(std::async(std::launch::async, get_0000));
  }

  return threads;
}

std::vector<std::future<void>> create_threads_json(long n) {
  if (n <= 0) throw std::runtime_error("incorrect number of threads");

  std::vector<std::future<void>> threads(n);
  for (long i = 0; i < n; ++i) {
    threads.push_back(std::async(std::launch::async, get_0000_json));
  }

  return threads;
}

void sigint(int sig) {
  th_mute.lock();
  stop_flag = true;
  th_mute.unlock();
  std::cout << "Catch signal: " << sig << std::endl;
}
