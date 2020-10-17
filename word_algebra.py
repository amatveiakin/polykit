from linear import Annotation, Linear
from tensor import Inf


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
    for multipliers, coeff in expr.without_annotations().items():
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

def project_on_x2(expr):
    return project_on_xi(expr, 2)

def project_on_x3(expr):
    return project_on_xi(expr, 3)

def project_on_x4(expr):
    return project_on_xi(expr, 4)

def project_on_x5(expr):
    return project_on_xi(expr, 5)

def project_on_x6(expr):
    return project_on_xi(expr, 6)

def project_on_x7(expr):
    return project_on_xi(expr, 7)

def project_on_x8(expr):
    return project_on_xi(expr, 8)

def project_on_x9(expr):
    return project_on_xi(expr, 9)

def project_on_x10(expr):
    return project_on_xi(expr, 10)


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
            ret += Linear({word_substitute(word, index_map): coeff})
    return ret

# Converts word to a standard form modulo substitutions
def word_to_template(word):
    index_map = {}
    next_index = 0
    for c in word:
        if not c in index_map:
            index_map[c] = next_index
            next_index += 1
    return word_substitute(word, index_map)
