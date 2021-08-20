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
