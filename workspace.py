import numpy as np

import format

from tensor import Inf, D, Product, Tensor
from lyndon import lyndon_factorize, to_lyndon_basis
from enumerator import Enumerator
from polylog import *
from shuffle import *
from util import *
from word_algebra import *


# format.set_enable_unicode(True)
# format.set_enable_monospace_font_correction(False)

# t = Tensor.from_list([
#      Product([D(1, 2), D(1, 3)]),
#      Product([D(1, 3), D(2, 3)]),
#      Product([D(2, 3), D(1, 2)]),
# ])
# print(str(t))
# with open("out.txt", "w", encoding="utf-8") as f:
#     f.write(str(t))

# t = Tensor(Li2(1, 2, 3, 4))
# print(str(t))

# t = Tensor(Li2_p6(1, 2, 3, 4, 5, 6))
# print(str(t))
# print("----")
# q = Tensor(Li2(1, 3, 5, 6) - Li2(1, 4, 5, 6) - Li2(2, 3, 5, 6) + Li2(2, 4, 5, 6))
# print(str(q))
# print("----")
# print(t.summands - q.summands)

# t = Tensor(Li5_p6(1, 2, 3, 4, 5, 6))
# # print(t)
# t.check_criterion()
# exit()

l = Li5_p6(1, 2, 3, 4, 5, 6)
# l = Li5_p6(1, 2, 3, 4, Inf, 6) + Li5_p6(1, 2, 3, Inf, 4, 6)
words_before_lyndon = project_on_xi(l, 1)
words = to_lyndon_basis(words_before_lyndon)
# print(f"Before Lyndon - {len(words_before_lyndon)} terms:\n{words_before_lyndon}\n")
print(f"After Lyndon - {len(words)} terms:\n{words}\n")
# words_filtered = Linear({k : v for k, v in words.items() if len(set(k)) >= 4})
# print(f"After Lyndon, at least 4 different - {len(words_filtered)} terms:\n{words_filtered}\n")


def is_representative_indices(indices):
    max_seen = 0  # Allow both 0 and 1 at the first position, because 0 has special meaning
    for idx in indices:
        if idx > max_seen + 1:
            return False
        max_seen = idx
    return True

# index_combinations = generate_all_words(alphabet_size=6, length=5)
# for indices in index_combinations:
#     if not is_representative_indices(indices):
#         continue
#     indices = tuple([1] + [Inf if idx == 0 else idx + 1 for idx in indices])
#     w = to_lyndon_basis(project_on_xi(Li5_p6(*indices), 1))
#     # if len(w) > 0:
#     #     print("Li5_p6(" + ", ".join(map(str, indices)) + ") =\n" + str(w))
#     if len(w) == 1:
#         print(f"Found {get_one_item(w.items())[0]}")

known_eliminations = set()
index_combinations = generate_all_words(alphabet_size=6, length=5)
expr = words.copy()
while True:
    # print(f"Expressions ({len(expr)}):\n{expr}")
    expr_new = Linear()
    for indices in index_combinations:
        if not is_representative_indices(indices):
            continue
        index_map = {i + 2 : Inf if indices[i] == 0 else indices[i] + 1 for i in range(len(indices))}
        # print("indices = (" + ", ".join(map(str, index_map.values())) + ")")
        words_new = to_lyndon_basis(word_expr_substitute(expr, index_map))
        # if len(words_new) > 0:
        #     print("Li5_p6(" + ", ".join(map(str, index_map.values())) + ") =\n" + str(words_new))
        if len(words_new) == 1:
            tmpl = word_to_template(get_one_item(words_new.items())[0])
            if not tmpl in known_eliminations:
                print(f"Found {tmpl}")
                known_eliminations.add(tmpl)
    for word, coeff in expr.items():
        if not word_to_template(word) in known_eliminations:
            expr_new[word] = coeff
    if len(expr_new) == len(expr):
        break
    expr = expr_new

# enumerator = Enumerator()
# index_combinations = generate_all_words(alphabet_size=6, length=5)
# expressions = []
# for indices in index_combinations:
#     if not is_representative_indices(indices):
#         continue
#     index_map = {i + 2 : Inf if indices[i] == 0 else indices[i] + 1 for i in range(len(indices))}
#     words_new = to_lyndon_basis(word_expr_substitute(words, index_map))
#     if len(words_new) == 0:
#         continue
#     expressions.append(words_new.map_obj(lambda w: enumerator.index(w)))

# mat = np.zeros((len(expressions), enumerator.num_objects), dtype=int)
# for k in range(len(expressions)):
#     expr = expressions[k]
#     for idx, coeff in expr.items():
#         mat[k][idx] = coeff
# print(f"mat:\n{mat}")

# mat = np.zeros((enumerator.num_objects, len(expressions)), dtype=int)
# for k in range(len(expressions)):
#     expr = expressions[k]
#     for idx, coeff in expr.items():
#         mat[idx][k] = coeff
# print(mat)

# rhs = np.zeros(enumerator.num_objects, dtype=int)
# goal_word = (2, 3, 4, 5, 6)
# assert enumerator.exists(goal_word)
# goal_idx = enumerator.index(goal_word)
# rhs[goal_idx] = 1
# print(f"rhs:\n{rhs}")

# result = np.linalg.lstsq(mat, rhs)
# print(f"\nResult:n{result}")
