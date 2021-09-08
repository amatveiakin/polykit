#include "lib/triangulation.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "lib/range.h"
#include "lib/util.h"


// Compare number of triangulations to https://oeis.org/A000108
TEST(TriangulationTest, TriangulationsCount) {
  EXPECT_THAT(
    mapped(range_incl(2, 10), [](int n) {
      return get_triangulations(seq_incl(1, n)).size();
    }),
    testing::ElementsAre(1, 1, 2, 5, 14, 42, 132, 429, 1430)
  );
}

TEST(TriangulationTest, TriangulationQuadranglesCount) {
  for (const int num_vertices : range_incl(2, 10)) {
    const auto vertices = seq_incl(1, num_vertices);
    EXPECT_EQ(
      get_triangulations(vertices).size(),
      get_triangulation_quadrangles(vertices).size()
    );
  }
}
