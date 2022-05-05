// WIP. TODO: Safe Rust wrapper and a build script.

#pragma once

struct Triplet {
  int row;
  int col;
  int value;
};

extern "C"
int linbox_matrix_rank(int num_rows, int num_cols, int num_triplets, const Triplet* triplets);
