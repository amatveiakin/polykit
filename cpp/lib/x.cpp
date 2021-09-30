#include "x.h"


std::string to_string(XForm form) {
  SWITCH_ENUM_OR_DIE(form, {
    case XForm::var:        return "var";
    case XForm::neg_var:    return "neg_var";
    case XForm::sq_var:     return "sq_var";
    case XForm::zero:       return "zero";
    case XForm::infinity:   return "infinity";
    case XForm::undefined:  return "undefined";
  });
}

std::string to_string(X x) {
  if (!*current_formatting_config().compact_x) {
    SWITCH_ENUM_OR_DIE(x.form(), {
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
    });
  } else {
    SWITCH_ENUM_OR_DIE(x.form(), {
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
    });
  }
}
