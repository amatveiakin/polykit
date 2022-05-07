#include "space_algebra.h"

#include "format.h"


std::string to_string(const SpaceVennRanks& ranks) {
  return absl::StrCat(
    fmt::parens(absl::StrCat(ranks.a(), ", ", ranks.b())), ", ",
    fmt::set_intersection(), " = ", ranks.intersected()
  );
}

std::string to_string(const SpaceMappingRanks& ranks) {
  return absl::StrCat(ranks.space(), " - ", ranks.image(), " = ", ranks.kernel());
}
