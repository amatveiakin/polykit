#pragma once

#include <string>

#include "format_basic.h"


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

template<typename Arg>
std::string function_to_string(const std::string& name, const std::vector<Arg>& args) {
  return name + "(" + str_join(args, ",") + ")";
}


inline std::string pad_left(std::string str, int length, char padding = ' ') {
  return std::string(std::max(0, length - static_cast<int>(str.length())), padding) +
      std::move(str);
}


inline std::string format_coeff(int coeff) {
  if (coeff == 0) {
    return " 0 ";
  } else if (coeff == 1) {
    return " + ";
  } else if (coeff == -1) {
    return " - ";
  } else if (coeff > 0) {
    return "+" + to_string(coeff) + " ";
  } else {
    return to_string(coeff) + " ";
  }
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
