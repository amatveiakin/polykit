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
  const int num_terms = annotations.expression.num_terms();
  const int max_terms = *current_formatting_config().max_terms_in_annotations_one_liner;
  if (annotations.empty() || annotations.has_errors()) {
    return "<?>";
  } else if (num_terms <= max_terms) {
    ScopedFormatting sf(FormattingConfig().set_compact_expression(true));
    std::stringstream ss;
    ss << annotations.expression;
    return num_terms > 1 ? fmt::parens(ss.str()) : ss.str();
  } else {
    return absl::StrCat("<", num_terms, " ", en_plural(num_terms, "term"), ">");
  }
}
