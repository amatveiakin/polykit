// TODO: Replace `virtual`-s with `override`-s

#include "format.h"

#include <regex>

#include "absl/strings/str_cat.h"
#include "absl/strings/substitute.h"

#include "util.h"


static const FormattingConfig default_formatting_config = FormattingConfig()
  .set_formatter(Formatter::ascii)
  .set_rich_text_format(RichTextFormat::native)
  .set_annotation_sorting(AnnotationSorting::lexicographic)
  .set_expression_line_limit(100)
  .set_expression_include_annotations(true)
  .set_parsable_expression(false)
  .set_compact_expression(false)
;


static thread_local std::vector<FormattingConfig> formatting_config_stack;
static thread_local FormattingConfig aggregated_formatting_config = default_formatting_config;

// This is marked as thread-local to avoid nasty race consitions, but
// in reality only writing to console from one thread is supported.
static thread_local std::vector<RichTextOptions> console_rich_text_options_stack;

std::string get_command_for_console_rich_text_options() {
  const RichTextOptions& options = console_rich_text_options_stack.empty()
    ? RichTextOptions()
    : console_rich_text_options_stack.back();
  return options.text_color == TextColor::normal
    ? "\033[0m"
    : absl::StrCat("\033[", static_cast<int>(options.text_color), "m");
}

// Note: color brightness is inverted, because console is white-on-black
// and html is black-on-white.
std::string text_color_to_html_color(TextColor color) {
  switch (color) {
    case TextColor::normal:         return "Black";
    case TextColor::red:            return "LightCoral";
    case TextColor::green:          return "LightGreen";
    case TextColor::yellow:         return "Khaki";
    case TextColor::blue:           return "DeepSkyBlue";
    case TextColor::magenta:        return "Violet";
    case TextColor::cyan:           return "MediumAquamarine";
    case TextColor::bright_red:     return "Red";
    case TextColor::bright_green:   return "LimeGreen";
    case TextColor::bright_yellow:  return "Gold";
    case TextColor::bright_blue:    return "Blue";
    case TextColor::bright_magenta: return "Magenta";
    case TextColor::bright_cyan:    return "DarkCyan";
  }
  FATAL(absl::StrCat("Unknown color: ", color));
}

std::string rich_text_options_to_css(const RichTextOptions& options) {
  return "color: " + text_color_to_html_color(options.text_color);
}


FormattingConfig current_formatting_config() {
  return aggregated_formatting_config;
}

template<typename T>
static void apply_field_override(
    std::optional<T>& dst, const std::optional<T>& src) {
  if (src.has_value()) {
    dst = src;
  }
}

void FormattingConfig::apply_overrides(const FormattingConfig& src) {
  apply_field_override(formatter, src.formatter);
  apply_field_override(rich_text_format, src.rich_text_format);
  apply_field_override(annotation_sorting, src.annotation_sorting);
  apply_field_override(expression_line_limit, src.expression_line_limit);
  apply_field_override(expression_include_annotations, src.expression_include_annotations);
  apply_field_override(parsable_expression, src.parsable_expression);
  apply_field_override(compact_expression, src.compact_expression);
}

static void recompute_formatting_config() {
  aggregated_formatting_config = default_formatting_config;
  for (const auto& config : formatting_config_stack) {
    aggregated_formatting_config.apply_overrides(config);
  }
}

ScopedFormatting::ScopedFormatting(FormattingConfig config) {
  formatting_config_stack.push_back(config);
  recompute_formatting_config();
}

ScopedFormatting::~ScopedFormatting() {
  formatting_config_stack.pop_back();
  recompute_formatting_config();
}

ScopedRichTextOptions::ScopedRichTextOptions(std::ostream& os, const RichTextOptions& options) {
  stream = &os;
  (*stream) << current_formatter()->begin_rich_text(options);
}

ScopedRichTextOptions::~ScopedRichTextOptions() {
  (*stream) << current_formatter()->end_rich_text();
}


static std::vector<std::string> ints_to_strings(const std::vector<int> v) {
  return mapped(v, [](int x){ return absl::StrCat(x); });
}

static bool is_html() {
  return *current_formatting_config().rich_text_format == RichTextFormat::html;
}

static std::string maybe_html_newline() {
  return is_html() ? " <br>\n" : "\n";
}


std::string AbstractFormatter::unity() { return fmt::colored(chevrons("1"), TextColor::yellow); }
std::string AbstractFormatter::minus() { return "-"; }

std::string AbstractFormatter::sub_num(const std::string& main, const std::vector<int>& indices) {
  return sub(main, ints_to_strings(indices));
}
std::string AbstractFormatter::super_num(const std::string& main, const std::vector<int>& indices) {
  return super(main, ints_to_strings(indices));
}
std::string AbstractFormatter::lrsub_num(int left_index, const std::string& main, const std::vector<int>& right_indices) {
  return lrsub(absl::StrCat(left_index), main, ints_to_strings(right_indices));
}

std::string AbstractFormatter::begin_rich_text(const RichTextOptions& options) {
  switch (*current_formatting_config().rich_text_format) {
    case RichTextFormat::native:
    case RichTextFormat::plain_text:
      return {};
    case RichTextFormat::console:
      console_rich_text_options_stack.push_back(options);
      return get_command_for_console_rich_text_options();
    case RichTextFormat::html:
      return absl::Substitute(R"(<span style="$0">)", rich_text_options_to_css(options));
  }
  FATAL("Illegal rich_text_format");
}

std::string AbstractFormatter::end_rich_text() {
  switch (*current_formatting_config().rich_text_format) {
    case RichTextFormat::native:
    case RichTextFormat::plain_text:
      return {};
    case RichTextFormat::console:
      console_rich_text_options_stack.pop_back();
      return get_command_for_console_rich_text_options();
    case RichTextFormat::html:
      return absl::Substitute("</span>");
  }
  FATAL("Illegal rich_text_format");
}


class AsciiFormatter : public AbstractFormatter {
  virtual std::string newline() { return maybe_html_newline(); }
  virtual std::string inf() { return "Inf"; }
  virtual std::string dot() { return "."; }
  virtual std::string tensor_prod() { return " * "; }
  virtual std::string coprod_lie() { return "  ^  "; }
  virtual std::string coprod_hopf() { return "  @  "; }
  virtual std::string comult() { return is_html() ? "&amp;" : "&"; }

  virtual std::string sum(const std::string& lhs, const std::string& rhs, HSpacing hspacing) {
    const std::string spacing = (hspacing == HSpacing::dense ? "" : " ");
    return absl::StrCat(lhs, spacing, "+", spacing, rhs);
  }
  virtual std::string diff(const std::string& lhs, const std::string& rhs, HSpacing hspacing) {
    const std::string spacing = (hspacing == HSpacing::dense ? "" : " ");
    return absl::StrCat(lhs, spacing, "-", spacing, rhs);
  }
  virtual std::string frac(const std::string& numerator, const std::string& denominator) {
    return absl::StrCat(numerator, "/", denominator);
  }

  virtual std::string parens(const std::string& expr) {
    return absl::StrCat("(", expr, ")");
  }
  virtual std::string brackets(const std::string& expr) {
    return absl::StrCat("[", expr, "]");
  }
  virtual std::string braces(const std::string& expr) {
    return absl::StrCat("{", expr, "}");
  }
  virtual std::string chevrons(const std::string& expr) {
    return is_html()
      ? absl::StrCat("&lt;", expr, "&gt;")
      : absl::StrCat("<", expr, ">");
  }
  virtual std::string frac_parens(const std::string& expr) {
    return parens(expr);
  }

  virtual std::string coeff(int v) {
    if (*current_formatting_config().parsable_expression) {
      // Allows to copy annotations from the output and use them in code.
      // Should not start with whitespace, because that would mess up identation.
      // Note: not using `tensor_prod()` here, because logically this is not
      // tensor product.
      if      (v == 0)  { return "0 *"; }
      else if (v == 1)  { return "+  "; }
      else if (v == -1) { return "-  "; }
      else if (v > 0)   { return absl::StrCat("+", v, "*"); }
      else              { return absl::StrCat(v, "*"); }
    } else if (*current_formatting_config().compact_expression) {
      if      (v == 0)  { return "0 "; }
      else if (v == 1)  { return ""; }
      else if (v == -1) { return "-"; }
      else if (v > 0)   { return absl::StrCat(v, " "); }
      else              { return absl::StrCat(v, " "); }
    } else {
      if      (v == 0)  { return " 0 "; }
      else if (v == 1)  { return " + "; }
      else if (v == -1) { return " - "; }
      else if (v > 0)   { return absl::StrCat("+", v, " "); }
      else              { return absl::StrCat(v, " "); }
    }
  }

  virtual std::string sub(const std::string& main, const std::vector<std::string>& indices) {
    CHECK(!main.empty());
    return indices.empty() ? main : absl::StrCat(main, "_", str_join(indices, "_"));
  }
  virtual std::string lrsub(const std::string& left_index, const std::string& main, const std::vector<std::string>& right_indices) {
    CHECK(!main.empty());
    return absl::StrCat(
      left_index.empty() ? "" : absl::StrCat(left_index, "_"),
      main,
      right_indices.empty() ? "" : absl::StrCat("_", str_join(right_indices, "_"))
    );
  }

  virtual std::string super(const std::string& main, const std::vector<std::string>& indices) {
    CHECK(!main.empty());
    return indices.empty()
      ? main
      : indices.size() == 1
        ? absl::StrCat(main, indices.front())
        : absl::StrCat(main, "^", str_join(indices, "^"));
  }

  virtual std::string var(int idx) {
    return absl::StrCat("x", idx);
  }
  virtual std::string function(const std::string& name, const std::vector<std::string>& args, HSpacing hspacing) {
    const auto separator = (hspacing == HSpacing::dense ? "," : ", ");
    return name + parens(str_join(args, separator));
  }
  virtual std::string function_indexed_args(const std::string& name, const std::vector<int>& indices, HSpacing hspacing) {
    return function(name, ints_to_strings(indices), hspacing);
  }
};


class UnicodeFormatter : public AbstractFormatter {
  static constexpr char kNbsp[] = " ";
  // static constexpr char kThinSpace[] = " ";
  static constexpr char kThinNbsp[] = " ";
  static constexpr char kMinusSign[] = "−";

  virtual std::string newline() { return maybe_html_newline(); }
  virtual std::string inf() { return "∞"; }
  virtual std::string dot() { return "⋅"; }
  virtual std::string minus() { return kMinusSign; }
  virtual std::string tensor_prod() { return "⊗"; }
  virtual std::string coprod_lie() { return hspace("∧"); }
  virtual std::string coprod_hopf() { return hspace("☒"); }
  virtual std::string comult() { return "△"; }

  virtual std::string sum(const std::string& lhs, const std::string& rhs, HSpacing hspacing) {
    const std::string spacing = (hspacing == HSpacing::dense ? "" : kNbsp);
    return absl::StrCat(lhs, spacing, "+", spacing, rhs);
  }
  virtual std::string diff(const std::string& lhs, const std::string& rhs, HSpacing hspacing) {
    const std::string spacing = (hspacing == HSpacing::dense ? "" : kNbsp);
    return absl::StrCat(lhs, spacing, kMinusSign, spacing, rhs);
  }
  virtual std::string frac(const std::string& numerator, const std::string& denominator) {
    return absl::StrCat(numerator, "/", denominator);
  }

  std::string hspace(const std::string& expr) {
    return absl::StrCat(kNbsp, expr, kNbsp);
  }

  virtual std::string parens(const std::string& expr) {
    return absl::StrCat("(", expr, ")");
  }
  virtual std::string brackets(const std::string& expr) {
    return absl::StrCat("[", expr, "]");
  }
  virtual std::string braces(const std::string& expr) {
    return absl::StrCat("{", expr, "}");
  }
  virtual std::string chevrons(const std::string& expr) {
    return absl::StrCat("⟨", expr, "⟩");
  }
  virtual std::string frac_parens(const std::string& expr) {
    return parens(expr);
  }

  // TODO: Check how well std::regex_replace actually supports unicode.
  // TODO: Fix all minuses! Should this functions be used more often?
  std::string fix_minus(const std::string& expr) {
    return std::regex_replace(expr, std::regex("-"), kMinusSign);
  }
  // std::string number(int v) {
  //   return fix_minus(absl::StrCat(v));
  // }

  virtual std::string coeff(int v) {
    if (*current_formatting_config().compact_expression) {
      if      (v == 0)  { return "0"; }
      else if (v == 1)  { return ""; }
      else if (v == -1) { return kMinusSign; }
      else if (v > 0)   { return absl::StrCat(v); }
      else              { return fix_minus(absl::StrCat(v)); }
    } else {
      // TODO: Use using "figure space" (U+2007) here.
      if      (v == 0)  { return absl::StrCat(" 0", kThinNbsp); }
      else if (v == 1)  { return absl::StrCat(" +", kThinNbsp); }
      else if (v == -1) { return absl::StrCat(" ", kMinusSign, kThinNbsp); }
      else if (v > 0)   { return absl::StrCat("+", v, kThinNbsp); }
      else              { return fix_minus(absl::StrCat(v, kThinNbsp)); }
    }
  }

  static std::string char_to_subscript(char ch) {
    switch (ch) {
      case '+': return "₊";
      case '-': return "₋";
      case '0': return "₀";
      case '1': return "₁";
      case '2': return "₂";
      case '3': return "₃";
      case '4': return "₄";
      case '5': return "₅";
      case '6': return "₆";
      case '7': return "₇";
      case '8': return "₈";
      case '9': return "₉";
    }
    FATAL(absl::StrCat("There is no known subscript for '", std::string(1, ch), "'"));
  }
  static std::string char_to_superscript(char ch) {
    switch (ch) {
      case '+': return "⁺";
      case '-': return "⁻";
      case '0': return "⁰";
      case '1': return "¹";
      case '2': return "²";
      case '3': return "³";
      case '4': return "⁴";
      case '5': return "⁵";
      case '6': return "⁶";
      case '7': return "⁷";
      case '8': return "⁸";
      case '9': return "⁹";
    }
    FATAL(absl::StrCat("There is no known superscript for '", std::string(1, ch), "'"));
  }
  static std::string string_to_subscript(const std::string& str) {
    CHECK_EQ(str.size(), 1) << str
      << "Unicode formatter doesn't support multi-character subscripts: there are "
      << "no subscript commas in Unicode, so there is no way to separate the indices.";
    std::string ret;
    for (const char ch : str) {
      ret += char_to_subscript(ch);
    }
    return ret;
  }
  static std::string string_to_superscript(const std::string& str) {
    CHECK_EQ(str.size(), 1) << str
      << "Unicode formatter doesn't support multi-character superscripts: there are "
      << "no superscript commas in Unicode, so there is no way to separate the indices.";
    std::string ret;
    for (const char ch : str) {
      ret += char_to_superscript(ch);
    }
    return ret;
  }

  virtual std::string sub(const std::string& main, const std::vector<std::string>& indices) {
    CHECK(!main.empty());
    return absl::StrCat(main, str_join(indices, "", string_to_subscript));
  }
  virtual std::string lrsub(const std::string& left_index, const std::string& main, const std::vector<std::string>& right_indices) {
    CHECK(!main.empty());
    return absl::StrCat(
      string_to_subscript(left_index),
      main,
      str_join(right_indices, "", string_to_subscript)
    );
  }

  virtual std::string super(const std::string& main, const std::vector<std::string>& indices) {
    CHECK(!main.empty());
    return absl::StrCat(main, str_join(indices, "", string_to_superscript));
  }

  virtual std::string var(int idx) {
    return sub_num("x", {idx});
  }
  virtual std::string function(const std::string& name, const std::vector<std::string>& args, HSpacing hspacing) {
    const auto separator = (hspacing == HSpacing::dense ? "," : absl::StrCat(",", kThinNbsp));
    return name + parens(str_join(args, separator));
  }
  virtual std::string function_indexed_args(const std::string& name, const std::vector<int>& indices, HSpacing hspacing) {
    return function(name, ints_to_strings(indices), hspacing);
  }
};


class LatexFormatter : public AbstractFormatter {
  virtual std::string newline() { return "\\\\\n"; }
  // TODO: Make sure all op signs are in fact math ops from Latex point of view
  virtual std::string inf() { return "\\infty"; }
  virtual std::string dot() { return ""; }
  virtual std::string tensor_prod() { return " \\otimes "; }
  virtual std::string coprod_lie() { return hspace("\\wedge"); }
  virtual std::string coprod_hopf() { return hspace("\\boxtimes"); }
  virtual std::string comult() { return " \\triangle "; }

  virtual std::string sum(const std::string& lhs, const std::string& rhs, HSpacing) {
    return absl::StrCat(lhs, "+", rhs);
  }
  virtual std::string diff(const std::string& lhs, const std::string& rhs, HSpacing) {
    return absl::StrCat(lhs, "-", rhs);
  }
  virtual std::string frac(const std::string& numerator, const std::string& denominator) {
    return absl::StrCat("\\frac{", numerator, "}{", denominator, "}");
  }

  std::string hspace(const std::string& expr) {
    return absl::StrCat("\\ ", expr, "\\ ");
  }

  // Note: could use \left and \right if necessary.
  virtual std::string parens(const std::string& expr) {
    return absl::StrCat("(", expr, ")");
  }
  virtual std::string brackets(const std::string& expr) {
    return absl::StrCat("[", expr, "]");
  }
  virtual std::string braces(const std::string& expr) {
    return absl::StrCat("\\{", expr, "\\}");
  }
  virtual std::string chevrons(const std::string& expr) {
    return absl::StrCat("\\langle", expr, "\\rangle");
  }
  virtual std::string frac_parens(const std::string& expr) {
    return expr;
  }

  virtual std::string coeff(int v) {
    if (*current_formatting_config().compact_expression) {
      if      (v == 0)  { return "0"; }
      else if (v == 1)  { return ""; }
      else if (v == -1) { return "-"; }
      else if (v > 0)   { return absl::StrCat(v); }
      else              { return absl::StrCat(v); }
    } else {
      if      (v == 0)  { return "0"; }
      else if (v == 1)  { return "+"; }
      else if (v == -1) { return "-"; }
      else if (v > 0)   { return absl::StrCat("+", v); }
      else              { return absl::StrCat(v); }
    }
  }

  virtual std::string sub(const std::string& main, const std::vector<std::string>& indices) {
    CHECK(!main.empty());
    return indices.empty() ? main : absl::StrCat(main, "_{", str_join(indices, ","), "}");
  }
  virtual std::string lrsub(const std::string& left_index, const std::string& main, const std::vector<std::string>& right_indices) {
    CHECK(!main.empty());
    // TODO: Cleaner solution, e.g. \tensor
    return absl::StrCat(
      left_index.empty() ? "" : absl::StrCat("{}_{", left_index, "}"),
      main,
      right_indices.empty() ? "" : absl::StrCat("_{", str_join(right_indices, ","), "}")
    );
  }

  virtual std::string super(const std::string& main, const std::vector<std::string>& indices) {
    CHECK(!main.empty());
    return indices.empty() ? main : absl::StrCat(main, "^{", str_join(indices, ","), "}");
  }

  virtual std::string var(int idx) {
    return sub_num("x", {idx});
  }
  virtual std::string function(const std::string& name, const std::vector<std::string>& args, HSpacing) {
    // TODO: Fix: operatorname is applied to sub-indices
    return absl::StrCat("\\operatorname{", name, "}", parens(str_join(args, ",")));
  }
  virtual std::string function_indexed_args(const std::string& name, const std::vector<int>& indices, HSpacing hspacing) {
    return function(name, mapped(indices, [&](int x){ return var(x); }), hspacing);
  }

  virtual std::string begin_rich_text(const RichTextOptions& options) {
    if (*current_formatting_config().rich_text_format != RichTextFormat::native) {
      return {};
    }
    // TODO: implement
    return {};
  }
  virtual std::string end_rich_text() {
    if (*current_formatting_config().rich_text_format != RichTextFormat::native) {
      return {};
    }
    // TODO: implement
    return {};
  }
};


static AbstractFormatter* ascii_formatter = new AsciiFormatter;
static AbstractFormatter* unicode_formatter = new UnicodeFormatter;
static AbstractFormatter* latex_formatter = new LatexFormatter;

AbstractFormatter* current_formatter() {
  switch (*current_formatting_config().formatter) {
    case Formatter::ascii:      return ascii_formatter;
    case Formatter::unicode:    return unicode_formatter;
    case Formatter::latex:      return latex_formatter;
  }
  FATAL("Unknown formatter");
}
