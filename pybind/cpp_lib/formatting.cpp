#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "cpp/lib/format.h"


namespace py = pybind11;

static void py_formatting(
  std::optional<Encoder> encoder,
  std::optional<RichTextFormat> rich_text_format,
  std::optional<AnnotationSorting> annotation_sorting,
  std::optional<int> expression_line_limit,
  std::optional<bool> expression_include_annotations,
  std::optional<bool> parsable_expression,
  std::optional<bool> compact_expression
) {
  // Note: not exporting `new_line_after_expression`. Use `print()` / `print(end='')` for this.
  set_default_formatting({
    .encoder = encoder,
    .rich_text_format = rich_text_format,
    .annotation_sorting = annotation_sorting,
    .expression_line_limit = expression_line_limit,
    .expression_include_annotations = expression_include_annotations,
    .parsable_expression = parsable_expression,
    .compact_expression = compact_expression,
  });
}


void pybind_format(py::module_& m) {
  py::enum_<Encoder>(m, "Encoder")
    .value("ascii", Encoder::ascii)
    .value("unicode", Encoder::unicode)
    .value("latex", Encoder::latex)
  ;

  py::enum_<RichTextFormat>(m, "RichTextFormat")
    .value("native", RichTextFormat::native)
    .value("plain_text", RichTextFormat::plain_text)
    .value("console", RichTextFormat::console)
    .value("html", RichTextFormat::html)
  ;

  py::enum_<AnnotationSorting>(m, "AnnotationSorting")
    .value("lexicographic", AnnotationSorting::lexicographic)
    .value("length", AnnotationSorting::length)
  ;

  m.def(
    "set_formatting",
    &py_formatting,
    py::kw_only(),
    py::arg("encoder") = py::none(),
    py::arg("rich_text_format") = py::none(),
    py::arg("annotation_sorting") = py::none(),
    py::arg("expression_line_limit") = py::none(),
    py::arg("expression_include_annotations") = py::none(),
    py::arg("parsable_expression") = py::none(),
    py::arg("compact_expression") = py::none()
  );

  m.def("reset_formatting", &reset_default_formatting);

  m.attr("NoLineLimit") = FormattingConfig::kNoLineLimit;
}
