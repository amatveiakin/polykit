#pragma once

#include <limits>
#include <optional>
#include <vector>

#include "string.h"


enum class Formatter {
  plain_text,
  unicode,
  latex,
};

// Defines how expressions are printed. nullopt is used for overriding
// and means "keep as is". It is guaranteed that in the config returned
// by `current_formatting_config()` all values are non-null.
struct FormattingConfig {
  static constexpr int kNoLineLimit = std::numeric_limits<int>::max();

  std::optional<Formatter> formatter;
  std::optional<bool> html_mode;  // only for `unicode` formatter
  std::optional<int> expression_line_limit;
  std::optional<bool> expression_include_annotations;
  std::optional<bool> parsable_expression;
  std::optional<bool> compact_expression;

  FormattingConfig& set_formatter(Formatter v) { formatter = v; return *this; }
  FormattingConfig& set_html_mode(bool v) { html_mode = v; return *this; }
  FormattingConfig& set_expression_line_limit(int v) { expression_line_limit = v; return *this; }
  FormattingConfig& set_expression_include_annotations(bool v) { expression_include_annotations = v; return *this; }
  FormattingConfig& set_parsable_expression(bool v) { parsable_expression = v; return *this; }
  FormattingConfig& set_compact_expression(bool v) { compact_expression = v; return *this; }

  void apply_overrides(const FormattingConfig& src);
};

FormattingConfig current_formatting_config();

struct ScopedFormatting {
  ScopedFormatting(FormattingConfig config);
  ~ScopedFormatting();
};


enum class HSpacing {
  dense,
  sparse,
};

class AbstractFormatter {
public:
  virtual ~AbstractFormatter() {}

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

AbstractFormatter* current_formatter();


namespace fmt {

inline std::string inf() { return current_formatter()->inf(); }
inline std::string unity() { return current_formatter()->unity(); }
inline std::string dot() { return current_formatter()->dot(); }
inline std::string tensor_prod() { return current_formatter()->tensor_prod(); }
inline std::string coprod_lie() { return current_formatter()->coprod_lie(); }
inline std::string coprod_hopf() { return current_formatter()->coprod_hopf(); }
inline std::string comult() { return current_formatter()->comult(); }

inline std::string box(const std::string& expr) {
  return current_formatter()->box(expr);
}
inline std::string parens(const std::string& expr) {
  return current_formatter()->parens(expr);
}
inline std::string brackets(const std::string& expr) {
  return current_formatter()->brackets(expr);
}
inline std::string braces(const std::string& expr) {
  return current_formatter()->braces(expr);
}
inline std::string chevrons(const std::string& expr) {
  return current_formatter()->chevrons(expr);
}

inline std::string coeff(int v) {
  return current_formatter()->coeff(v);
}

inline std::string sub(
    const std::string& main,
    const std::vector<std::string>& indices) {
  return current_formatter()->sub(main, indices);
}
inline std::string sub_num(
    const std::string& main,
    const std::vector<int>& indices) {
  return current_formatter()->sub_num(main, indices);
}
inline std::string lrsub(
    const std::string& left_index,
    const std::string& main,
    const std::vector<std::string>& right_indices) {
  return current_formatter()->lrsub(left_index, main, right_indices);
}
inline std::string lrsub_num(
    int left_index,
    const std::string& main,
    const std::vector<int>& right_indices) {
  return current_formatter()->lrsub_num(left_index, main, right_indices);
}

inline std::string var(int idx) {
  return current_formatter()->var(idx);
}
inline std::string function(
    const std::string& name,
    const std::vector<std::string>& args,
    HSpacing hspacing = HSpacing::dense) {
  return current_formatter()->function(name, args, hspacing);
}
inline std::string function_indexed_args(
    const std::string& name,
    const std::vector<int>& indices,
    HSpacing hspacing = HSpacing::dense) {
  return current_formatter()->function_indexed_args(name, indices, hspacing);
}

}  // namespace fmt
