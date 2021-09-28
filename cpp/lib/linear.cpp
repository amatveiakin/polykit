#include "linear.h"

#include <sstream>


std::ostream& operator<<(std::ostream& os, const LinearAnnotation& annotations) {
  switch (*current_formatting_config().annotation_sorting) {
    case AnnotationSorting::lexicographic:
      to_ostream(os, annotations.expression, std::less<>{}, LinearNoContext{});
      break;
    case AnnotationSorting::length:
      to_ostream(os, annotations.expression, [](const std::string& a, const std::string& b) {
        const int a_length = -static_cast<int>(a.size());
        const int b_length = -static_cast<int>(b.size());
        return std::tie(a_length, a) < std::tie(b_length, b);
      }, LinearNoContext{});
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
