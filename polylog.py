from linear import Linear, tensor_product
from tensor import D, Product, Tensor


def symbol_tensor_product(lhs, rhs):
    return tensor_product(lhs, rhs, lambda a, b: a + b)


# Returns
#   (a-b)(c-d)
#   ----------
#   (a-d)(c-b)
# as symbol
def cross_ratio(a, b, c, d):
    return Linear({
        (D(a, b),): 1,
        (D(c, d),): 1,
        (D(a, d),): -1,
        (D(c, b),): -1,
    })

# Returns
#   1 - cross_ratio(a, b, c, d)
# as symbol
def inv_cross_ratio(a, b, c, d):
    return cross_ratio(a, c, b, d)


def Li2(a, b, c, d):
    return symbol_tensor_product(
        cross_ratio(a, b, c, d),
        inv_cross_ratio(a, b, c, d)
    )
