#include "ratio.h"

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

void CompoundRatio::normalize() {
  check();
  bool simplification_found = false;
  do {
    simplification_found = false;
    // Do no use `range` of here since `loops_.size()` changes during iteration.
    for (int i = 0; i < loops_.size(); ++i) {
      for (int j = i+1; j < loops_.size(); ++j) {
        std::vector<int> a = loops_.at(i);
        std::vector<int> b = loops_.at(j);
        const std::vector<int> common = set_intersection(a, b);
        const int an = a.size();
        const int bn = b.size();
        const int num_common = common.size();
        CHECK_LE(num_common, 2);
        if (num_common == 2) {
          simplification_found = true;
          int c1 = common[0];
          int c2 = common[1];
          if (a[(absl::c_find(a, c2) - a.begin() + 1) % an] != c1) {
            std::swap(c1, c2);
          }
          const int ap1 = absl::c_find(a, c1) - a.begin();
          const int ap2 = (ap1 + an - 1) % an;
          const int bp1 = absl::c_find(b, c1) - b.begin();
          const int bp2 = (bp1 + 1) % bn;
          CHECK_EQ(a[ap2], c2) << dump_to_string(a) << " + " << dump_to_string(b);
          CHECK_EQ(b[bp2], c2) << dump_to_string(a) << " + " << dump_to_string(b);
          if (bp1 < bp2) {
            a.insert(a.begin() + ap1, b.begin(), b.begin() + bp1);
            a.insert(a.begin() + ap1, b.begin() + bp2 + 1, b.end());
          } else {
            a.insert(a.begin() + ap1, b.begin() + bp2 + 1, b.begin() + bp1);
          }
          loops_[i] = a;
          loops_.erase(loops_.begin() + j);
        }
      }
    }
  } while (simplification_found);
  for (auto& l : loops_) {
    normalize_loop(l);
  }
  absl::c_sort(loops_);
  check();
}

// Note. There are other reducible expression of the form (1 - CompoundRatio),
// but this is the only one relevant to our task.
std::optional<CompoundRatio> CompoundRatio::one_minus(const CompoundRatio& ratio) {
  if (ratio.loops_.size() == 1) {
    const std::vector<int>& l = ratio.loops_.front();
    if (l.size() == 4) {
      return CompoundRatio({{l[0], l[2], l[1], l[3]}});
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
  return CompoundRatio(std::move(loops));
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
