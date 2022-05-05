#pragma once

// TODO: Switch to size_t for row/col when `std::os::raw::c_size_t` is stable:
//   https://github.com/rust-lang/rust/issues/88345
struct Triplet {
  int row;
  int col;
  int value;
};

extern "C"
int linbox_matrix_rank(int num_rows, int num_cols, int num_triplets, const Triplet* triplets);
