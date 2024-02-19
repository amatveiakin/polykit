#pragma once

#include <chrono>
#include <string>


class Profiler {
public:
  static void rss_stats();

  Profiler(bool enable = true);

  void finish(const std::string& operation);

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
