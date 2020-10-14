import format

from tensor import D,  Summand, Tensor
from lyndon import Lyndon, lyndon_factorize
from shuffle import shuffle_product, shuffle_product_many
from alphabet_mapping import AlphabetMapping


# format.set_enable_unicode(True)
# format.set_enable_monospace_font_correction(False)

# t = Tensor([
#      Summand([D(1, 2), D(2, 3)]),
#      Summand([D(2, 3), D(3, 1)]),
#      Summand([D(3, 1), D(1, 2)]),
# ])
# print(str(t))
# with open("out.txt", "w", encoding="utf-8") as f:
#     f.write(str(t))

# print(AlphabetMapping(4).from_alphabet_mapping)

# print(Lyndon(2, 3).words)
# print(lyndon_factorize([1, 0, 1, 0, 0, 1, 1, 1, 0, 1]))
# print(lyndon_factorize([1, 0, 1, 0, 1, 1, 1, 0, 0, 1]))

# print(shuffle_product((1, 2), (3, 4)))

# print(shuffle_product_many([(1, 2), (3, 4)]))
# print(shuffle_product_many([(1, 2), (3, 4), (5,)]))
# print(shuffle_product_many([(1, 2), (3, 4), (5, 6)]))

# shuffle = shuffle_product_many([(1, 2), (3, 4), (1, 4)])
# print(shuffle)
# print(len(shuffle))
# print(len(set(shuffle)))
