#include "x.h"


std::string to_string(XForm form) {
  switch (form) {
    case XForm::var:        return "var";
    case XForm::neg_var:    return "neg_var";
    case XForm::sq_var:     return "sq_var";
    case XForm::zero:       return "zero";
    case XForm::infinity:   return "infinity";
    case XForm::undefined:  return "undefined";
  }
  FATAL(absl::StrCat("Unknown form: ", form));
}

std::string to_string(X x) {
  if (!*current_formatting_config().compact_x) {
    switch (x.form()) {
      case XForm::var:
        return fmt::sub_num("x", {x.idx()});
      case XForm::neg_var:
        return fmt::minus() + fmt::sub_num("x", {x.idx()});
      case XForm::sq_var:
        return fmt::super_num(fmt::sub_num("x", {x.idx()}), {2});
      case XForm::zero:
        return "0";
      case XForm::infinity:
        return fmt::inf();
      case XForm::undefined:
        return "<?>";
    }
  } else {
    switch (x.form()) {
      case XForm::var:
        return fmt::num(x.idx());
      case XForm::neg_var:
        return fmt::num(-x.idx());
      case XForm::sq_var:
        return fmt::super_num(fmt::num(x.idx()), {2});
      case XForm::zero:
        return "<0>";
      case XForm::infinity:
        return fmt::inf();
      case XForm::undefined:
        return "<?>";
    }
  }
  FATAL(absl::StrCat("Unknown form: ", to_string(x.form())));
}
