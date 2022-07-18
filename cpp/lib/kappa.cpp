#include "kappa.h"


const static absl::flat_hash_set<Kappa> weakly_separated_with_x = {
  Kappa(KappaX{}),
  Kappa(Gamma({1,2,3})),
  Kappa(Gamma({2,3,4})),
  Kappa(Gamma({3,4,5})),
  Kappa(Gamma({4,5,6})),
  Kappa(Gamma({5,6,1})),
  Kappa(Gamma({6,1,2})),
  Kappa(Gamma({2,5,1})),
  Kappa(Gamma({3,4,1})),
  Kappa(Gamma({2,4,1})),
  Kappa(Gamma({3,5,6})),
  Kappa(Gamma({3,4,6})),
  Kappa(Gamma({2,5,6})),
};

const static absl::flat_hash_set<Kappa> weakly_separated_with_y = {
  Kappa(KappaY{}),
  Kappa(Gamma({1,2,3})),
  Kappa(Gamma({2,3,4})),
  Kappa(Gamma({3,4,5})),
  Kappa(Gamma({4,5,6})),
  Kappa(Gamma({5,6,1})),
  Kappa(Gamma({6,1,2})),
  Kappa(Gamma({1,4,6})),
  Kappa(Gamma({2,3,6})),
  Kappa(Gamma({1,3,6})),
  Kappa(Gamma({2,4,5})),
  Kappa(Gamma({2,3,5})),
  Kappa(Gamma({1,4,5})),
};


std::string to_string(const Kappa& k) {
  return std::visit(overloaded {
    [&](const KappaX&) { return std::string("(  X  )"); },
    [&](const KappaY&) { return std::string("(  Y  )"); },
    [&](const Gamma& g) { return fmt::parens(str_join(g.index_vector(), ",")); },
  }, k);
  // return std::visit(overloaded {
  //   [&](const KappaX&) { return "(12,34,56)"; },
  //   [&](const KappaY&) { return "(23,45,61)"; },
  //   [&](const Gamma& g) { return fmt::parens(absl::StrCat("pl:", str_join(g.index_vector(), ","))); },
  // }, k);
}


bool are_weakly_separated(const Kappa& k1, const Kappa& k2) {
  return std::visit(overloaded {
    [&](const KappaX&) { return weakly_separated_with_x.contains(k2); },
    [&](const KappaY&) { return weakly_separated_with_y.contains(k2); },
    [&](const Gamma& g1) {
      return std::visit(overloaded {
        [&](const KappaX&) { return weakly_separated_with_x.contains(k1); },
        [&](const KappaY&) { return weakly_separated_with_y.contains(k1); },
        [&](const Gamma& g2) { return are_weakly_separated(g1, g2); },
      }, k2);
    },
  }, k1);
}

bool is_weakly_separated(const KappaExpr::ObjectT& term) {
  for (int i : range(term.size())) {
    for (int j : range(i)) {
      if (!are_weakly_separated(term[i], term[j])) {
        return false;
      }
    }
  }
  return true;
}
bool is_weakly_separated(const KappaNCoExpr::ObjectT& term) {
  return is_weakly_separated(flatten(term));
}

KappaExpr gamma_expr_to_kappa_expr(const GammaExpr& expr) {
  return expr.mapped<KappaExpr>([](const auto& term) {
    return mapped(term, convert_to<Kappa>);
  });
}

KappaExpr NLog(int weight, const std::array<KappaExpr, 3>& triple) {
  const auto &[a, b, c] = triple;
  CHECK_GE(weight, 2);
  KappaExpr ret = tensor_product(a - c, b - c);
  for (EACH : range(weight - 2)) {
    ret = tensor_product(b - c, ret);
  }
  return ret;
}
