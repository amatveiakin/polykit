#pragma once

#include "word.h"


constexpr int kFormalSymbolSentinel = 0;

// Neither weights nor points can contain the kFormalSymbolSentinel.
Word formal_symbol(
    const std::vector<int>& weights,
    const std::vector<std::vector<int>>& points);

void decode_formal_symbol(
    const Word& symbol,
    std::vector<int>& weights,
    std::vector<std::vector<int>>& points);

std::string formal_symbol_to_string(const Word& symbol);
