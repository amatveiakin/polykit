#include "polylog_param.h"

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "format.h"
#include "util.h"


CompressedBlob li_param_to_key(const LiParam& param) {
  Compressor compressor;
  compressor.add_segment({param.foreweight()});
  compressor.add_segment(param.weights());
  for (const auto& p : param.points()) {
    compressor.add_segment(p);
  }
  return std::move(compressor).result();
}

LiParam key_to_li_param(const CompressedBlob& key) {
  Decompressor decompressor(key);
  std::vector<int> foreweight = decompressor.next_segment();
  CHECK_EQ(foreweight.size(), 1);
  std::vector<int> weights = decompressor.next_segment();
  std::vector<std::vector<int>> points;
  while (!decompressor.done()) {
    points.push_back(decompressor.next_segment());
  }
  return LiParam(foreweight.front(), std::move(weights), std::move(points));
}

std::string to_string(const LiParam& param) {
  return fmt::function(
    fmt::lrsub_num(param.foreweight(), "Li", param.weights()),
    mapped(param.points(), [](const std::vector<int>& prod){
      return str_join(prod, "", fmt::var);
    })
  );
}
