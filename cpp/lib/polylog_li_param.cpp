#include "polylog_li_param.h"

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "format.h"
#include "util.h"


LiParamCompressed li_param_to_key(const LiParam& param) {
  Compressor compressor;
  compressor.push_value(param.foreweight());
  compressor.push_segment(param.weights());
  for (const auto& p : param.points()) {
    compressor.push_segment(p);
  }
  return std::move(compressor).result<LiParamCompressed>();
}

LiParam key_to_li_param(const LiParamCompressed& key) {
  Decompressor decompressor(key);
  const int foreweight = decompressor.pop_value();
  std::vector<int> weights = decompressor.pop_segment();
  std::vector<std::vector<int>> points;
  while (!decompressor.done()) {
    points.push_back(decompressor.pop_segment());
  }
  CHECK(decompressor.done());
  return LiParam(foreweight, std::move(weights), std::move(points));
}

std::string to_string(const LiParam& param) {
  return fmt::function(
    fmt::lrsub_num(param.foreweight(), fmt::opname("Li"), param.weights()),
    mapped(param.points(), [](const std::vector<int>& prod){
      return str_join(prod, "", fmt::var);
    })
  );
}
