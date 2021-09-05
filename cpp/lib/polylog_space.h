#pragma once

#include "Eigen/Core"

#include "delta.h"
#include "itertools.h"
#include "ptr_util.h"
#include "x.h"


using PolylogSpace = std::vector<std::shared_ptr<const DeltaExpr>>;
using PolylogCoSpace = std::vector<std::pair<std::shared_ptr<const DeltaExpr>, std::shared_ptr<const DeltaExpr>>>;

PolylogSpace CB1(const XArgs& xargs);
PolylogSpace CB2(const XArgs& xargs);
PolylogSpace CB3(const XArgs& xargs);
PolylogSpace CB4(const XArgs& xargs);
PolylogSpace CB5(const XArgs& xargs);

PolylogSpace CL4(const XArgs& xargs);
// TODO: Keep only one version
PolylogSpace CL5(const XArgs& xargs);
PolylogSpace CL5Alt(const XArgs& xargs);

PolylogSpace L(int weight, const XArgs& xargs);
PolylogSpace L3(const XArgs& xargs);
PolylogSpace L4(const XArgs& xargs);

PolylogCoSpace polylog_space_2(const XArgs& args);
PolylogCoSpace polylog_space_3(const XArgs& args);
PolylogCoSpace polylog_space_4(const XArgs& args);
PolylogCoSpace polylog_space_5(const XArgs& args);
PolylogCoSpace polylog_space_6(const XArgs& args);
PolylogCoSpace polylog_space_6_alt(const XArgs& args);
PolylogCoSpace polylog_space_6_via_l(const XArgs& args);

inline auto polylog_space(int weight) {
  switch (weight) {
    case 3: return &polylog_space_3;
    case 4: return &polylog_space_4;
    case 5: return &polylog_space_5;
    case 6: return &polylog_space_6;
  }
  FATAL(absl::StrCat("Unsupported weight in polylog_space_by_weight: ", weight));
}

Eigen::MatrixXd polylog_space_matrix(int weight, const XArgs& points, bool apply_comult);
Eigen::MatrixXd polylog_space_matrix_6_via_l(const XArgs& points, bool apply_comult);
