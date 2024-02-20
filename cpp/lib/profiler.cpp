#include "profiler.h"

#include <iostream>

#include "absl/strings/str_format.h"

#include "format_basic.h"
#include "memory_usage_rss.h"


std::string get_rss_stats() {
  return absl::StrFormat("RSS %s / %s", format_bytes(get_current_rss()), format_bytes(get_peak_rss()));
}

void Profiler::rss_stats() {
  const auto message = absl::StrFormat("[%s]", get_rss_stats());
  std::cerr << fmt::colored(message, TextColor::gray) << "\n";
}

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
    const auto message = absl::StrFormat("[%s] %6.2fs @ %s", get_rss_stats(), time_sec, operation);
    std::cerr << fmt::colored(message, TextColor::gray) << "\n";
    start_ = finish;
  }
}
