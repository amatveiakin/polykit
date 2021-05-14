import re

from .delta import Inf, d_expr_dimension
from .linear import Linear
from .lyndon import to_lyndon_basis
from .util import get_one_item


def word_expr_weight(expr):
    return len(get_one_item(expr.items())[0])

def word_expr_max_char(expr):
    return max([max(word) for word, _ in expr.items()])

def words_with_n_distinct_chars(expr, min_distinct):
    return expr.filtered_obj(lambda word: len(set(word)) >= min_distinct)


# Replaces each letter c with index_map[c]
def word_substitute(
        word,       # Tuple[int]
        index_map,  # int -> int
    ):
    return tuple([index_map.get(c, c) for c in word])

# For each word, replaces each letter c with index_map[c]
def word_expr_substitute(
        expr,       # Linear[word],  word is Tuple[int]
        index_map,  # int -> int
    ):
    ret = Linear()
    for word, coeff in expr.items():
        word_new = word_substitute(word, index_map)
        if not Inf in word_new:
            ret += Linear({word_new: coeff})
    return ret

def _word_to_template_impl(word, index_map):
    next_index = 0 if len(index_map) == 0 else max(index_map.values()) + 1
    for c in word:
        if not c in index_map:
            index_map[c] = next_index
            next_index += 1
    return word_substitute(word, index_map)

# Converts word to a standard form modulo substitutions
def word_to_template(word):
    return _word_to_template_impl(word, {})

def word_expr_to_template(expr, index_map=None):
    if index_map is None:
        index_map = {}
    return expr.mapped_obj(lambda w: _word_to_template_impl(w, index_map))
