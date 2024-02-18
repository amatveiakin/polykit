#include "profiler.h"

#include <iostream>

#include "absl/strings/str_format.h"

#include "format_basic.h"
#include "memory_usage_rss.h"


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
    const double current_rss_gb = get_current_rss() / 1024. / 1024. / 1024.;
    const double peak_rss_gb = get_peak_rss() / 1024. / 1024. / 1024.;
    const double time_sec = (finish - start_) / std::chrono::milliseconds(1) / 1000.;
    const auto message = absl::StrFormat(
      "[RSS %6.2fGB / %6.2fGB] %6.2fs @ %s",
      current_rss_gb, peak_rss_gb, time_sec, operation
    );
    std::cerr << fmt::colored(message, TextColor::gray) << "\n";
    start_ = finish;
  }
}
