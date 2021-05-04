import re

from .linear import Linear
from .lyndon import to_lyndon_basis
from .tensor import Inf, d_expr_dimension
from .util import get_one_item


def W(*args):
    return Linear({args: 1})

# Projects a linear combination of tensor products of (x_i - x_j) on x_index.
# Keeps only the products that contain x_index in every bracket. Turns each
# such product into a word containing the other indices.
#
# Example:
#     + (x1 - x2)*(x1 - x3)
#     + (x1 - x2)*(x2 - x3)
#     - (x1 - x2)*(x1 - x4)
#   projected on index == 1 gives:
#     (2, 3) - (2, 4)
#
def project_on_xi(
        expr,   # Linear[D]
        index,  # int
    ):
    words = Linear()
    for multipliers, coeff in expr.items():
        w = []
        for d in multipliers:
            if d.a == index:
                w.append(d.b)
            elif d.b == index:
                w.append(d.a)
            else:
                break
        if len(w) == len(multipliers):
            words += Linear({tuple(w): coeff})
    return words

def project_on_x1(expr):
    return project_on_xi(expr, 1)


def word_expr_weight(expr):
    return len(get_one_item(expr.items())[0])

def word_expr_max_char(expr):
    return max([max(word) for word, _ in expr.items()])

def words_with_n_distinct_chars(expr, min_distinct):
    return expr.filtered_obj(lambda word: len(set(word)) >= min_distinct)


# Equivalent to "Tensor(expr) -> lyndon_basis -> summands == Linear()", but faster
def is_zero(expr):
    if expr == Linear():
        return True
    num_points = d_expr_dimension(expr)
    for i in range(1, num_points - 2):
        if to_lyndon_basis(project_on_xi(expr, i)) != Linear():
            return False
    return True


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
