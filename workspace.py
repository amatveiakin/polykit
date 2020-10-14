import format

from tensor import D, Product, Tensor
from lyndon import lyndon_factorize, to_lyndon_basis
from polylog import Li2


# format.set_enable_unicode(True)
# format.set_enable_monospace_font_correction(False)

# t = Tensor.from_list([
#      Product([D(1, 2), D(3, 4)]),
#      Product([D(3, 4), D(1, 2)]),
# ])
# print(str(t))

# t = Tensor.from_list([
#      Product([D(1, 2), D(2, 3)]),
#      Product([D(2, 3), D(3, 1)]),
#      Product([D(3, 1), D(1, 2)]),
# ])
# print(str(t))
# with open("out.txt", "w", encoding="utf-8") as f:
#     f.write(str(t))

t = Tensor(Li2(1, 2, 3, 4))
print(str(t))
