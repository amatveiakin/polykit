// TODO: Replace `virtual`-s with `override`-s

#include "format.h"

#include "util.h"

#include "absl/strings/str_cat.h"


static const FormattingConfig default_formatting_config = FormattingConfig()
  .set_formatter(Formatter::plain_text)
  .set_expression_line_limit(100)
  .set_expression_include_annotations(true)
  .set_parsable_expression(false)
  .set_compact_expression(false)
;

static thread_local std::vector<FormattingConfig> formatting_config_stack;
static thread_local FormattingConfig aggregated_formatting_config = default_formatting_config;

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


static std::vector<std::string> ints_to_strings(const std::vector<int> v) {
  return mapped(v, [](int x){ return absl::StrCat(x); });
}


std::string AbstractFormatter::sub_num(const std::string& main, const std::vector<int>& indices) {
  return sub(main, ints_to_strings(indices));
}
std::string AbstractFormatter::lrsub_num(int left_index, const std::string& main, const std::vector<int>& right_indices) {
  return lrsub(absl::StrCat(left_index), main, ints_to_strings(right_indices));
}


class PlainTextFormatter : public AbstractFormatter {
  virtual std::string inf() { return "Inf"; }
  virtual std::string unity() { return "<1>"; }
  virtual std::string dot() { return "."; }
  virtual std::string tensor_prod() { return " * "; }
  virtual std::string coprod_lie() { return "  ^  "; }
  virtual std::string coprod_hopf() { return "  @  "; }
  virtual std::string comult() { return "&"; }

  virtual std::string box(const std::string& expr) {
    return absl::StrCat(expr, "\n");
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
    return absl::StrCat("<", expr, ">");
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
    CHECK(!indices.empty());
    return absl::StrCat(main, "_", str_join(indices, "_"));
  }
  virtual std::string lrsub(const std::string& left_index, const std::string& main, const std::vector<std::string>& right_indices) {
    CHECK(!main.empty());
    return absl::StrCat(
      left_index.empty() ? "" : absl::StrCat(left_index, "_"),
      main,
      right_indices.empty() ? "" : absl::StrCat("_", str_join(right_indices, "_"))
    );
  }

  virtual std::string AbstractFormatter::var(int idx) {
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


class LatexFormatter : public AbstractFormatter {
  // TODO: Make sure all op signs are in fact math ops from Latex point of view
  virtual std::string inf() { return "\\infty"; }
  virtual std::string unity() { return chevrons("1"); }
  virtual std::string dot() { return ""; }
  virtual std::string tensor_prod() { return " \\otimes "; }
  virtual std::string coprod_lie() { return " \\ \\wedge\\  "; }
  virtual std::string coprod_hopf() { return " \\ \\boxtimes\\  "; }
  virtual std::string comult() { return " \\triangle "; }

  virtual std::string box(const std::string& expr) {
    return absl::StrCat(expr, "\\\\\n");
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
    CHECK(!indices.empty());
    return absl::StrCat(main, "_{", str_join(indices, ","), "}");
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

  virtual std::string AbstractFormatter::var(int idx) {
    return sub_num("x", {idx});
  }
  virtual std::string function(const std::string& name, const std::vector<std::string>& args, HSpacing) {
    // TODO: Fix: operatorname is applied to sub-indices
    return absl::StrCat("\\operatorname{", name, "}", parens(str_join(args, ",")));
  }
  virtual std::string function_indexed_args(const std::string& name, const std::vector<int>& indices, HSpacing hspacing) {
    return function(name, mapped(indices, [&](int x){ return var(x); }), hspacing);
  }
};


static AbstractFormatter* plain_text_formatter = new PlainTextFormatter;
static AbstractFormatter* latex_formatter = new LatexFormatter;

AbstractFormatter* current_formatter() {
  switch (*current_formatting_config().formatter) {
    case Formatter::plain_text: return plain_text_formatter;
    case Formatter::latex:      return latex_formatter;
  }
  FATAL("Unknown formatter");
}
