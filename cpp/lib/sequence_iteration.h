// TODO: More options. As sparate functions or united single umbrella with
// params like "are elements unique", "should elements be increasing".
// Optimization potential: use generator style instead of actually
// building a vector.

#pragma once

#include <vector>


std::vector<std::vector<int>> all_squences(int alphabet_size, int length);
std::vector<std::vector<int>> increasing_squences(int alphabet_size, int length);
std::vector<std::vector<int>> increasing_squences(int alphabet_size);
