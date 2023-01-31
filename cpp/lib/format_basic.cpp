#include "format_basic.h"

#include <regex>

#include "absl/strings/str_cat.h"
#include "absl/strings/substitute.h"

#include "unicode_alphabets.h"


template<typename F>
static auto string_mapper(const F& char_mapper) {
  return [&](const std::string& s) {
    std::string ret;
    for (const char ch : s) {
      ret += char_mapper(ch);
    }
    return ret;
  };
}


static const FormattingConfig default_formatting_config = FormattingConfig()
  .set_encoder(Encoder::ascii)
  .set_rich_text_format(RichTextFormat::native)
  .set_unicode_version(UnicodeVersion::full)
  .set_annotation_sorting(AnnotationSorting::lexicographic)
  .set_expression_line_limit(100)
  .set_expression_include_annotations(true)
  .set_parsable_expression(false)
  .set_compact_expression(false)
  .set_compact_x(false)
  .set_max_terms_in_annotations_one_liner(1)
  .set_new_line_after_expression(true)
;


static thread_local std::vector<FormattingConfig> formatting_config_stack;
static thread_local FormattingConfig default_formatting_config_overrides;
static thread_local FormattingConfig aggregated_formatting_config = default_formatting_config;

// This is marked as thread-local to avoid nasty race consitions, but
// in reality only writing to console from one thread is supported.
static thread_local std::vector<RichTextOptions> console_rich_text_options_stack;

static int text_color_to_console_color(TextColor color) {
  SWITCH_ENUM_OR_DIE(color, {
    case TextColor::normal:         return 0;   // normal
    case TextColor::red:            return 91;  // bright_red
    case TextColor::green:          return 92;  // bright_green
    case TextColor::yellow:         return 93;  // bright_yellow
    case TextColor::blue:           return 94;  // bright_blue
    case TextColor::magenta:        return 95;  // bright_magenta
    case TextColor::cyan:           return 96;  // bright_cyan
    case TextColor::orange:         return 33;  // yellow (actually dark yellow, but close enough)
    case TextColor::pale_red:       return 31;  // red
    case TextColor::pale_green:     return 32;  // green
    case TextColor::pale_blue:      return 34;  // blue
    case TextColor::pale_magenta:   return 35;  // magenta
    case TextColor::pale_cyan:      return 36;  // cyan
  });
}

static std::string text_color_to_html_color(TextColor color) {
  SWITCH_ENUM_OR_DIE(color, {
    case TextColor::normal:         return "Black";
    case TextColor::red:            return "Red";
    case TextColor::green:          return "LimeGreen";
    case TextColor::yellow:         return "GoldenRod";
    case TextColor::blue:           return "Blue";
    case TextColor::magenta:        return "Magenta";
    case TextColor::cyan:           return "DarkCyan";
    case TextColor::orange:         return "DarkOrange";
    case TextColor::pale_red:       return "LightCoral";
    case TextColor::pale_green:     return "LightGreen";
    case TextColor::pale_blue:      return "DeepSkyBlue";
    case TextColor::pale_magenta:   return "Violet";
    case TextColor::pale_cyan:      return "MediumAquamarine";
  });
}

// Note. Color names are case-sensitive. Basic colors start with a small letter
// and additional colors provided by `dvipsnames` option start with a capital
// letter. Colors that differ only in case can be very different, e.g. "Green"
// is much darker than "green".
static std::string text_color_to_latex_color(TextColor color) {
  SWITCH_ENUM_OR_DIE(color, {
    case TextColor::normal:         return "black";
    case TextColor::red:            return "red";
    case TextColor::green:          return "Green";
    case TextColor::yellow:         return "Goldenrod";
    case TextColor::blue:           return "blue";
    case TextColor::magenta:        return "magenta";
    case TextColor::cyan:           return "cyan";
    case TextColor::orange:         return "Peach";
    case TextColor::pale_red:       return "Salmon";
    case TextColor::pale_green:     return "LimeGreen";
    case TextColor::pale_blue:      return "SkyBlue";
    case TextColor::pale_magenta:   return "Lavender";
    case TextColor::pale_cyan:      return "Turquoise";
  });
}

static std::string get_command_for_console_rich_text_options() {
  const RichTextOptions& options = console_rich_text_options_stack.empty()
    ? RichTextOptions()
    : console_rich_text_options_stack.back();
  return options.text_color == TextColor::normal
    ? "\033[0m"
    : absl::StrCat("\033[", text_color_to_console_color(options.text_color), "m");
}

static std::string rich_text_options_to_css(const RichTextOptions& options) {
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
  apply_field_override(encoder, src.encoder);
  apply_field_override(rich_text_format, src.rich_text_format);
  apply_field_override(unicode_version, src.unicode_version);
  apply_field_override(annotation_sorting, src.annotation_sorting);
  apply_field_override(expression_line_limit, src.expression_line_limit);
  apply_field_override(expression_include_annotations, src.expression_include_annotations);
  apply_field_override(parsable_expression, src.parsable_expression);
  apply_field_override(compact_expression, src.compact_expression);
  apply_field_override(compact_x, src.compact_x);
  apply_field_override(max_terms_in_annotations_one_liner, src.max_terms_in_annotations_one_liner);
  apply_field_override(new_line_after_expression, src.new_line_after_expression);
}

static void recompute_formatting_config() {
  aggregated_formatting_config = default_formatting_config;
  aggregated_formatting_config.apply_overrides(default_formatting_config_overrides);
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

void set_default_formatting(const FormattingConfig& config) {
  default_formatting_config_overrides = config;
  recompute_formatting_config();
}

void reset_default_formatting() {
  default_formatting_config_overrides = {};
  recompute_formatting_config();
}


ScopedRichTextOptions::ScopedRichTextOptions(std::ostream& os, const RichTextOptions& options) {
  stream = &os;
  (*stream) << current_encoder()->begin_rich_text(options);
}

ScopedRichTextOptions::~ScopedRichTextOptions() {
  (*stream) << current_encoder()->end_rich_text();
}


static bool is_full_unicode() {
  return *current_formatting_config().unicode_version == UnicodeVersion::full;
}

// TODO: Consider wrapping all calls to Ascii and Unicode encoders with `html_escape`
//   instead of manually checking `is_html` everywhere.
static bool is_html() {
  return *current_formatting_config().rich_text_format == RichTextFormat::html;
}

static std::string maybe_html_newline() {
  return is_html() ? " <br>\n" : "\n";
}


std::string AbstractEncoder::unity() { return fmt::colored(chevrons("1"), TextColor::yellow); }
std::string AbstractEncoder::minus() { return "-"; }

std::string AbstractEncoder::opname(const std::string& name) { return name; }

std::string AbstractEncoder::begin_rich_text(const RichTextOptions& options) {
  SWITCH_ENUM_OR_DIE(*current_formatting_config().rich_text_format, {
    case RichTextFormat::native:
    case RichTextFormat::plain_text:
      return {};
    case RichTextFormat::console:
      console_rich_text_options_stack.push_back(options);
      return get_command_for_console_rich_text_options();
    case RichTextFormat::html:
      return absl::Substitute(R"(<span style="$0">)", rich_text_options_to_css(options));
  });
}

std::string AbstractEncoder::end_rich_text() {
  SWITCH_ENUM_OR_DIE(*current_formatting_config().rich_text_format, {
    case RichTextFormat::native:
    case RichTextFormat::plain_text:
      return {};
    case RichTextFormat::console:
      console_rich_text_options_stack.pop_back();
      return get_command_for_console_rich_text_options();
    case RichTextFormat::html:
      return absl::Substitute("</span>");
  });
}


class AsciiEncoder : public AbstractEncoder {
  std::string newline() override { return maybe_html_newline(); }
  std::string inf() override { return "Inf"; }
  std::string dot() override { return "."; }
  std::string tensor_prod() override { return " * "; }
  std::string coprod_normal() override { return "  ^  "; }
  std::string coprod_iterated() override { return "  @  "; }
  std::string coprod_hopf() override { return "  %  "; }
  std::string comult() override { return is_html() ? "&amp;" : "&"; }
  std::string set_union() override { return "|"; }
  std::string set_intersection() override { return is_html() ? "&amp;" : "&"; }
  std::string set_complement() override { return "~"; }

  std::string sum(const std::string& lhs, const std::string& rhs, HSpacing hspacing) override {
    const std::string spacing = (hspacing == HSpacing::dense ? "" : " ");
    return absl::StrCat(lhs, spacing, "+", spacing, rhs);
  }
  std::string diff(const std::string& lhs, const std::string& rhs, HSpacing hspacing) override {
    const std::string spacing = (hspacing == HSpacing::dense ? "" : " ");
    return absl::StrCat(lhs, spacing, "-", spacing, rhs);
  }
  std::string frac(const std::string& numerator, const std::string& denominator) override {
    return absl::StrCat(numerator, "/", denominator);
  }

  std::string parens(const std::string& expr) override {
    return absl::StrCat("(", expr, ")");
  }
  std::string brackets(const std::string& expr) override {
    return absl::StrCat("[", expr, "]");
  }
  std::string braces(const std::string& expr) override {
    return absl::StrCat("{", expr, "}");
  }
  std::string chevrons(const std::string& expr) override {
    return is_html()
      ? absl::StrCat("&lt;", expr, "&gt;")
      : absl::StrCat("<", expr, ">");
  }
  std::string frac_parens(const std::string& expr) override {
    return parens(expr);
  }

  std::string num(int v) override {
    return absl::StrCat(v);
  }
  std::string coeff(int v) override {
    // TODO: Remove left padding here (and in other formatters), this is done
    //   by `to_ostream` for in linear.cpp.
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
    } else {
      if      (v == 0)  { return " 0 "; }
      else if (v == 1)  { return " + "; }
      else if (v == -1) { return " - "; }
      else if (v > 0)   { return absl::StrCat("+", v, " "); }
      else              { return absl::StrCat(v, " "); }
    }
  }
  std::string coeff_one_liner(int v, bool first_term) override {
    if (first_term) {
      if      (v == 1)  { return ""; }
      else if (v == -1) { return "-"; }
      else if (v >= 0)  { return absl::StrCat(v, " "); }
      else              { return absl::StrCat(v, " "); }
    } else {
      if      (v == 1)  { return " + "; }
      else if (v == -1) { return " - "; }
      else if (v >= 0)  { return absl::StrCat(" +", v, " "); }
      else              { return absl::StrCat(" ", v, " "); }
    }
  }

  // TODO: Consider removing "_" from subindices for parseability.
  std::string sub(const std::string& main, const std::vector<std::string>& indices) override {
    CHECK(!main.empty());
    return indices.empty() ? main : absl::StrCat(main, "_", str_join(indices, "_"));
  }
  std::string lrsub(const std::string& left_index, const std::string& main, const std::vector<std::string>& right_indices) override {
    CHECK(!main.empty());
    return absl::StrCat(
      left_index.empty() ? "" : absl::StrCat(left_index, "_"),
      main,
      right_indices.empty() ? "" : absl::StrCat("_", str_join(right_indices, "_"))
    );
  }

  std::string super(const std::string& main, const std::vector<std::string>& indices) override {
    CHECK(!main.empty());
    return indices.empty()
      ? main
      : indices.size() == 1
        ? absl::StrCat(main, indices.front())
        : absl::StrCat(main, "^", str_join(indices, "^"));
  }

  std::string mathcal(const std::string& str) override { return str; }
  std::string mathbb(const std::string& str) override { return str; }

  std::string var(int idx) override {
    return absl::StrCat("x", idx);
  }
  std::string function(const std::string& name, const std::vector<std::string>& args, HSpacing hspacing) override {
    const auto separator = (hspacing == HSpacing::dense ? "," : ", ");
    return name + parens(str_join(args, separator));
  }
};


class UnicodeEncoder : public AbstractEncoder {
  static constexpr char kNbsp[] = " ";
  // static constexpr char kThinSpace[] = " ";
  static constexpr char kThinNbsp[] = " ";
  static constexpr char kMinusSign[] = "−";

  // TODO: Test unicode in different terminals, browsers and operating systems.
  std::string newline() override { return maybe_html_newline(); }
  std::string inf() override { return "∞"; }
  std::string dot() override { return ""; }
  std::string minus() override { return kMinusSign; }
  std::string tensor_prod() override { return is_full_unicode() ? "⨂" : "⊗"; }
  std::string coprod_normal() override { return hspace("∧"); }
  std::string coprod_iterated() override { return is_full_unicode() ? hspace("⦻") : hspace(tensor_prod()); }
  std::string coprod_hopf() override { return hspace("☒"); }
  std::string comult() override { return "△"; }
  std::string set_union() override { return "⋃"; }
  std::string set_intersection() override { return "⋂"; }
  std::string set_complement() override { return "¬"; }  // TODO: Is this the best symbol? (same for LaTeX)

  std::string sum(const std::string& lhs, const std::string& rhs, HSpacing hspacing) override {
    const std::string spacing = (hspacing == HSpacing::dense ? "" : kNbsp);
    return absl::StrCat(lhs, spacing, "+", spacing, rhs);
  }
  std::string diff(const std::string& lhs, const std::string& rhs, HSpacing hspacing) override {
    const std::string spacing = (hspacing == HSpacing::dense ? "" : kNbsp);
    return absl::StrCat(lhs, spacing, kMinusSign, spacing, rhs);
  }
  std::string frac(const std::string& numerator, const std::string& denominator) override {
    return absl::StrCat(numerator, "/", denominator);
  }

  std::string hspace(const std::string& expr) {
    return absl::StrCat(kNbsp, expr, kNbsp);
  }

  std::string parens(const std::string& expr) override {
    return absl::StrCat("(", expr, ")");
  }
  std::string brackets(const std::string& expr) override {
    return absl::StrCat("[", expr, "]");
  }
  std::string braces(const std::string& expr) override {
    return absl::StrCat("{", expr, "}");
  }
  std::string chevrons(const std::string& expr) override {
    return absl::StrCat("⟨", expr, "⟩");
  }
  std::string frac_parens(const std::string& expr) override {
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

  std::string num(int v) override {
    return fix_minus(absl::StrCat(v));
  }
  std::string coeff(int v) override {
    // TODO: Use using "figure space" (U+2007) here.
    if      (v == 0)  { return absl::StrCat(" 0", kThinNbsp); }
    else if (v == 1)  { return absl::StrCat(" +", kThinNbsp); }
    else if (v == -1) { return absl::StrCat(" ", kMinusSign, kThinNbsp); }
    else if (v > 0)   { return absl::StrCat("+", v, kThinNbsp); }
    else              { return fix_minus(absl::StrCat(v, kThinNbsp)); }
  }
  std::string coeff_one_liner(int v, bool first_term) override {
    if (first_term) {
      if      (v == 1)  { return ""; }
      else if (v == -1) { return kMinusSign; }
      else if (v >= 0)  { return absl::StrCat(v, kThinNbsp); }
      else              { return fix_minus(absl::StrCat(v, kThinNbsp)); }
    } else {
      if      (v == 1)  { return absl::StrCat(" +", kThinNbsp); }
      else if (v == -1) { return absl::StrCat(" ", kMinusSign, kThinNbsp); }
      else if (v >= 0)  { return absl::StrCat(" ", v, kThinNbsp); }
      else              { return fix_minus(absl::StrCat(" ", v, kThinNbsp)); }
    }
  }

  static std::string to_subscript(char ch) {
    const auto ret = unicode_subscript(ch);
    CHECK(ret.has_value()) << "There is no known subscript for '" << std::string(1, ch) << "'";
    return *ret;
  }
  static std::string to_superscript(char ch) {
    const auto ret = unicode_superscript(ch);
    CHECK(ret.has_value()) << "There is no known superscript for '" << std::string(1, ch) << "'";
    return *ret;
  }
  static std::string to_mathcal(char ch) {
    const auto ret = unicode_mathcal(ch);
    CHECK(ret.has_value()) << "There is no mathcal for '" << std::string(1, ch) << "'";
    return *ret;
  }
  static std::string to_mathbb(char ch) {
    const auto ret = unicode_mathbb(ch);
    CHECK(ret.has_value()) << "There is no mathbb for '" << std::string(1, ch) << "'";
    return *ret;
  }

  std::string sub(const std::string& main, const std::vector<std::string>& indices) override {
    CHECK(!main.empty());
    const std::string separator = absl::c_all_of(indices, [](const std::string& s) {
      return strlen_utf8(s) == 1;
    }) ? "" : ",";
    return absl::StrCat(main, str_join(indices, separator, string_mapper(to_subscript)));
  }
  std::string lrsub(const std::string& left_index, const std::string& main, const std::vector<std::string>& right_indices) override {
    CHECK(!main.empty());
    return absl::StrCat(
      string_mapper(to_subscript)(left_index),
      sub(main, right_indices)
    );
  }

  std::string super(const std::string& main, const std::vector<std::string>& indices) override {
    CHECK(!main.empty());
    const std::string separator = absl::c_all_of(indices, [](const std::string& s) {
      return strlen_utf8(s) == 1;
    }) ? "" : "˒";
    return absl::StrCat(main, str_join(indices, separator, string_mapper(to_superscript)));
  }

  std::string mathcal(const std::string& str) override {
    return is_full_unicode() ? string_mapper(to_mathcal)(str) : str;
  }
  std::string mathbb(const std::string& str) override {
    return is_full_unicode() ? string_mapper(to_mathbb)(str) : str;
  }

  std::string var(int idx) override {
    return sub("x", {to_string(idx)});
  }
  std::string function(const std::string& name, const std::vector<std::string>& args, HSpacing hspacing) override {
    const auto separator = (hspacing == HSpacing::dense ? "," : absl::StrCat(",", kThinNbsp));
    return name + parens(str_join(args, separator));
  }
};


// TODO: Either find new names for all functions that include an underscore sign,
//   or fix underscores in LaTeX.
class LatexEncoder : public AbstractEncoder {
  std::string newline() override { return "\\\\\n"; }
  // TODO: Make sure all op signs are in fact math ops from Latex point of view
  std::string inf() override { return "\\infty"; }
  std::string dot() override { return ""; }
  std::string tensor_prod() override { return " \\otimes "; }
  std::string coprod_normal() override { return hspace("\\wedge"); }
  std::string coprod_iterated() override { return hspace("\\bigotimes"); }
  std::string coprod_hopf() override { return hspace("\\boxtimes"); }
  std::string comult() override { return " \\triangle "; }
  std::string set_union() override { return "\\bigcup"; }
  std::string set_intersection() override { return "\\bigcap"; }
  std::string set_complement() override { return "\\neg"; }

  std::string sum(const std::string& lhs, const std::string& rhs, HSpacing) override {
    return absl::StrCat(lhs, "+", rhs);
  }
  std::string diff(const std::string& lhs, const std::string& rhs, HSpacing) override {
    return absl::StrCat(lhs, "-", rhs);
  }
  std::string frac(const std::string& numerator, const std::string& denominator) override {
    return absl::StrCat("\\frac{", numerator, "}{", denominator, "}");
  }

  std::string hspace(const std::string& expr) {
    return absl::StrCat("\\ ", expr, "\\ ");
  }

  // Note: could use \left and \right if necessary.
  std::string parens(const std::string& expr) override {
    return absl::StrCat("(", expr, ")");
  }
  std::string brackets(const std::string& expr) override {
    return absl::StrCat("[", expr, "]");
  }
  std::string braces(const std::string& expr) override {
    return absl::StrCat("\\{", expr, "\\}");
  }
  std::string chevrons(const std::string& expr) override {
    return absl::StrCat("\\langle", expr, "\\rangle");
  }
  std::string frac_parens(const std::string& expr) override {
    return expr;
  }

  std::string num(int v) override {
    return absl::StrCat(v);
  }
  std::string coeff(int v) override {
    if      (v == 0)  { return "0"; }
    else if (v == 1)  { return "+"; }
    else if (v == -1) { return "-"; }
    else if (v > 0)   { return absl::StrCat("+", v); }
    else              { return absl::StrCat(v); }
  }
  std::string coeff_one_liner(int v, bool first_term) override {
    if (first_term) {
      if      (v == 1)  { return ""; }
      else if (v == -1) { return "-"; }
      else if (v >= 0)  { return absl::StrCat(v); }
      else              { return absl::StrCat(v); }
    } else {
      if      (v == 1)  { return "+"; }
      else if (v == -1) { return "-"; }
      else if (v >= 0)  { return absl::StrCat("+", v); }
      else              { return absl::StrCat(v); }
    }
  }

  std::string sub(const std::string& main, const std::vector<std::string>& indices) override {
    CHECK(!main.empty());
    return indices.empty() ? main : absl::StrCat(main, "_{", str_join(indices, ","), "}");
  }
  std::string lrsub(const std::string& left_index, const std::string& main, const std::vector<std::string>& right_indices) override {
    CHECK(!main.empty());
    // TODO: Cleaner solution, e.g. \tensor
    return absl::StrCat(
      left_index.empty() ? "" : absl::StrCat("{}_{", left_index, "}"),
      main,
      right_indices.empty() ? "" : absl::StrCat("_{", str_join(right_indices, ","), "}")
    );
  }

  std::string super(const std::string& main, const std::vector<std::string>& indices) override {
    CHECK(!main.empty());
    return indices.empty() ? main : absl::StrCat(main, "^{", str_join(indices, ","), "}");
  }

  // Note: default LaTeX fonts define \mathcal only for capical letters.
  std::string mathcal(const std::string& str) override {
    return absl::StrCat("\\mathcal{", str, "}");
  }
  // Note: requires `amsfonts` package.
  // Note: default LaTeX fonts define \mathbb only for capical letters.
  std::string mathbb(const std::string& str) override {
    return absl::StrCat("\\mathbb{", str, "}");
  }

  std::string var(int idx) override {
    return sub("x", {to_string(idx)});
  }
  std::string opname(const std::string& name) override {
    return absl::StrCat("\\operatorname{", name, "}");
  }
  std::string function(const std::string& name, const std::vector<std::string>& args, HSpacing) override {
    return absl::StrCat(name, parens(str_join(args, ",")));
  }

  std::string begin_rich_text(const RichTextOptions& options) override {
    if (*current_formatting_config().rich_text_format != RichTextFormat::native) {
      return {};
    }
    return absl::StrCat("\\textcolor{", text_color_to_latex_color(options.text_color), "}{");
  }
  std::string end_rich_text() override {
    if (*current_formatting_config().rich_text_format != RichTextFormat::native) {
      return {};
    }
    return "}";
  }
};


AbstractEncoder* current_encoder() {
  // Important: encoders need to be static local (rather than static global)
  //   in order to allow debug print in static initializers of other modules.
  static AbstractEncoder* ascii_encoder = new AsciiEncoder;
  static AbstractEncoder* unicode_encoder = new UnicodeEncoder;
  static AbstractEncoder* latex_encoder = new LatexEncoder;

  SWITCH_ENUM_OR_DIE(*current_formatting_config().encoder, {
    case Encoder::ascii:      return ascii_encoder;
    case Encoder::unicode:    return unicode_encoder;
    case Encoder::latex:      return latex_encoder;
  });
}
