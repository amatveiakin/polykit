from linear import Linear
from polylog_gen import X, cross_ratio, neg_cross_ratio, symbol_product
from tensor import D


# def Li2(a, b, c, d):
#     return symbol_product(
#         cross_ratio(a, b, c, d),
#         neg_cross_ratio(a, b, c, d)
#     )

# def mystic(x1, x2, x3, x4, x5, x6, func):
#     return symbol_product(
#         neg_cross_ratio(x1, x2, x3, x4),
#         func(x1, x4, x5, x6)
#     ) - symbol_product(
#         neg_cross_ratio(x3, x4, x5, x2),
#         func(x1, x2, x5, x6)
#     ) + symbol_product(
#         neg_cross_ratio(x3, x4, x5, x6),
#         func(x1, x2, x3, x6)
#     )

# def Li2_p6(x1, x2, x3, x4, x5, x6):
#     return mystic(x1, x2, x3, x4, x5, x6, neg_cross_ratio)

# def Li3(a, b, c, d):
#     return symbol_product(
#         cross_ratio(a, b, c, d),
#         cross_ratio(a, b, c, d),
#         neg_cross_ratio(a, b, c, d)
#     )

# def Li3_p6(x1, x2, x3, x4, x5, x6):
#     return symbol_product(
#         cross_ratio(x1, x2, x3, x4, x5, x6),
#         Li2_p6(x1, x2, x3, x4, x5, x6),
#     ) + mystic(x1, x2, x3, x4, x5, x6, Li2)

# def Li4(a, b, c, d):
#     return symbol_product(
#         cross_ratio(a, b, c, d),
#         cross_ratio(a, b, c, d),
#         cross_ratio(a, b, c, d),
#         neg_cross_ratio(a, b, c, d)
#     )

# def Li4_p6(x1, x2, x3, x4, x5, x6):
#     return symbol_product(
#         cross_ratio(x1, x2, x3, x4, x5, x6),
#         Li3_p6(x1, x2, x3, x4, x5, x6),
#     ) + mystic(x1, x2, x3, x4, x5, x6, Li3)

# def Li5(a, b, c, d):
#     return symbol_product(
#         cross_ratio(a, b, c, d),
#         cross_ratio(a, b, c, d),
#         cross_ratio(a, b, c, d),
#         cross_ratio(a, b, c, d),
#         neg_cross_ratio(a, b, c, d)
#     )

# def Li5_p6(x1, x2, x3, x4, x5, x6):
#     return symbol_product(
#         cross_ratio(x1, x2, x3, x4, x5, x6),
#         Li4_p6(x1, x2, x3, x4, x5, x6),
#     ) + mystic(x1, x2, x3, x4, x5, x6, Li4)
