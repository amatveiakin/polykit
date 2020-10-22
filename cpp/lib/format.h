#pragma once

#include <string>


// For generic programming like str_join
inline std::string to_string(int x) { return std::to_string(x); }


// TODO: Rewrite via absl::StrJoin
template<typename T, typename F>
std::string str_join(const T& container, std::string separator, F element_to_string) {
  std::string ret;
  for (const auto v : container) {
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
  for (const auto v : container) {
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
