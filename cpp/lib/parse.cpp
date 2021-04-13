#include "parse.h"

#include <codecvt>
#include <locale>

#include "absl/strings/str_cat.h"

#include "check.h"


static ParserRegex whitespace_re(R"(\s+)");
static ParserRegex integer_re(R"([0-9]+)");


// TODO: Support Unicode (e.g. Unioncode minus and tensor signs); maybe you these:
//   static std::wstring utf8_to_wstring(const std::string& s) { return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(s); }
//   static std::string wstring_to_utf8(const std::wstring& s) { return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(s); }

bool peek(const ParserState& st, const ParserRegex& re) {
  const auto s = st.view();
  return std::regex_search(s.begin(), s.end(), re.re(), std::regex_constants::match_continuous);
}

static std::string_view try_consume_impl(ParserState& st, const ParserRegex& re) {
  auto& s = st.view();
  std::match_results<std::string_view::const_iterator> match;
  if (std::regex_search(s.begin(), s.end(), match, re.re(), std::regex_constants::match_continuous)) {
    CHECK(match[0].first == s.begin());
    const size_t len = match[0].length();
    const auto ret = s.substr(0, len);
    s = s.substr(len);
    return ret;
  }
  return {};
}

static void consume_whitespace(ParserState& st) {
  try_consume_impl(st, whitespace_re);
}

std::string_view try_consume(ParserState& st, const ParserRegex& re) {
  auto ret = try_consume_impl(st, re);
  consume_whitespace(st);
  return ret;
}

static std::string trim_for_error_message(std::string_view s) {
  constexpr int kMaxLength = 100;
  return s.size() <= kMaxLength ? std::string(s) : absl::StrCat(s.substr(0, kMaxLength), "...");
}

std::string_view consume(ParserState& st, const ParserRegex& re) {
  auto match = try_consume(st, re);
  CHECK(!match.empty()) << re.str() << " not found in " << trim_for_error_message(st.view());
  return match;
}

static int to_int(std::string_view s) {
  return stoi(std::string(s));
}

std::optional<int> try_consume_integer(ParserState& st) {
  const auto integer_str = try_consume(st, integer_re);
  return integer_str.empty() ? std::nullopt : std::optional(to_int(integer_str));
}

int consume_integer(ParserState& st) {
  return to_int(consume(st, integer_re));
}


ParserState::ParserState(const std::string* s) : view_(*s) {
  consume_whitespace(*this);
}
