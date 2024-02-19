#include "profiler.h"

#include <iostream>

#include "absl/strings/str_format.h"

#include "format_basic.h"
#include "memory_usage_rss.h"


std::string get_rss_stats() {
  // const double current_rss_mb = get_current_rss() / 1024. / 1024.;
  // const double peak_rss_mb = get_peak_rss() / 1024. / 1024.;
  // return absl::StrFormat("RSS %6.2fMB / %6.2fMB", current_rss_mb, peak_rss_mb);
  const double current_rss_gb = get_current_rss() / 1024. / 1024. / 1024.;
  const double peak_rss_gb = get_peak_rss() / 1024. / 1024. / 1024.;
  return absl::StrFormat("RSS %6.2fGB / %6.2fGB", current_rss_gb, peak_rss_gb);
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
