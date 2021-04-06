#include "polylog_lira_param.h"

#include "format.h"
#include "util.h"


// Compressor doesn't support zeroes, hence kCompressionForeweightBump
static constexpr int kCompressionForeweightBump = 2;

LiraParamCompressed lira_param_to_key(const LiraParam& param) {
  Compressor compressor;
  compressor.add_segment({param.foreweight() + kCompressionForeweightBump});
  compressor.add_segment(param.weights());
  for (const auto& r : param.ratios()) {
    compress_compound_ratio(r, compressor);
  }
  return std::move(compressor).result<LiraParamCompressed>();
}

LiraParam key_to_lira_param(const LiraParamCompressed& key) {
  Decompressor decompressor(key);
  std::vector<int> foreweight_vec = decompressor.next_segment();
  CHECK_EQ(foreweight_vec.size(), 1);
  const int foreweight = foreweight_vec.front() - kCompressionForeweightBump;
  std::vector<int> weights = decompressor.next_segment();
  std::vector<CompoundRatio> ratios;
  while (!decompressor.done()) {
    ratios.push_back(uncompress_compound_ratio(decompressor));
  }
  CHECK(decompressor.done());
  return LiraParam(foreweight, std::move(weights), std::move(ratios));
}

std::string lira_param_function_name(int foreweight, const std::vector<int>& weights) {
  return fmt::lrsub_num(foreweight, fmt::opname("Li"), weights);
}

std::string lira_param_function_name(const LiraParam& param) {
  return lira_param_function_name(param.foreweight(), param.weights());
}

std::string to_string(const LiraParam& param) {
  return fmt::function(
    lira_param_function_name(param),
    mapped_to_string(param.ratios()),
    HSpacing::sparse
  );
}
