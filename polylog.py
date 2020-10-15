from linear import Linear, tensor_product_many
from tensor import D, Product, Tensor


def symbol_product_many(symbols):
    return tensor_product_many(symbols, lambda a, b: a + b)

def X(a, b):
    return Linear({(D(a, b),): 1})


# Returns
#   (a-b)(c-d)
#   ----------
#   (a-d)(c-b)
# as symbol
def cross_ratio(a, b, c, d):
    return X(a, b) + X(c, d) - X(a, d) - X(c, b)

def cross_ratio_6(a, b, c, d, e, f):
    return X(a, b) + X(c, d) + X(e, f) - X(b, c) - X(d, e) - X(a, f)

# Returns
#   1 - cross_ratio(a, b, c, d)
# as symbol
def neg_cross_ratio(a, b, c, d):
    return cross_ratio(a, c, b, d)


def Li2(a, b, c, d):
    return symbol_product_many([
        cross_ratio(a, b, c, d),
        neg_cross_ratio(a, b, c, d)
    ])

def mystic(x1, x2, x3, x4, x5, x6, func):
    return symbol_product_many([
        neg_cross_ratio(x1, x2, x3, x4),
        func(x1, x4, x5, x6)
    ]) - symbol_product_many([
        neg_cross_ratio(x3, x4, x5, x2),
        func(x1, x2, x5, x6)
    ]) + symbol_product_many([
        neg_cross_ratio(x3, x4, x5, x6),
        func(x1, x2, x3, x6)
    ])

def Li2_p6(x1, x2, x3, x4, x5, x6):
    return mystic(x1, x2, x3, x4, x5, x6, neg_cross_ratio)

def Li3(a, b, c, d):
    return symbol_product_many([
        cross_ratio(a, b, c, d),
        cross_ratio(a, b, c, d),
        neg_cross_ratio(a, b, c, d)
    ])

def Li3_inf(a, b, c):
    return symbol_product_many([
        X(a, b) - X(a, c),
        X(a, b) - X(a, c),
        X(b, c) - X(a, c),
    ])

def Li3_p6(x1, x2, x3, x4, x5, x6):
    return symbol_product_many([
        cross_ratio_6(x1, x2, x3, x4, x5, x6),
        Li2_p6(x1, x2, x3, x4, x5, x6),
    ]) + mystic(x1, x2, x3, x4, x5, x6, Li2)

def Li4(a, b, c, d):
    return symbol_product_many([
        cross_ratio(a, b, c, d),
        cross_ratio(a, b, c, d),
        cross_ratio(a, b, c, d),
        neg_cross_ratio(a, b, c, d)
    ])

def Li4_p6(x1, x2, x3, x4, x5, x6):
    return symbol_product_many([
        cross_ratio_6(x1, x2, x3, x4, x5, x6),
        Li3_p6(x1, x2, x3, x4, x5, x6),
    ]) + mystic(x1, x2, x3, x4, x5, x6, Li3)

def Li5(a, b, c, d):
    return symbol_product_many([
        cross_ratio(a, b, c, d),
        cross_ratio(a, b, c, d),
        cross_ratio(a, b, c, d),
        cross_ratio(a, b, c, d),
        neg_cross_ratio(a, b, c, d)
    ])

def Li5_p6(x1, x2, x3, x4, x5, x6):
    return symbol_product_many([
        cross_ratio_6(x1, x2, x3, x4, x5, x6),
        Li4_p6(x1, x2, x3, x4, x5, x6),
    ]) + mystic(x1, x2, x3, x4, x5, x6, Li4)
