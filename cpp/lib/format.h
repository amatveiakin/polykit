#pragma once

#include <vector>

#include "string.h"


enum class HSpacing {
  dense,
  sparse,
};

class Formatter {
public:
  virtual ~Formatter() {}

  virtual std::string inf() = 0;
  virtual std::string unity() = 0;
  virtual std::string dot() = 0;
  virtual std::string tensor_prod() = 0;
  virtual std::string coprod_lie() = 0;
  virtual std::string coprod_hopf() = 0;
  virtual std::string comult() = 0;

  virtual std::string box(const std::string& expr) = 0;

  virtual std::string parens(const std::string& expr) = 0;
  virtual std::string brackets(const std::string& expr) = 0;
  virtual std::string braces(const std::string& expr) = 0;
  virtual std::string chevrons(const std::string& expr) = 0;

  virtual std::string coeff(int v) = 0;

  virtual std::string sub(
      const std::string& main,
      const std::vector<std::string>& indices) = 0;
  virtual std::string sub_num(
      const std::string& main,
      const std::vector<int>& indices);
  virtual std::string lrsub(
      const std::string& left_index,
      const std::string& main,
      const std::vector<std::string>& right_indices) = 0;
  virtual std::string lrsub_num(
      int left_index,
      const std::string& main,
      const std::vector<int>& right_indices);

  virtual std::string var(int idx) = 0;
  virtual std::string function(
      const std::string& name,
      const std::vector<std::string>& args,
      HSpacing hspacing) = 0;
  virtual std::string function_indexed_args(
      const std::string& name,
      const std::vector<int>& indices,
      HSpacing hspacing) = 0;
};

// TODO: Allow to dymanically change formatter and format options
extern Formatter* formatter;


namespace fmt {

inline std::string inf() { return formatter->inf(); }
inline std::string unity() { return formatter->unity(); }
inline std::string dot() { return formatter->dot(); }
inline std::string tensor_prod() { return formatter->tensor_prod(); }
inline std::string coprod_lie() { return formatter->coprod_lie(); }
inline std::string coprod_hopf() { return formatter->coprod_hopf(); }
inline std::string comult() { return formatter->comult(); }

inline std::string box(const std::string& expr) {
  return formatter->box(expr);
}
inline std::string parens(const std::string& expr) {
  return formatter->parens(expr);
}
inline std::string brackets(const std::string& expr) {
  return formatter->brackets(expr);
}
inline std::string braces(const std::string& expr) {
  return formatter->braces(expr);
}
inline std::string chevrons(const std::string& expr) {
  return formatter->chevrons(expr);
}

inline std::string coeff(int v) {
  return formatter->coeff(v);
}

inline std::string sub(
    const std::string& main,
    const std::vector<std::string>& indices) {
  return formatter->sub(main, indices);
}
inline std::string sub_num(
    const std::string& main,
    const std::vector<int>& indices) {
  return formatter->sub_num(main, indices);
}
inline std::string lrsub(
    const std::string& left_index,
    const std::string& main,
    const std::vector<std::string>& right_indices) {
  return formatter->lrsub(left_index, main, right_indices);
}
inline std::string lrsub_num(
    int left_index,
    const std::string& main,
    const std::vector<int>& right_indices) {
  return formatter->lrsub_num(left_index, main, right_indices);
}

inline std::string var(int idx) {
  return formatter->var(idx);
}
inline std::string function(
    const std::string& name,
    const std::vector<std::string>& args,
    HSpacing hspacing = HSpacing::dense) {
  return formatter->function(name, args, hspacing);
}
inline std::string function_indexed_args(
    const std::string& name,
    const std::vector<int>& indices,
    HSpacing hspacing = HSpacing::dense) {
  return formatter->function_indexed_args(name, indices, hspacing);
}

}  // namespace fmt
