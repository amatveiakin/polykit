#include "polylog_cross_ratio_param.h"

#include "format.h"
#include "util.h"


void CompoundRatio::normalize() {
  check();
  absl::c_sort(numerator_);
  absl::c_sort(denominator_);
  auto num_it = numerator_.begin();
  auto denom_it = denominator_.begin();
  while (num_it != numerator_.end() && denom_it != denominator_.end()) {
    if (*num_it == *denom_it) {
      num_it = numerator_.erase(num_it);
      denom_it = denominator_.erase(denom_it);
    } else if (*num_it < *denom_it) {
      ++num_it;
    } else {
      ++denom_it;
    }
  }
  check();
}

// TODO: Generalize this to other cross ratio equations.
// TODO: Find a clearer way to compute this.
// TODO: Test this.
std::optional<CompoundRatio> CompoundRatio::one_minus() const {
  const auto strictly_increasing = [](const std::array<int, 4>& arr) -> bool {
    for (int i = 0; i < arr.size() - 1; ++i) {
      if (arr[i] >= arr[i+1]) {
        return false;
      }
    }
    return true;
  };
  // Will overcount in case of two common elements, but that doesn't matter.
  // We only need "one" vs "not one".
  const auto common_elements = [](Delta x, Delta y) -> int {
    return (x.a() == y.a()) + (x.a() == y.b()) +
            (x.b() == y.a()) + (x.b() == y.b());
  };
  const auto delta_replace = [](Delta x, Delta r) -> Delta {
    if (x.a() == r.a()) {
      return Delta(r.b(), x.b());
    } else if (x.a() == r.b()) {
      return Delta(r.a(), x.b());
    } else if (x.b() == r.a()) {
      return Delta(x.a(), r.b());
    } else if (x.b() == r.b()) {
      return Delta(x.a(), r.a());
    } else {
      FAIL("No match");
    }
  };

  check();
  if (numerator_.size() != 2) {
    return std::nullopt;
  }
  const auto& p = numerator_;
  const auto& q = denominator_;
  std::array sp{p[0].a(), p[0].b(), p[1].a(), p[1].b()};
  std::array sq{q[0].a(), q[0].b(), q[1].a(), q[1].b()};
  absl::c_sort(sp);
  absl::c_sort(sq);
  if (sp != sq || !strictly_increasing(sp) || !strictly_increasing(sq) ||
      common_elements(p[0], q[0]) != 1) {
    return std::nullopt;
  }
  CHECK_EQ(common_elements(p[0], q[1]), 1);
  CHECK_EQ(common_elements(p[1], q[0]), 1);
  CHECK_EQ(common_elements(p[1], q[1]), 1);
  // Verified: this is a cross-ratio. Now use the formula:
  //   1 - [a,b,c,d] = [a,c,b,d]
  return CompoundRatio({delta_replace(p[0], q[0]), delta_replace(p[1], q[0])}, q);
}


Word lira_param_to_key(const LiraParam& params) {
  Compressor compressor;
  compressor.add_segment(params.weights());
  for (const auto& r : params.ratios()) {
    compressor.add_segment(r.serialized());
  }
  return Word(std::move(compressor).result());
}

LiraParam key_to_lira_param(const Word& word) {
  Decompressor decompressor(word.span());
  std::vector<int> weights = decompressor.next_segment();
  std::vector<CompoundRatio> ratios;
  while (!decompressor.done()) {
    ratios.push_back(CompoundRatio::from_serialized(
      absl::MakeConstSpan(decompressor.next_segment())));
  }
  return LiraParam(std::move(weights), std::move(ratios));
}

std::string to_string(const LiraParam& params) {
  return fmt::function(
    fmt::sub_num("Li", params.weights()),
    mapped(params.ratios(), [](const CompoundRatio& ratio){
      return to_string(ratio);
    }),
    HSpacing::sparse
  );
}
