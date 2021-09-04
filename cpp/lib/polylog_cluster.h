#pragma once

#include "Eigen/Core"

#include "delta.h"
#include "itertools.h"
#include "ptr_util.h"
#include "x.h"


using ClusterSpace = std::vector<std::shared_ptr<const DeltaExpr>>;
using ClusterCoSpace = std::vector<std::pair<std::shared_ptr<const DeltaExpr>, std::shared_ptr<const DeltaExpr>>>;

ClusterSpace CB1(const XArgs& xargs);
ClusterSpace CB2(const XArgs& xargs);
ClusterSpace CB3(const XArgs& xargs);
ClusterSpace CB4(const XArgs& xargs);
ClusterSpace CB5(const XArgs& xargs);

ClusterSpace CL4(const XArgs& xargs);
// TODO: Keep only one version
ClusterSpace CL5(const XArgs& xargs);
ClusterSpace CL5Alt(const XArgs& xargs);

ClusterCoSpace cluster_space_2(const XArgs& args);
ClusterCoSpace cluster_space_3(const XArgs& args);
ClusterCoSpace cluster_space_4(const XArgs& args);
ClusterCoSpace cluster_space_5(const XArgs& args);
ClusterCoSpace cluster_space_6(const XArgs& args);
ClusterCoSpace cluster_space_6_alt(const XArgs& args);
ClusterCoSpace cluster_space_6_via_l(const XArgs& args);

inline auto cluster_space(int weight) {
  switch (weight) {
    case 3: return &cluster_space_3;
    case 4: return &cluster_space_4;
    case 5: return &cluster_space_5;
    case 6: return &cluster_space_6;
  }
  FATAL(absl::StrCat("Unsupported weight in cluster_space_by_weight: ", weight));
}

Eigen::MatrixXd cluster_space_matrix(int weight, const XArgs& points, bool apply_comult);
Eigen::MatrixXd cluster_space_matrix_6_via_l(const XArgs& points, bool apply_comult);
