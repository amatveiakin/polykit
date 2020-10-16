from linear import Linear, tensor_product, tensor_product_many
from shuffle import shuffle_product_many
from tensor import D


def X(a, b):
    d = D(a, b)
    return Linear() if d.is_nil() else Linear({(d,): 1})


# Returns
#   (a-b)(c-d)
#   ----------
#   (a-d)(c-b)
# as symbol
def cross_ratio(a, b, c, d):
    return X(a, b) + X(c, d) - X(a, d) - X(c, b)

def cross_ratio_6(a, b, c, d, e, f):
    return X(a, b) + X(c, d) + X(e, f) - X(b, c) - X(d, e) - X(a, f)

def cross_ratio_n(v):
    ret = Linear()
    n = len(v)
    for i in range(n):
        sign = 1 if i % 2 == 0 else -1
        ret += sign * X(v[i], v[(i+1)%n])
    return ret

# Returns
#   1 - cross_ratio(a, b, c, d)
# as symbol
def neg_cross_ratio(a, b, c, d):
    return cross_ratio(a, c, b, d)


def symbol_product(lhs, rhs):
    return tensor_product(lhs, rhs, lambda a, b: a + b)

def symbol_product_many(symbols):
    return tensor_product_many(symbols, lambda a, b: a + b)


# def gen_Li_n_point_min_weight(n):
#     return _gen_Li_n_point_min_weight_impl(1, n)

# def _gen_Li_4_point(a, b, c, d):
#     return (
#         neg_cross_ratio(a, b, c, d)
#         if a % 2 == 1 else
#         neg_cross_ratio(b, c, d, a)
#     )

# def _gen_Li_n_point_min_weight_impl(a, d):
#     num_points = d - a + 1
#     assert num_points >= 4 and num_points % 2 == 0, f"Bad number of points: {num_points}"
#     for b in range(a+1, d, 2):
#         for c in range(b+1, d, 2):
#             sign = 1
#             parent = _gen_Li_4_point(a, b, c, d)
#             children = []
#             if b > a + 1:
#                 children.append(_gen_Li_n_point_min_weight_impl(a, b))
#             if c > b + 1:
#                 children.append(_gen_Li_n_point_min_weight_impl(b, c))
#                 sign *= -1
#             if d > c + 1:
#                 children.append(_gen_Li_n_point_min_weight_impl(c, d))
#             print("### children:\n" + str(children))
#             if len(children) > 0:
#                 shuffle = shuffle_product_many(children)
#                 print("### shuffle:\n" + str(shuffle))
#                 return symbol_product_many([shuffle, parent])
#             else:
#                 return parent
#             # return (
#             #     symbol_product_many([shuffle_product_many(children), parent])
#             #     if len(children) > 0
#             #     else parent
#             # )

def gen_Li(num_points, weight):
    return _gen_Li_impl(list(range(1, num_points+1)), weight)

def _gen_Li_4_point(a, b, c, d):
    return (
        neg_cross_ratio(a, b, c, d)
        if a % 2 == 1 else
        -neg_cross_ratio(b, c, d, a)
    )

def _gen_Li_impl(points, weight):
    num_points = len(points)
    assert num_points >= 4 and num_points % 2 == 0, f"Bad number of points: {num_points}"
    min_weight = (num_points - 2) // 2
    assert weight >= min_weight, f"Weight {weight} is less than minimum weight {min_weight}"
    if weight == min_weight:
        if num_points == 4:
            return _gen_Li_4_point(*points)
        else:
            ret = Linear()
            for i in range(num_points - 3):
                ret += symbol_product(
                    _gen_Li_4_point(*points[i:i+4]),
                    _gen_Li_impl(points[:i+1] + points[i+3:], weight - 1),
                )
            return ret
    else:
        ret = symbol_product(
            cross_ratio_n(points),
            _gen_Li_impl(points, weight - 1),
        )
        if num_points > 4:
            for i in range(num_points - 3):
                ret += symbol_product(
                    _gen_Li_4_point(*points[i:i+4]),
                    _gen_Li_impl(points[:i+1] + points[i+3:], weight - 1),
                )
        return ret
