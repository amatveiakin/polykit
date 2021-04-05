#pragma once

#include <chrono>
#include <iostream>
#include <string>


// TODO: Rename to `Timer`.
class Profiler {
public:
  Profiler() {
    start_ = std::chrono::steady_clock::now();
  }

  void finish(const std::string& operation) {
    const auto finish = std::chrono::steady_clock::now();
    const double time_sec = (finish - start_) / std::chrono::milliseconds(1) / 1000.;
    std::cout << "Profiler: " << operation << " took " << time_sec << " seconds\n";
    start_ = finish;
  }

private:
  std::chrono::time_point<std::chrono::steady_clock> start_;
};


class ScopedProfiler {
public:
  ScopedProfiler(std::string operation) : operation_(std::move(operation)) {}
  ~ScopedProfiler() {
    profiler_.finish(operation_);
  }

private:
  Profiler profiler_;
  std::string operation_;
};
