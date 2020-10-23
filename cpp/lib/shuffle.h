#pragma once

#include <vector>

#include "util.h"
#include "word.h"


// Returns shuffle product of two words as a List of words.
// Rules:
//   1 ⧢ v = v
//   u ⧢ 1 = u
//   ua ⧢ vb = (u ⧢ vb)a + (ua ⧢ v)b
WordExpr shuffle_product(const Word& u, const Word& v);

// Returns  w1 ⧢ w2 ⧢ ... ⧢ wn
WordExpr shuffle_product(std::vector<Word> words);
