#include "polylog_li_param.h"

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "format.h"
#include "util.h"


// Compressor doesn't support zeroes, hence kCompressionForeweightBump
static constexpr int kCompressionForeweightBump = 2;

LiParamCompressed li_param_to_key(const LiParam& param) {
  Compressor compressor;
  compressor.add_segment({param.foreweight() + kCompressionForeweightBump});
  compressor.add_segment(param.weights());
  for (const auto& p : param.points()) {
    compressor.add_segment(p);
  }
  return std::move(compressor).result<LiParamCompressed>();
}

LiParam key_to_li_param(const LiParamCompressed& key) {
  Decompressor decompressor(key);
  std::vector<int> foreweight_vec = decompressor.next_segment();
  CHECK_EQ(foreweight_vec.size(), 1);
  const int foreweight = foreweight_vec.front() - kCompressionForeweightBump;
  std::vector<int> weights = decompressor.next_segment();
  std::vector<std::vector<int>> points;
  while (!decompressor.done()) {
    points.push_back(decompressor.next_segment());
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
