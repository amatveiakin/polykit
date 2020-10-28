#include "polylog_cross_ratio_param.h"

#include "format.h"
#include "util.h"


Word lira_param_to_key(const LiraParam& params) {
  Compressor compressor;
  compressor.add_segment(params.weights());
  for (const auto& ratios : params.ratios()) {
    std::vector<int> segment;
    for (const CrossRatio& r : ratios) {
      append_vector(segment, r.indices());
    }
    compressor.add_segment(segment);
  }
  return Word(std::move(compressor).result());
}

LiraParam key_to_lira_param(const Word& word) {
  Decompressor decompressor(word.span());
  std::vector<int> weights = decompressor.next_segment();
  std::vector<std::vector<CrossRatio>> ratios;
  while (!decompressor.done()) {
    const std::vector<int> segment = decompressor.next_segment();
    CHECK(segment.size() % kCrossRatioElements == 0) << list_to_string(segment);
    const int num_ratios = segment.size() / kCrossRatioElements;
    ratios.push_back({});
    for (int i = 0; i < num_ratios; ++i) {
      ratios.back().push_back(CrossRatio(absl::MakeConstSpan(segment).subspan(
        i * kCrossRatioElements, kCrossRatioElements)));
    }
  }
  return LiraParam(std::move(weights), std::move(ratios));
}

std::string to_string(const LiraParam& params) {
  return fmt::function(
    fmt::sub_num("Li", params.weights()),
    mapped(params.ratios(), [](const std::vector<CrossRatio>& prod){
      return str_join(prod, "");
    })
  );
}
