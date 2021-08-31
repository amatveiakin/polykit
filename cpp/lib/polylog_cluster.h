#pragma once

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

ClusterCoSpace cluster_weight2(const XArgs& args);
ClusterCoSpace cluster_weight3(const XArgs& args);
ClusterCoSpace cluster_weight4(const XArgs& args);
ClusterCoSpace cluster_weight5(const XArgs& args);
ClusterCoSpace cluster_weight6(const XArgs& args);
ClusterCoSpace cluster_weight6_alt(const XArgs& args);
