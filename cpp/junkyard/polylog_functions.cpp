#include "gamma.h"


// Some Casimir
GammaNCoExpr C33(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 4);
  const auto pl = [&](int p1, int p2, int p3) {
    return G(choose_indices_one_based(points, {p1,p2,p3}));
  };
  return
    + ncoproduct(pl(1,2,3), pl(1,2,4), pl(1,3,4))
    - ncoproduct(pl(2,3,4), pl(2,3,1), pl(2,4,1))
    + ncoproduct(pl(3,4,1), pl(3,4,2), pl(3,1,2))
    - ncoproduct(pl(4,1,2), pl(4,1,3), pl(4,2,3))
  ;
}

// Some Casimir
GammaNCoExpr C335(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 5);
  const auto c = [&](int p1, int p2, int p3, int p4) {
    return C33(choose_indices_one_based(points, {p1,p2,p3,p4}));
  };
  return
    + c(1,2,3,4)
    + c(2,3,4,5)
    + c(3,4,5,1)
    + c(4,5,1,2)
    + c(5,1,2,3)
  ;
}
