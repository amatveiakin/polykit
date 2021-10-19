#pragma once

#include <iostream>
#include <limits>
#include <optional>
#include <vector>

#include "string.h"
#include "util.h"

#include "absl/strings/str_cat.h"


enum class Encoder {
  ascii,
  unicode,
  latex,
};

// Rich text support diagram:
//
//           |  Plain text   Native    Console     HTML
// ----------+------------------------------------------
//   ASCII   |      no        no         YES       YES
//  Unicode  |      no        no         YES       YES
//   LaTeX   |      no        YES        no        no
//
enum class RichTextFormat {
  native,
  plain_text,
  console,
  html,
};

// For now the criteria are:
//   - `simple` means it is rendered by Windows 10 terminal with "DejaVu Sans Mono" font.
//   - `full` is everything else; but still trying to stay within things supported by modern browsers.
enum class UnicodeVersion {
  simple,
  full,
};

enum class AnnotationSorting {
  lexicographic,
  length,
};

// Defines how expressions are printed. nullopt is used for overriding
// and means "keep as is". It is guaranteed that in the config returned
// by `current_formatting_config()` all values are non-null.
struct FormattingConfig {
  static constexpr int kNoLineLimit = std::numeric_limits<int>::max();

  std::optional<Encoder> encoder;
  std::optional<RichTextFormat> rich_text_format;
  std::optional<UnicodeVersion> unicode_version;
  std::optional<AnnotationSorting> annotation_sorting;
  std::optional<int> expression_line_limit;
  std::optional<bool> expression_include_annotations;
  std::optional<bool> parsable_expression;
  std::optional<bool> compact_expression;
  std::optional<bool> compact_x;
  std::optional<bool> new_line_after_expression;

  FormattingConfig& set_encoder(Encoder v) { encoder = v; return *this; }
  FormattingConfig& set_rich_text_format(RichTextFormat v) { rich_text_format = v; return *this; }
  FormattingConfig& set_unicode_version(UnicodeVersion v) { unicode_version = v; return *this; }
  FormattingConfig& set_annotation_sorting(AnnotationSorting v) { annotation_sorting = v; return *this; }
  FormattingConfig& set_expression_line_limit(int v) { expression_line_limit = v; return *this; }
  FormattingConfig& set_expression_include_annotations(bool v) { expression_include_annotations = v; return *this; }
  FormattingConfig& set_parsable_expression(bool v) { parsable_expression = v; return *this; }
  FormattingConfig& set_compact_expression(bool v) { compact_expression = v; return *this; }
  FormattingConfig& set_compact_x(bool v) { compact_x = v; return *this; }
  FormattingConfig& set_new_line_after_expression(bool v) { new_line_after_expression = v; return *this; }

  void apply_overrides(const FormattingConfig& src);
};

FormattingConfig current_formatting_config();

class ScopedFormatting {
public:
  ScopedFormatting(FormattingConfig config);
  ~ScopedFormatting();
};

// Changes default formatting config. This function exists for the sake of Python bindings.
// Prefer `ScopedFormatting` in C++ code.
// Note: has lower priority that `ScopedFormatting`, even if applied afterwards!
void set_default_formatting(const FormattingConfig& config);
void reset_default_formatting();


// Console assumes white-on-black, so "pale" colors are darker.
// HTML and LaTeX assume black-on-white, so "pale" colors are brighter.
//
// When using colors in LaTeX add
//    \usepackage[dvipsnames]{xcolor}
// to the preambule.
enum class TextColor {
  normal,
  red,
  green,
  yellow,
  blue,
  magenta,
  cyan,
  orange,
  pale_red,
  pale_green,
  pale_blue,
  pale_magenta,
  pale_cyan,
};

// Note. It would make sense to add std::optional-s similarly to FormattingConfig
// when there are more rich text options.
struct RichTextOptions {
  TextColor text_color = TextColor::normal;

  RichTextOptions& set_text_color(TextColor v) { text_color = v; return *this; }
};

class ScopedRichTextOptions {
public:
  ScopedRichTextOptions(std::ostream& os, const RichTextOptions& options);
  ~ScopedRichTextOptions();

private:
  std::ostream* stream;
};


enum class HSpacing {
  dense,
  sparse,
};

class AbstractEncoder {
public:
  virtual ~AbstractEncoder() {}

  virtual std::string newline() = 0;
  virtual std::string inf() = 0;
  virtual std::string unity();
  virtual std::string minus();
  virtual std::string dot() = 0;
  virtual std::string tensor_prod() = 0;
  virtual std::string coprod_normal() = 0;
  virtual std::string coprod_iterated() = 0;
  virtual std::string coprod_hopf() = 0;
  virtual std::string comult() = 0;
  virtual std::string set_union() = 0;
  virtual std::string set_intersection() = 0;
  virtual std::string set_complement() = 0;

  virtual std::string sum(
      const std::string& lhs,
      const std::string& rhs,
      HSpacing hspacing) = 0;
  virtual std::string diff(
      const std::string& lhs,
      const std::string& rhs,
      HSpacing hspacing) = 0;
  virtual std::string frac(
      const std::string& numerator,
      const std::string& denominator) = 0;
  // Note: there is no function for product. Simply use an str_join
  // with one of the product signs above.

  virtual std::string parens(const std::string& expr) = 0;
  virtual std::string brackets(const std::string& expr) = 0;
  virtual std::string braces(const std::string& expr) = 0;
  virtual std::string chevrons(const std::string& expr) = 0;
  virtual std::string frac_parens(const std::string& expr) = 0;  // adds parens iff `fraq` is one-liner

  virtual std::string num(int v) = 0;
  virtual std::string coeff(int v) = 0;

  virtual std::string sub(
      const std::string& main,
      const std::vector<std::string>& indices) = 0;
  virtual std::string lrsub(
      const std::string& left_index,
      const std::string& main,
      const std::vector<std::string>& right_indices) = 0;
  virtual std::string super(
      const std::string& main,
      const std::vector<std::string>& indices) = 0;

  virtual std::string mathcal(const std::string& str) = 0;
  virtual std::string mathbb(const std::string& str) = 0;

  virtual std::string var(int idx) = 0;
  virtual std::string opname(const std::string& name);
  virtual std::string function(
      const std::string& name,
      const std::vector<std::string>& args,
      HSpacing hspacing) = 0;

  // Usage:
  //   * Each begin_rich_text call must be followed by exactly one end_rich_text call.
  //   * Calls to begin_rich_text/end_rich_text can be nested, e.g. begin, begin, end, end.
  //   * RichTextFormat must stay the same as long as there are open rich text segments.
  //   * Encoder must stay the same as long as there are open rich text segments.
  // Warning: Never write `absl::StrCat(begin_rich_text, ..., end_rich_text)`,
  // as function parameter evaluation order is unspecified in C++. In contrast,
  // `begin_rich_text + ... + end_rich_text` is fine (since C++17).
  virtual std::string begin_rich_text(const RichTextOptions& options);
  virtual std::string end_rich_text();
};

AbstractEncoder* current_encoder();


namespace fmt {

inline std::string newline() { return current_encoder()->newline(); }
inline std::string inf() { return current_encoder()->inf(); }
inline std::string unity() { return current_encoder()->unity(); }
inline std::string minus() { return current_encoder()->minus(); }
inline std::string dot() { return current_encoder()->dot(); }
inline std::string tensor_prod() { return current_encoder()->tensor_prod(); }
inline std::string coprod_normal() { return current_encoder()->coprod_normal(); }
inline std::string coprod_iterated() { return current_encoder()->coprod_iterated(); }
inline std::string coprod_hopf() { return current_encoder()->coprod_hopf(); }
inline std::string comult() { return current_encoder()->comult(); }
inline std::string set_union() { return current_encoder()->set_union(); }
inline std::string set_intersection() { return current_encoder()->set_intersection(); }
inline std::string set_complement() { return current_encoder()->set_complement(); }

inline std::string sum(
    const std::string& lhs,
    const std::string& rhs,
    HSpacing hspacing = HSpacing::sparse) {
  return current_encoder()->sum(lhs, rhs, hspacing);
}
inline std::string diff(
    const std::string& lhs,
    const std::string& rhs,
    HSpacing hspacing = HSpacing::sparse) {
  return current_encoder()->diff(lhs, rhs, hspacing);
}
inline std::string frac(
    const std::string& numerator,
    const std::string& denominator) {
  return current_encoder()->frac(numerator, denominator);
}

inline std::string parens(const std::string& expr) {
  return current_encoder()->parens(expr);
}
inline std::string brackets(const std::string& expr) {
  return current_encoder()->brackets(expr);
}
inline std::string braces(const std::string& expr) {
  return current_encoder()->braces(expr);
}
inline std::string chevrons(const std::string& expr) {
  return current_encoder()->chevrons(expr);
}
inline std::string frac_parens(const std::string& expr) {
  return current_encoder()->frac_parens(expr);
}

inline std::string num(int v) {
  return current_encoder()->num(v);
}
inline std::string coeff(int v) {
  return current_encoder()->coeff(v);
}

inline std::string sub(
    const std::string& main,
    const std::vector<std::string>& indices) {
  return current_encoder()->sub(main, indices);
}
inline std::string sub_num(
    const std::string& main,
    absl::Span<const int> indices) {
  return sub(main, mapped_to_string(indices));
}
inline std::string lrsub(
    const std::string& left_index,
    const std::string& main,
    const std::vector<std::string>& right_indices) {
  return current_encoder()->lrsub(left_index, main, right_indices);
}
inline std::string lrsub_num(
    int left_index,
    const std::string& main,
    absl::Span<const int> right_indices) {
  return lrsub(to_string(left_index), main, mapped_to_string(right_indices));
}
inline std::string super(
    const std::string& main,
    const std::vector<std::string>& indices) {
  return current_encoder()->super(main, indices);
}
inline std::string super_num(
    const std::string& main,
    absl::Span<const int> indices) {
  return super(main, mapped_to_string(indices));
}

inline std::string mathcal(const std::string& str) {
  return current_encoder()->mathcal(str);
}
inline std::string mathbb(const std::string& str) {
  return current_encoder()->mathbb(str);
}

inline std::string var(int idx) {
  return current_encoder()->var(idx);
}
inline std::string opname(const std::string& name) {
  return current_encoder()->opname(name);
}
inline std::string function(
    const std::string& name,
    const std::vector<std::string>& args,
    HSpacing hspacing = HSpacing::dense) {
  return current_encoder()->function(name, args, hspacing);
}

// Warning: nested `fmt::colored` calls are not supported with RichTextFormat::console.
// Use begin_rich_text/end_rich_text for everything except the innermost color.
inline std::string colored(const std::string& expr, TextColor text_color) {
  std::string ret;
  ret += current_encoder()->begin_rich_text(RichTextOptions().set_text_color(text_color));
  ret += expr;
  ret += current_encoder()->end_rich_text();
  return ret;
}

}  // namespace fmt
