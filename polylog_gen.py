from linear import Linear, tensor_product
from tensor import D
from util import args_to_iterable


def X(a, b):
    d = D(a, b)
    return Linear() if d.is_nil() else Linear({(d,): 1})


# def cross_ratio(a, b, c, d):
#     return X(a, b) + X(c, d) - X(a, d) - X(c, b)

# def cross_ratio_6(a, b, c, d, e, f):
#     return X(a, b) + X(c, d) + X(e, f) - X(b, c) - X(d, e) - X(a, f)

def cross_ratio(*points):
    v = args_to_iterable(points)
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


def symbol_product(*multipliers):
    return tensor_product(multipliers, product=lambda a, b: a + b)


def Li(num_points, weight):
    return _Li_impl(list(range(1, num_points+1)), weight)

def _Li_4_point(a, b, c, d):
    return (
        neg_cross_ratio(a, b, c, d)
        if a % 2 == 1 else
        -neg_cross_ratio(b, c, d, a)
    )

def _Li_impl(points, weight):
    num_points = len(points)
    assert num_points >= 4 and num_points % 2 == 0, f"Bad number of points: {num_points}"
    min_weight = (num_points - 2) // 2
    assert weight >= min_weight, f"Weight {weight} is less than minimum weight {min_weight}"
    if weight == min_weight:
        if num_points == 4:
            return _Li_4_point(*points)
        else:
            ret = Linear()
            for i in range(num_points - 3):
                ret += symbol_product(
                    _Li_4_point(*points[i:i+4]),
                    _Li_impl(points[:i+1] + points[i+3:], weight - 1),
                )
            return ret
    else:
        ret = symbol_product(
            cross_ratio(points),
            _Li_impl(points, weight - 1),
        )
        if num_points > 4:
            for i in range(num_points - 3):
                ret += symbol_product(
                    _Li_4_point(*points[i:i+4]),
                    _Li_impl(points[:i+1] + points[i+3:], weight - 1),
                )
        return ret
