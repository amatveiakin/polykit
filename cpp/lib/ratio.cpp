#include "ratio.h"

#include "delta.h"
#include "format.h"
#include "set_util.h"


template<typename Container>
static void normalize_loop(Container& loop) {
  CHECK(loop.size() % 2 == 0);
  int min_value = std::numeric_limits<int>::max();
  int min_pos = -1;
  for (int i = 0; i < loop.size(); i += 2) {
    if (loop[i] < min_value) {
      min_value = loop[i];
      min_pos = i;
    }
  }
  absl::c_rotate(loop, loop.begin() + min_pos);
}


template<>
void CrossRatioTmpl<CrossRatioNormalization::full>::normalize() {
  const int min_pos = absl::c_min_element(indices_) - indices_.begin();
  switch (min_pos) {
    case 0:
      break;
    case 1:
      std::swap(indices_[0], indices_[1]);
      std::swap(indices_[2], indices_[3]);
      break;
    case 2:
      std::swap(indices_[0], indices_[2]);
      std::swap(indices_[1], indices_[3]);
      break;
    case 3:
      std::swap(indices_[0], indices_[3]);
      std::swap(indices_[1], indices_[2]);
      break;
    default:
      FATAL("Invalid min element position");
  }
}

template<>
void CrossRatioTmpl<CrossRatioNormalization::rotation_only>::normalize() {
  normalize_loop(indices_);
}

std::string dependent_cross_ratio_formula(
    CrossRatioN initial, const std::string& notation, CrossRatioN value) {
  auto is_permutation = [&](std::array<int, 4> permutation) {
    return permute(initial.indices(), permutation) == value.indices();
  };
  if (is_permutation({0,1,2,3})) {
    return notation;
  }
  if (is_permutation({0,2,1,3})) {
    return fmt::diff("1", notation);
  }
  if (is_permutation({0,3,2,1})) {
    return fmt::frac("1", notation);
  }
  if (is_permutation({0,2,3,1})) {
    return fmt::diff("1", fmt::frac("1", notation));
  }
  if (is_permutation({0,1,3,2})) {
    return fmt::frac(notation, fmt::frac_parens(fmt::diff(notation, "1", HSpacing::dense)));
  }
  if (is_permutation({0,3,1,2})) {
    return fmt::frac("1", fmt::frac_parens(fmt::diff("1", notation, HSpacing::dense)));
  }
  FATAL(absl::StrCat("Unknown permutation ", to_string(value), " of ", to_string(initial)));
}


void CompoundRatio::check() const {
  for (const auto& points : loops_) {
    CHECK(points.size() % 2 == 0);
    CHECK_GE(points.size(), kMinCompoundRatioComponents);
  }
}

static void cancel_fraction(std::multiset<Delta>& a, std::multiset<Delta>& b) {
  for (auto a_it = a.begin(); a_it != a.end();) {
    const auto b_it = b.find(*a_it);
    if (b_it != b.end()) {
      a_it = a.erase(a_it);
      b.erase(b_it);
    } else {
      ++a_it;
    }
  }

}

// Optimization potential: store CompoundRatio as numerator+denominator, convert
// to loops only for outputting.
void CompoundRatio::normalize() {
  check();

  // Convert to fraction
  std::multiset<Delta> numerator;
  std::multiset<Delta> denominator;
  for (auto& loop : loops_) {
    const int n = loop.size();
    for (int i : range(n)) {
      Delta d(loop[i], loop[(i+1)%n]);
      (i%2 == 0 ? numerator : denominator).insert(d);
    }
  }

  // Simplify fraction
  cancel_fraction(numerator, denominator);
  cancel_fraction(denominator, numerator);  // TODO: Check if the other direction is needed

  // Convert back to loops
  CHECK_EQ(numerator.size(), denominator.size());
  loops_.clear();
  while (!numerator.empty()) {
    loops_.push_back({});
    auto& loop = loops_.back();
    const auto num_it = numerator.begin();
    const int first_point = num_it->a();
    int current_point = num_it->b();
    numerator.erase(num_it);
    loop.push_back(first_point);
    auto* container = &denominator;
    while (current_point != first_point) {
      loop.push_back(current_point);
      const auto it = absl::c_find_if(*container, [&](Delta d) {
        return d.contains(current_point);
      });
      CHECK(it != container->end());
      current_point = it->other_point(current_point);
      container->erase(it);
      container = (container == &numerator) ? &denominator : &numerator;
    }
    CHECK(container == &numerator);
  }
  CHECK(denominator.empty());

  // Finalize
  for (auto& loop : loops_) {
    normalize_loop(loop);
  }
  absl::c_sort(loops_);
  check();
}

// Note. There are other reducible expressions of the form (1 - CompoundRatio),
// but this is the only one relevant to our task.
std::optional<CompoundRatio> CompoundRatio::one_minus(const CompoundRatio& ratio) {
  if (ratio.loops_.size() == 1) {
    const std::vector<int>& l = ratio.loops_.front();
    if (l.size() == 4) {
      return CompoundRatio::from_loops({{l[0], l[2], l[1], l[3]}});
    }
  }
  return std::nullopt;
}

static constexpr int kCompressionSizeBump = 1;

void compress_compound_ratio(const CompoundRatio& ratio, Compressor& compressor) {
  const auto& loops = ratio.loops();
  // Compressor doesn't support zeroes, hence kCompressionSizeBump
  compressor.add_segment({int(loops.size()) + kCompressionSizeBump});
  for (const auto& l : loops) {
    compressor.add_segment(l);
  }
}

CompoundRatio uncompress_compound_ratio(Decompressor& decompressor) {
  std::vector<std::vector<int>> loops;
  const std::vector<int> size_vec = decompressor.next_segment();
  CHECK_EQ(size_vec.size(), kCompressionSizeBump);
  const int size = size_vec.front() - 1;
  for (EACH : range(size)) {
    loops.push_back(decompressor.next_segment());
  }
  return CompoundRatio::from_loops(std::move(loops));
}

CompoundRatioCompressed compress_compound_ratio(const CompoundRatio& ratio) {
  Compressor compressor;
  compress_compound_ratio(ratio, compressor);
  return std::move(compressor).result<CompoundRatioCompressed>();
}

CompoundRatio uncompress_compound_ratio(const CompoundRatioCompressed& data) {
  Decompressor decompressor(data);
  CompoundRatio ret = uncompress_compound_ratio(decompressor);
  CHECK(decompressor.done());
  return ret;
}

std::string to_string(const CompoundRatio& ratio) {
  return ratio.is_unity()
    ? fmt::unity()
    : str_join(
        ratio.loops(),
        "",
        [](const std::vector<int>& loop) {
          return fmt::brackets(str_join(loop, ","));
        }
      );
}
