#include <chrono>
#include <iomanip>
#include <iostream>

#include "cpp/lib/polylog_qli.h"


int main(int argc, char *argv[]) {
  for (int w : range(4, 8)) {
    const int iteration = w < 6 ? 20 : 5;
    const auto start = std::chrono::steady_clock::now();
    for (EACH : range(iteration)) {
      auto expr = QLiVec(w, seq_incl(1, 8));
    }
    const auto finish = std::chrono::steady_clock::now();
    const double time_sec = (finish - start) / std::chrono::milliseconds(1) / 1000. / iteration;
    std::cout << "(" << w << ", " << std::fixed << std::setprecision(3) << time_sec << ")\n";
  }
}
