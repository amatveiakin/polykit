#include "delta_parse.h"

#include <regex>

#include "parse.h"


static ParserRegex x_re(R"(x)");
static ParserRegex br_open_re(R"([\[(])");
static ParserRegex br_close_re(R"([\])])");
static ParserRegex comma_re(R"(,)");
static ParserRegex minus_re(R"(-)");
static ParserRegex sign_re(R"([-+])");
static ParserRegex multiplication_re(R"(\*)");
static ParserRegex tensor_prod_re(R"(\*)");
// TODO:
//   static ParserRegex minus_re(R"([-−])");  // dash or Unicode minus sign
//   static ParserRegex sign_re(R"([-−+])");
//   static ParserRegex tensor_prod_re(R"([*⊗])");


static int consume_variable(ParserState& st) {
  consume(st, x_re);
  return consume_integer(st);
}

static Delta consume_delta(ParserState& st) {
  consume(st, br_open_re);
  Delta ret;
  const auto a_or = try_consume_integer(st);
  if (a_or.has_value()) {
    const int a = a_or.value();
    consume(st, comma_re);
    const int b = consume_integer(st);
    ret = {a, b};
  } else {
    const int a = consume_variable(st);
    consume(st, minus_re);
    const int b = consume_variable(st);
    ret = {a, b};
  }
  consume(st, br_close_re);
  return ret;
}

static DeltaExpr parse_term(ParserState& st) {
  std::vector<Delta> term;
  const auto sign_str = consume(st, sign_re);
  int coeff = sign_str == "+" ? 1 : -1;
  if (const auto coeff_multiple = try_consume_integer(st)) {
    coeff *= coeff_multiple.value();
    try_consume(st, multiplication_re);
  }
  do {
    term.push_back(consume_delta(st));
  } while (!try_consume(st, tensor_prod_re).empty() || peek(st, br_open_re));
  return coeff * DeltaExpr::single(term);
}

static DeltaExpr parse_expression(ParserState& st) {
  DeltaExpr expr;
  while (!st.done()) {
    expr += parse_term(st);
  }
  return expr;
}

DeltaExpr parse_delta_expression(const std::string& s) {
  ParserState st(&s);
  return parse_expression(st);
}
