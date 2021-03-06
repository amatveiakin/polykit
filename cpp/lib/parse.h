// Utils for constructing simple LR(1) parsers with no split between lexer and parser.

#pragma once

#include <optional>
#include <regex>


class ParserRegex {
public:
  ParserRegex(const std::string& s) :
      re_(s, std::regex_constants::nosubs | std::regex_constants::optimize),
      str_(s) {}

  std::regex re() const { return re_; }
  std::string str() const { return str_; }

private:
  std::regex re_;
  std::string str_;
};


class ParserState {
public:
  ParserState(const std::string* s);

  const std::string_view& view() const { return view_; }
  std::string_view&       view()       { return view_; }

  bool done() const { return view_.empty(); }

private:
  std::string_view view_;
};


bool peek(const ParserState& st, const ParserRegex& re);
std::string_view try_consume(ParserState& st, const ParserRegex& re);
std::string_view consume(ParserState& st, const ParserRegex& re);
std::optional<int> try_consume_integer(ParserState& st);
int consume_integer(ParserState& st);
