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
words_before_lyndon = project_on_xi(l, 1)
words = to_lyndon_basis(words_before_lyndon)
# print(f"Before Lyndon - {len(words_before_lyndon)} terms:\n{words_before_lyndon}\n")
print(f"After Lyndon - {len(words)} terms:\n{words}\n")
