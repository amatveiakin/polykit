#include "polylog_cross_ratio_param.h"

#include "format.h"
#include "util.h"


CompressedBlob lira_param_to_key(const LiraParam& param) {
  Compressor compressor;
  compressor.add_segment({param.foreweight()});
  compressor.add_segment(param.weights());
  for (const auto& r : param.ratios()) {
    r.compress(compressor);
  }
  return std::move(compressor).result();
}

LiraParam key_to_lira_param(const CompressedBlob& key) {
  Decompressor decompressor(key);
  std::vector<int> foreweight = decompressor.next_segment();
  CHECK_EQ(foreweight.size(), 1);
  std::vector<int> weights = decompressor.next_segment();
  std::vector<CompoundRatio> ratios;
  while (!decompressor.done()) {
    ratios.push_back(CompoundRatio::from_compressed(decompressor));
  }
  return LiraParam(foreweight.front(), std::move(weights), std::move(ratios));
}

std::string lira_param_function_name(int foreweight, const std::vector<int>& weights) {
  return fmt::lrsub_num(foreweight, "Li", weights);
}

std::string lira_param_function_name(const LiraParam& param) {
  return lira_param_function_name(param.foreweight(), param.weights());
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
