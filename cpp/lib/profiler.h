#pragma once

#include <chrono>
#include <iostream>
#include <string>


class Profiler {
public:
  Profiler() {
    start_ = std::chrono::steady_clock::now();
  }

  void finish(const std::string& phase_name) {
    const auto finish = std::chrono::steady_clock::now();
    const double time_sec = (finish - start_) / std::chrono::milliseconds(1) / 1000.;
    std::cout << "Profiler: " << phase_name << " took " << time_sec << " seconds\n";
  }

private:
  std::chrono::time_point<std::chrono::steady_clock> start_;
};
