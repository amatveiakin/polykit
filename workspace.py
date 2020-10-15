import format

from tensor import D, Product, Tensor
from lyndon import lyndon_factorize, to_lyndon_basis
from polylog import *
from shuffle import *


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

# t = Tensor(Li_123456_2(1, 2, 3, 4, 5, 6))
# print(str(t))

# print("----")

# q = Tensor(Li2(1, 3, 5, 6) - Li2(1, 4, 5, 6) - Li2(2, 3, 5, 6) + Li2(2, 4, 5, 6))
# print(str(q))

# print("----")
# print(t.summands - q.summands)

# t = Tensor(Li3(1, 2, 3, 4))
# print(str(t))

# t = Tensor(Li3_inf(1, 2, 3))
# print(str(t))

l = Li5_p6(1, 2, 3, 4, 5, 6)
words = Linear()
for multipliers, coeff in l.items():
    w = []
    for d in multipliers:
        if d.a != 1:
            break
        w.append(d.b)
    if len(w) == len(multipliers):
        words += Linear({tuple(w): coeff})
words_lyndon = to_lyndon_basis(words)
print(f"Before Lyndon - {len(words)} terms:\n{words}\n")
print(f"After Lyndon - {len(words_lyndon)} terms:\n{words_lyndon}\n")

# print(to_lyndon_basis(Linear({
#     (0, 1, 2): 1,
#     (0, 2, 1): 1,
#     (2, 1, 0): 1
# })))

# print(to_lyndon_basis(Linear.count(shuffle_product_many([
#     (2, 1, 2),
#     (0, 3, 1),
#     (0, 1),
# ]))))

# print(to_lyndon_basis(Linear.count([
#     (0, 3, 1, 5, 1, 5, 5)
# ])))
