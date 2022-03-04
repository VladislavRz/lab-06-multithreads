#include "BruteForce.hpp"

int main(int argc, char* argv[]) {
  signal(SIGINT, sigint);
  srand(time(nullptr));
  log_init();
  parse_args(argc, argv);

  return 0;
}