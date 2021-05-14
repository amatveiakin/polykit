#include <chrono>
#include <iomanip>
#include <iostream>

#include "cpp/lib/polylog_qli.h"
#include "cpp/lib/sequence_iteration.h"


int main(int argc, char *argv[]) {
  DeltaExpr expr;
  const int total_points = 9;
  for (int weight : range(3, 7)) {
    const int iteration = weight < 5 ? 10 : 3;
    const auto start = std::chrono::steady_clock::now();
    for (EACH : range(iteration)) {
      for (int num_args = 4; num_args <= total_points; num_args += 2) {
        for (const auto& seq : increasing_sequences(total_points, num_args)) {
          const auto args = mapped(seq, [](int x) { return x + 1; });
          const int sign = neg_one_pow(sum(args) + num_args / 2);
          expr += sign * QLiSymmVec(weight, args);
        }
      }
      CHECK(to_lyndon_basis(expr).is_zero());
    }
    const auto finish = std::chrono::steady_clock::now();
    const double time_sec = (finish - start) / std::chrono::milliseconds(1) / 1000. / iteration;
    std::cout << "(" << weight << ", " << std::fixed << std::setprecision(3) << time_sec << ")\n";
  }
}
