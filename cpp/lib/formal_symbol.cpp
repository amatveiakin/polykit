#include "formal_symbol.h"

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "polylog_multiarg.h"


constexpr int kWordsPerByte = 2;  // Note: some code implicitly assumes it's 2
constexpr int kShift = CHAR_BIT / kWordsPerByte;
constexpr int kMaxValue = (1 << kShift) - 1;
constexpr int kLowerValueMask = (1 << kShift) - 1;

static constexpr bool char_ok(int ch) {
  return 0 <= ch && ch <= kMaxValue && ch != kFormalSymbolSentinel;
};

// Stores each int as half-byte. Input cannot end with kFormalSymbolSentinel.
// Optimization potential: use varint encoding instead.
static std::vector<unsigned char> compress(absl::Span<const int> data) {
  CHECK(!data.empty());
  CHECK_NE(data.back(), kFormalSymbolSentinel);
  std::vector<unsigned char> compressed;
  for (int i = 0; i < data.size(); i += kWordsPerByte) {
    const int a = data[i];
    const int b = (i+1 < data.size()) ? data[i+1] : kFormalSymbolSentinel;
    compressed.push_back((a << kShift) + b);
  }
  return compressed;
}

static std::vector<int> decompress(absl::Span<const unsigned char> compressed) {
  CHECK(!compressed.empty());
  std::vector<int> data;
  for (unsigned char ch : compressed) {
    const int a = ch >> kShift;
    const int b = ch & kLowerValueMask;
    data.push_back(a);
    data.push_back(b);
  }
  while (!data.empty() && data.back() == kFormalSymbolSentinel) {
    data.pop_back();
  }
  CHECK(!data.empty());
  return data;
}

static std::vector<int> scan_vector(absl::Span<const int>& input) {
  CHECK(!input.empty());
  const auto it = absl::c_find(input, kFormalSymbolSentinel);
  auto ret = std::vector(input.begin(), it);
  input = input.subspan((it == input.end() ? it : it + 1) - input.begin());
  return ret;
}

Word formal_symbol(
    const std::vector<int>& weights,
    const std::vector<std::vector<int>>& points) {
  std::vector<int> data;
  CHECK(absl::c_all_of(weights, char_ok));
  append_vector(data, weights);
  for (const auto& p : points) {
    data.push_back(kFormalSymbolSentinel);
    CHECK(absl::c_all_of(p, char_ok));
    append_vector(data, p);
  }
  return Word(compress(data));
}

void decode_formal_symbol(
    const Word& symbol,
    std::vector<int>& weights,
    std::vector<std::vector<int>>& points) {
  const auto input_chars = decompress(symbol.span());
  auto input_span = absl::MakeConstSpan(input_chars);
  weights = scan_vector(input_span);
  points.clear();
  while (!input_span.empty()) {
    points.push_back(scan_vector(input_span));
  }
}

std::string formal_symbol_to_string(const Word& symbol) {
  std::vector<int> weights;
  std::vector<std::vector<int>> points;
  decode_formal_symbol(symbol, weights, points);
  return lily_to_string(weights, points);
}
