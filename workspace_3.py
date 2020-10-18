import time

import format

from tensor import Inf, Tensor
from lyndon import lyndon_factorize, to_lyndon_basis
from enumerator import Enumerator
from autosolve import *
from polylog_gen import *
from shuffle import *
from util import *
from word_algebra import *


def print_with_title(msg, expr):
    print(f"{msg} - {len(expr)} terms:\n{expr}\n")


weight = 5
num_point = 6
expr = Li(weight, num_point, project_on_x1).without_annotations()
expr_lyndon = to_lyndon_basis(expr)
print_with_title("Before Lyndon", expr)
print_with_title("After Lyndon", expr_lyndon)

threshold = min(weight, num_point - 2)
expr_filtered = Linear({k : v for k, v in expr_lyndon.items() if len(set(k)) >= threshold})
print_with_title(f"After Lyndon, at least {threshold} different", expr_filtered)

# print_with_title(f"Diffs", expr_lyndon.mapped_obj(lambda w: tuple([w[i+1] - w[i] for i in range(len(w)-1)])))


def is_good_word(word):
    last_char = None
    num_different_chars = 1
    for char in word:
        if last_char != None:
            if char < last_char:
                return False
            if char == last_char and char % 2 == 1:
                return False
            if char != last_char:
                num_different_chars += 1
        last_char = char
    return num_different_chars >= threshold

expected = set()
all_words = generate_all_words(num_point - 1, weight)
for w in all_words:
    w = tuple([c + 2 for c in w])
    if is_good_word(w):
        expected.add(w)

actual = set()
for w, _ in expr_filtered.items():
    actual.add(w)

# print(f"Expected:\n" + "\n".join(sorted([str(x) for x in expected])) + "\n")
# print(f"Actual:\n" + "\n".join(sorted([str(x) for x in actual])) + "\n")
print(f"Diff:\n" + "\n".join(sorted([str(x) for x in (expected - actual)])) + "\n")

print(f"Hypothesis = {expected == actual}\n")


# all_sorted = True
# for word, _ in expr_lyndon.items():
#     if word != tuple(sorted(word)):
#         all_sorted = False
# print(f"all_sorted = {all_sorted}\n")

# last_word = None
# for word, _ in sorted(expr_filtered.items()):
#     if last_word is not None:
#         diff = tuple([word[i] - last_word[i] for i in range(len(word))])
#         print(" ".join(["{:2}".format(d) for d in diff]))
#     last_word = word
