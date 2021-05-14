#pragma once

#include <algorithm>
#include <string>

#include "string_basic.h"


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


inline std::string pad_left(std::string str, int length, char padding = ' ') {
  return std::string(std::max(0, length - static_cast<int>(str.length())), padding) +
      std::move(str);
}


std::string to_string_with_thousand_sep(int64_t value);

std::string en_plural(
    int number,
    const std::string& singular,
    const std::string& plural = {});
