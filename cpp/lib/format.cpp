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


class PlainTextFormatter : public Formatter {
  virtual std::string unity() { return "<1>"; }
  virtual std::string dot() { return "."; }
  virtual std::string tensor_prod() { return "*"; }
  virtual std::string coprod_lie() { return " ^ "; }
  virtual std::string coprod_hopf() { return " @ "; }
  virtual std::string comult() { return "&"; }

  virtual std::string box(const std::string& s) {
    return absl::StrCat(s, "\n");
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
    return absl::StrCat(main, "_", str_join(indices, "_"));
  }

  virtual std::string Formatter::var(int idx) {
    return absl::StrCat("x", idx);
  }
  virtual std::string function(const std::string& name, const std::vector<std::string>& args) {
    return absl::StrCat(name, "(", str_join(args, ","), ")");
  }
  virtual std::string function_indexed_args(const std::string& name, const std::vector<int>& indices) {
    return function(name, ints_to_strings(indices));
  }
};


class LatexFormatter : public Formatter {
  // TODO: Make sure all op signs are in fact math ops from Latex point of view
  virtual std::string unity() { return "\\langle 1 \\rangle"; }
  virtual std::string dot() { return ""; }
  virtual std::string tensor_prod() { return " \\otimes "; }
  virtual std::string coprod_lie() { return " \\ \\wedge\\  "; }
  virtual std::string coprod_hopf() { return " \\ \\boxtimes\\  "; }
  virtual std::string comult() { return " \\triangle "; }

  virtual std::string box(const std::string& s) {
    return absl::StrCat(s, "\\\\\n");
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
    return absl::StrCat(main, "_{", str_join(indices, ","), "}");
  }

  virtual std::string Formatter::var(int idx) {
    return sub_num("x", {idx});
  }
  virtual std::string function(const std::string& name, const std::vector<std::string>& args) {
    // TODO: Fix: operatorname is applied to sub-indices
    return absl::StrCat("\\operatorname{", name, "}(", str_join(args, ","), ")");
  }
  virtual std::string function_indexed_args(const std::string& name, const std::vector<int>& indices) {
    return function(name, mapped(indices, [&](int x){ return var(x); }));
  }
};


Formatter* formatter = new PlainTextFormatter;
// Formatter* formatter = new LatexFormatter;
