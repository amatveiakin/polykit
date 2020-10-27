#pragma once

#include "lib/word.h"


inline WordExpr W(std::initializer_list<int> data) {
  return WordExpr::single(Word(data));
}
