#include "profiler.h"

#include <iostream>

#include "absl/strings/str_format.h"

#include "format_basic.h"


Profiler::Profiler(bool enable) {
  enable_ = enable;
  if (enable_) {
    start_ = std::chrono::steady_clock::now();
  }
}

// TODO: Add user time and system time.
void Profiler::finish(const std::string& operation) {
  if (enable_) {
    const auto finish = std::chrono::steady_clock::now();
    const double time_sec = (finish - start_) / std::chrono::milliseconds(1) / 1000.;
    // const auto message = absl::StrFormat("[Profiler] %s: %6.2f s", operation, time_sec);
    const auto message = absl::StrFormat("[Profiler] %6.2fs @ %s", time_sec, operation);
    std::cerr << fmt::colored(message, TextColor::gray) << "\n";
    start_ = finish;
  }
}
