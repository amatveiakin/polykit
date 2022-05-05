#include "bindings.h"

#include <linbox/linbox-config.h>
#include <givaro/givrational.h>
#include <linbox/ring/modular.h>
#include <linbox/matrix/sparse-matrix.h>
#include <linbox/solutions/rank.h>

int linbox_matrix_rank(int num_rows, int num_cols, int num_triplets, const Triplet* triplets) {
  Givaro::QField<Givaro::Rational> QQ;
  LinBox::SparseMatrix<Givaro::QField<Givaro::Rational>, LinBox::SparseMatrixFormat::SparseSeq> matrix(
    QQ, num_rows, num_cols
  );
  for (int i = 0; i < num_triplets; ++i) {
    const Triplet& t = triplets[i];
    matrix.setEntry(t.row, t.col, t.value);
  }
  size_t rank;
  LinBox::rank(rank, matrix, LinBox::Method::SparseElimination());
  return rank;
}
