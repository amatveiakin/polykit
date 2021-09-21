#pragma once

#include <algorithm>
#include <string>

#include "check.h"
#include "string_basic.h"


enum class TextAlignment {
  left,
  center,
  right
};


// Returns the number of code points in a UTF-8 encoded string.
// Does not check whether the string is valid.
// Note. A "proper" solution would be to use `std::mbrlen`, but it requires `std::setlocale`.
inline int strlen_utf8(std::string_view s) {
  int len = 0;
  for (int i = 0; i < s.size(); ++i) {
    len += (s[i] & 0xc0) != 0x80;
  }
  return len;
}


inline void trim_left(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
}

inline void trim_right(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

inline void trim(std::string &s) {
  trim_left(s);
  trim_right(s);
}

inline std::string trimed_left(std::string s) {
  trim_left(s);
  return s;
}

inline std::string trimed_right(std::string s) {
  trim_right(s);
  return s;
}

inline std::string trimed(std::string s) {
  trim(s);
  return s;
}


inline std::string pad_left(std::string str, int width, char padding = ' ') {
  return std::string(std::max(0, width - strlen_utf8(str)), padding) +
      std::move(str);
}

inline std::string pad_right(std::string str, int width, char padding = ' ') {
  return std::move(str) +
      std::string(std::max(0, width - strlen_utf8(str)), padding);
}

inline std::string pad_twosided(std::string str, int width, char padding = ' ') {
  const int padding_width = std::max(0, width - strlen_utf8(str));
  return std::string(padding_width / 2, padding) +
      std::move(str) +
      std::string((padding_width + 1) / 2, padding);
}

inline std::string pad_string(TextAlignment alignment, std::string str, int width, char padding = ' ') {
  switch (alignment) {
    case TextAlignment::left: return pad_right(std::move(str), width, padding);
    case TextAlignment::center: return pad_twosided(std::move(str), width, padding);
    case TextAlignment::right: return pad_left(std::move(str), width, padding);
  }
  FATAL(absl::StrCat("Invalid alignment: ", alignment));
}


std::string to_string_with_thousand_sep(int64_t value);

std::string en_plural(
    int number,
    const std::string& singular,
    const std::string& plural = {});
