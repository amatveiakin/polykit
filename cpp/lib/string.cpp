#include "string.h"

#include <sstream>


std::vector<std::string> split(const std::string& s, char delim) {
  std::vector<std::string> result;
  std::stringstream ss(s);
  std::string item;
  while (getline(ss, item, delim)) {
    result.push_back(item);
  }
  return result;
}

struct separate_thousands : std::numpunct<char> {
  char_type do_thousands_sep() const override { return '\''; }
  string_type do_grouping() const override { return "\3"; }  // groups size
};

std::string to_string_with_thousand_sep(int64_t value) {
  static std::locale loc(std::locale::classic(), new separate_thousands);
  std::stringstream ss;
  ss.imbue(loc);
  ss << value;
  return ss.str();
}

std::string en_plural(
    int number,
    const std::string& singular,
    const std::string& plural) {
  number = std::abs(number);
  const bool use_plural = number % 100 == 11 || number % 10 != 1;
  return use_plural
      ? (plural.empty() ? singular + "s" : plural)
      : singular;
}
