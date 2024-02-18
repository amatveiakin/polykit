#include "pvector.h"

#include "compare.h"
#include "format_basic.h"
#include "string.h"
#include "util.h"


PVectorStats pvector_stats;

std::ostream& operator<<(std::ostream& os, const PVectorStats& stats) {
  constexpr int kWidth = 14;
  auto entries = to_vector(stats.data);
  absl::c_sort(entries, cmp::projected([](const auto& e) { return e.first; }));
  std::cout << "======== PVector statistics ========\n";
  std::cout
    << pad_left("empty/moved", kWidth) << " :"
    << pad_left("heap", kWidth) << " :"
    << pad_left("inlined", kWidth) << "\n";
  for (const auto& [key, value] : entries) {
    const std::string num_empty_str = to_string_with_thousand_sep(value.num_empty);
    const std::string num_heap_str = to_string_with_thousand_sep(value.num_heap);
    const std::string num_inlined_str = value.num_inlined >= 0 ? to_string_with_thousand_sep(value.num_inlined) : "-";
    const std::string rounded_size = absl::StrCat("(^", key.second, ")");
    std::cout
      << pad_left(num_empty_str, kWidth) << " :"
      << pad_left(num_heap_str, kWidth) << " :"
      << pad_left(num_inlined_str, kWidth)
      << "  @  " <<  key.first << " " << fmt::colored(rounded_size, TextColor::gray) << "\n"
    ;
  }
  return os;
}
