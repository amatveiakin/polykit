#include "polylog_param.h"

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "compression.h"
#include "format.h"
#include "util.h"


Word li_param_to_key(const LiParam& params) {
  Compressor compressor;
  compressor.add_segment(params.weights());
  for (const auto& p : params.points()) {
    compressor.add_segment(p);
  }
  return Word(std::move(compressor).result());
}

LiParam key_to_li_param(const Word& word) {
  Decompressor decompressor(word.span());
  std::vector<int> weights = decompressor.next_segment();
  std::vector<std::vector<int>> points;
  while (!decompressor.done()) {
    points.push_back(decompressor.next_segment());
  }
  return LiParam(std::move(weights), std::move(points));
}

std::string to_string(const LiParam& params) {
  return fmt::function(
    fmt::sub_num("Li", params.weights()),
    mapped(params.points(), [](const std::vector<int>& prod){
      return str_join(prod, "", fmt::var);
    })
  );
}
