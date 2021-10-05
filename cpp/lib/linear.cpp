#include "linear.h"

#include <sstream>


std::ostream& operator<<(std::ostream& os, const LinearAnnotation& annotations) {
  using namespace cmp;
  switch (*current_formatting_config().annotation_sorting) {
    case AnnotationSorting::lexicographic:
      to_ostream(os, annotations.expression, std::less<>{}, LinearNoContext{});
      break;
    case AnnotationSorting::length:
      to_ostream(os, annotations.expression, projected([](const std::string& s) {
        return std::tuple{desc_val(s.size()), asc_ref(s)};
      }), LinearNoContext{});
      break;
  }
  for (const auto& err : annotations.errors) {
    os << fmt::coeff(1) << "<?> " << err << fmt::newline();
  }
  return os;
}

std::string annotations_one_liner(const LinearAnnotation& annotations) {
  if (annotations.empty() || annotations.has_errors()) {
    return "<?>";
  } else if (annotations.expression.num_terms() == 1) {
    ScopedFormatting sf(FormattingConfig().set_compact_expression(true));
    std::stringstream ss;
    ss << annotations.expression;
    return trimed(ss.str());
  } else {
    return absl::StrCat("<", annotations.expression.num_terms(), " ",
        en_plural(annotations.expression.num_terms(), "term"), ">");
  }
}
