#pragma once

#include <string>

#include "string_basic.h"


// TODO: Rewrite via absl::StrJoin
template<typename T, typename F>
std::string str_join(const T& container, std::string separator, F element_to_string) {
  std::string ret;
  for (const auto& v : container) {
    if (!ret.empty()) {
      ret += separator;
    }
    ret += element_to_string(v);
  }
  return ret;
}

template<typename T>
std::string str_join(const T& container, std::string separator) {
  std::string ret;
  for (const auto& v : container) {
    if (!ret.empty()) {
      ret += separator;
    }
    ret += to_string(v);
  }
  return ret;
}

template<typename T, typename F>
std::string list_to_string(T container, F element_to_string) {
  return "(" + str_join(container, ", ", element_to_string) + ")";
}

template<typename T>
std::string list_to_string(T container) {
  return "(" + str_join(container, ", ") + ")";
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


inline std::string pad_left(std::string str, int length, char padding = ' ') {
  return std::string(std::max(0, length - static_cast<int>(str.length())), padding) +
      std::move(str);
}


inline std::string en_plural(
    int number,
    const std::string& singular,
    const std::string& plural = {}) {
  number = std::abs(number);
  const bool use_plural = number % 100 == 11 || number % 10 != 1;
  return use_plural
      ? (plural.empty() ? singular + "s" : plural)
      : singular;
}
