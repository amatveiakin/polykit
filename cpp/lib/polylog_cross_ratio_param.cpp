#include "polylog_cross_ratio_param.h"

#include "format.h"
#include "util.h"


static void normalize_loop(std::vector<int>& loop) {
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

void CompoundRatio::normalize() {
  check();
  bool simplification_found = false;
  do {
    simplification_found = false;
    for (int i = 0; i < loops_.size(); ++i) {
      for (int j = i+1; j < loops_.size(); ++j) {
        std::vector<int> a = loops_[i];
        std::vector<int> b = loops_[j];
        std::vector<int> common;
        const int an = a.size();
        const int bn = b.size();
        absl::c_set_intersection(sorted(a), sorted(b), std::back_inserter(common));
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
          CHECK_EQ(a[ap2], c2) << list_to_string(a) << " + " << list_to_string(b);
          CHECK_EQ(b[bp2], c2) << list_to_string(a) << " + " << list_to_string(b);
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

// TODO: Are there no other equations?
std::optional<CompoundRatio> CompoundRatio::one_minus() const {
  if (loops_.size() == 1) {
    const std::vector<int>& l = loops_.front();
    if (l.size() == 4) {
      return CompoundRatio({{l[0], l[2], l[1], l[3]}});
    }
  }
  return std::nullopt;
}


Word lira_param_to_key(const LiraParam& param) {
  Compressor compressor;
  compressor.add_segment({param.foreweight()});
  compressor.add_segment(param.weights());
  for (const auto& r : param.ratios()) {
    r.compress(compressor);
  }
  return Word(std::move(compressor).result());
}

LiraParam key_to_lira_param(const Word& word) {
  Decompressor decompressor(word.span());
  std::vector<int> foreweight = decompressor.next_segment();
  CHECK_EQ(foreweight.size(), 1);
  std::vector<int> weights = decompressor.next_segment();
  std::vector<CompoundRatio> ratios;
  while (!decompressor.done()) {
    ratios.push_back(CompoundRatio::from_compressed(decompressor));
  }
  return LiraParam(foreweight.front(), std::move(weights), std::move(ratios));
}

std::string lira_param_function_name(const LiraParam& param) {
  return fmt::lrsub_num(param.foreweight(), "Li", param.weights());
}

std::string to_string(const LiraParam& param) {
  return fmt::function(
    lira_param_function_name(param),
    mapped(param.ratios(), [](const CompoundRatio& ratio){
      return to_string(ratio);
    }),
    HSpacing::sparse
  );
}
