// TODO: Replace `virtual`-s with `override`-s

#include "format.h"

#include "util.h"

#include "absl/strings/str_cat.h"


static std::vector<std::string> ints_to_strings(const std::vector<int> v) {
  return mapped(v, [](int x){ return absl::StrCat(x); });
}


std::string Formatter::sub_num(const std::string& main, const std::vector<int>& indices) {
  return sub(main, ints_to_strings(indices));
}
std::string Formatter::lrsub_num(int left_index, const std::string& main, const std::vector<int>& right_indices) {
  return lrsub(absl::StrCat(left_index), main, ints_to_strings(right_indices));
}


class PlainTextFormatter : public Formatter {
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
    if (v == 0) {
      return " 0 ";
    } else if (v == 1) {
      return " + ";
    } else if (v == -1) {
      return " - ";
    } else if (v > 0) {
      return absl::StrCat("+", v, " ");
    } else {
      return absl::StrCat(v, " ");
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

  virtual std::string Formatter::var(int idx) {
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


class LatexFormatter : public Formatter {
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
    if (v == 0) {
      return "0";
    } else if (v == 1) {
      return "+";
    } else if (v == -1) {
      return "-";
    } else if (v > 0) {
      return absl::StrCat("+", v);
    } else {
      return absl::StrCat(v);
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

  virtual std::string Formatter::var(int idx) {
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


Formatter* formatter = new PlainTextFormatter;
// Formatter* formatter = new LatexFormatter;
