#include "memory_usage_micro.h"

#include "absl/strings/str_format.h"

#include "string.h"


std::string to_string(const MemoryUsage& u) {
  // return pad_left(to_string_with_thousand_sep(u.bytes), 12) + " B, "
  //   + pad_left(to_string_with_thousand_sep(u.allocations), 6) + " alloc";
  constexpr size_t kAllocationOverhead = 16;
  const size_t total_bytes_estimate = u.bytes + u.allocations * kAllocationOverhead;
  return absl::StrFormat(
    "%s + %4.1fk alloc ~ %s",
    format_bytes(u.bytes), u.allocations / 1000., format_bytes(total_bytes_estimate)
  );
}
