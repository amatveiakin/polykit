#pragma once

#include <vector>

#include "check.h"


template<typename First, typename Second>
std::vector<std::pair<First, Second>> zip(
    const std::vector<First>& first,
    const std::vector<Second>& second) {
  CHECK_EQ(first.size(), second.size());
  std::vector<std::pair<First, Second>> ret;
  for (int i : range(first.size())) {
    ret.push_back({first[i], second[i]});
  }
  return ret;
}

template<typename First, typename Second, typename F>
auto mapped_zip(
    const std::vector<First>& first,
    const std::vector<Second>& second,
    F&& func) {
  CHECK_EQ(first.size(), second.size());
  std::vector<std::decay_t<std::invoke_result_t<F, First, Second>>> ret;
  for (int i : range(first.size())) {
    ret.push_back(func(first[i], second[i]));
  }
  return ret;
}

template<typename First, typename Second>
std::pair<std::vector<First>, std::vector<Second>> unzip(
    const std::vector<std::pair<First, Second>>& both) {
  std::vector<First> first;
  std::vector<Second> second;
  for (const auto& element : both) {
    first.push_back(element.first);
    second.push_back(element.second);
  }
  return {std::move(first), std::move(second)};
}
