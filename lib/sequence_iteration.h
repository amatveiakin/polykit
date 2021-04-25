// Idea. Unite everything under single umbrella with params like "are elements unique",
// "should elements be increasing".
// Optimization potential: use generator style instead of actually building a vector.

#pragma once

#include <vector>


std::vector<std::vector<int>> all_sequences(int alphabet_size, int length);
std::vector<std::vector<int>> increasing_sequences(int alphabet_size, int length);
std::vector<std::vector<int>> nondecreasing_sequences(int alphabet_size, int length);
std::vector<std::vector<int>> increasing_sequences(int alphabet_size);
