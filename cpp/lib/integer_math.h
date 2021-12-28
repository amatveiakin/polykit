#pragma once

#include <vector>


// Returns binomial coefficient (n \choose k)
int binomial(int n, int k);

// Returns all the ways how a positive integer can be written as a sum of positive integers, e.g.
//   5 => 5, 4+1, 3+2, 3+1+1, 2+2+1, 2+1+1+1, 1+1+1+1+1
// The version with `num_summands` returns only sums with that exact number of summands.
std::vector<std::vector<int>> get_partitions(int n);
std::vector<std::vector<int>> get_partitions(int n, int num_summands);
