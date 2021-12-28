#pragma once

#include <chrono>
#include <iostream>
#include <string>


// TODO: Add user time and system time.
class Profiler {
public:
  Profiler(bool enable = true) {
    enable_ = enable;
    if (enable_) {
      start_ = std::chrono::steady_clock::now();
    }
  }

  void finish(const std::string& operation) {
    if (enable_) {
      const auto finish = std::chrono::steady_clock::now();
      const double time_sec = (finish - start_) / std::chrono::milliseconds(1) / 1000.;
      std::cerr << "Profiler: " << operation << " took " << time_sec << " seconds\n";
      start_ = finish;
    }
  }

private:
  bool enable_ = false;
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
