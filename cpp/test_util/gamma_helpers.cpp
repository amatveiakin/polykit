#include "gamma_helpers.h"


int detect_num_variables(const GammaExpr& expr) {
  int max_var = 0;
  for (const auto& [term, coeff] : expr) {
    for (const Gamma& g : term) {
      max_var = std::max(max_var, max_value(g.index_vector()));
    }
  }
  CHECK_GT(max_var, 0);
  return max_var;
}
