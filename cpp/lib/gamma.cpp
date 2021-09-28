#include "gamma.h"


std::string to_string(const Gamma& g) {
  return fmt::parens(str_join(g.index_vector(), ","));
}
