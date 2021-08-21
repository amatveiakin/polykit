#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "Eigen/Dense"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/delta_parse.h"
#include "lib/enumerator.h"
#include "lib/file_util.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/lexicographical.h"
#include "lib/lira_ones.h"
#include "lib/loops.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_li.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_lira.h"
#include "lib/polylog_via_correlators.h"
#include "lib/polylog_qli.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/pvector.h"
#include "lib/range.h"
#include "lib/sequence_iteration.h"
#include "lib/set_util.h"
#include "lib/shuffle.h"
#include "lib/snowpal.h"
#include "lib/summation.h"
#include "lib/theta.h"
#include "lib/zip.h"


class DeltaExprMatrixBuilder {
public:
  void add_expr(const DeltaExpr& expr) {
    sparse_columns.push_back({});
    auto& col = sparse_columns.back();
    expr.foreach([&](const auto& term, int coeff) {
      col.push_back({monoms_.index(term), coeff});
    });
  }

  // TODO: Try sparse matrices instead.
  Eigen::MatrixXd make_matrix() const {
    const int num_rows = monoms_.size();
    const int num_cols = sparse_columns.size();
    Eigen::MatrixXd mat = Eigen::MatrixXd::Zero(num_rows, num_cols);
    for (const int col : range(sparse_columns.size())) {
      for (const auto& [row, coeff] : sparse_columns[col]) {
        mat(row, col) = coeff;
      }
    }
    return mat;
  }

private:
  Enumerator<DeltaExpr::ObjectT> monoms_;
  std::vector<std::vector<std::pair<int, int>>> sparse_columns;  // col -> (row, value)
};

void describe(const DeltaExprMatrixBuilder& matrix_builder) {
  const auto& matrix = matrix_builder.make_matrix();
  Eigen::FullPivLU<Eigen::MatrixXd> lu_decomp(matrix);
  auto rank = lu_decomp.rank();
  std::cout << "(" << matrix.rows() << ", " << matrix.cols() << ") => " << rank << "\n";
}


#if 0
template<typename T>
class Permutations {
public:
  class Iterator {
  public:
    Iterator(std::vector<T> current_permutation, bool wrapped)
        : current_permutation_(std::move(current_permutation)), wrapped_(wrapped) {}
    Iterator(const Iterator&) = delete;  // avoid accidental (expensive) copying

    const std::vector<T>& operator*() const {
      return current_permutation_;
    }
    void operator++() {
      const wrapped_now = !absl::c_next_permutation(current_permutation_);
      wrapped_ = wrapped_ || wrapped_now;
    }
    bool operator==(const Iterator& other) {
      return current_permutation_ == other.current_permutation_ && wrapped_ = other.wrapped_;
    }

  private:
    std::vector<T> current_permutation_;
    bool wrapped_ = false;
  };

  Permutations(std::vector<T> elements)
      : current_permutation_(std::move(elements)) {}

  Iterator begin() const { return Iterator(current_permutation_, false); }
  Iterator end() const { return Iterator(current_permutation_, true); }

private:
  std::vector<T> current_permutation_;
};
#endif


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    // .set_expression_line_limit(FormattingConfig::kNoLineLimit)
    .set_annotation_sorting(AnnotationSorting::length)
  );



// # def prepare(expr):
// #     return to_lyndon_basis(expr)
// # for args in itertools.permutations([1,2,3,4]):
// #     expr = Corr(substitute([1,1,2,3,4], args))
// #     matrix_builder.add_expr(prepare(expr))
// # describe(matrix_builder)  # 9
// # for args in itertools.permutations([1,2,3,4]):
// #     expr = Corr(substitute([1,1,1,3,4], args))
// #     matrix_builder.add_expr(prepare(expr))
// # describe(matrix_builder)  # 21

  DeltaExprMatrixBuilder matrix_builder;

  std::vector args{1,2,3,4};
  do {
    const auto expr = Corr(choose_indices_one_based(args, {1,1,2,3,4}));
    matrix_builder.add_expr(to_lyndon_basis(expr));
  } while (absl::c_next_permutation(args));
  describe(matrix_builder);
  do {
    const auto expr = Corr(choose_indices_one_based(args, {1,1,1,3,4}));
    matrix_builder.add_expr(to_lyndon_basis(expr));
  } while (absl::c_next_permutation(args));
  describe(matrix_builder);
}
