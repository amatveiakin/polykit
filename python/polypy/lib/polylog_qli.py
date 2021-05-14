import itertools

from .delta import D, X, d_expr_substitute, symbol_product
from .linear import Linear
from .util import args_to_iterable, rotate_list


# def cross_ratio(*indexed_points):
#     v = args_to_iterable(indexed_points)
#     ret = Linear()
#     n = len(v)
#     for i in range(n):
#         ret += (-1)**i * X(v[i], v[(i+1)%n])
#     return ret

# # Returns
# #   1 - cross_ratio(a, b, c, d)
# # as symbol
# def neg_cross_ratio(a, b, c, d):
#     return cross_ratio(a, c, b, d)


# def _QLi_4_point(points):
#     assert len(points) == 4
#     return (
#         neg_cross_ratio(*points)
#         if points[0] % 2 == 1 else
#         -neg_cross_ratio(*rotate_list(points, 1))
#     )

# # _li_impl_cache = {}
# # def _QLi_impl(weight, points):
# #     # The only place where particular point values are used is `_QLi_4_point`,
# #     # and only sign matters there
# #     cache_key = tuple([weight] + [p % 2 for p in points])
# #     if cache_key in _li_impl_cache:
# #         ret = _li_impl_cache[cache_key]
# #         ...  # TODO: index mapping
# #         return ret
# #     else:
# #         ret = _QLi_impl_no_cache(weight, points)
# #         _li_impl_cache[cache_key] = ret
# #         return ret

# def _QLi_impl(weight, points):
#     num_points = len(points)
#     assert num_points >= 4 and num_points % 2 == 0, f"Bad number of points: {num_points}"
#     min_weight = (num_points - 2) // 2
#     assert weight >= min_weight, f"Weight {weight} is less than minimum weight {min_weight}"
#     def subsums():
#         ret = Linear()
#         for i in range(num_points - 3):
#             ret += symbol_product(
#                 _QLi_4_point(points[i:i+4]),
#                 _QLi_impl(weight - 1, points[:i+1] + points[i+3:]),
#             )
#         return ret
#     if weight == min_weight:
#         if num_points == 4:
#             return _QLi_4_point(points)
#         else:
#             return subsums()
#     else:
#         ret = symbol_product(
#             cross_ratio(points),
#             _QLi_impl(weight - 1, points),
#         )
#         if num_points > 4:
#             ret += subsums()
#         return ret


# _qli_cache = {}

# # Generates a polylog of a given weight on a given set of points.
# def QLi(weight, points):
#     assert isinstance(points, (list, tuple))
#     num_points = len(points)
#     cache_key = (weight, num_points)
#     asc_indices = list(range(1, num_points + 1))
#     index_map = {
#         asc_indices[i]: points[i]
#         for i in range(num_points)
#     }
#     asc_expr = None
#     if cache_key in _qli_cache:
#         asc_expr = _qli_cache[cache_key].copy()
#     else:
#         asc_expr = _QLi_impl(weight, asc_indices)
#         _qli_cache[cache_key] = asc_expr.copy()
#     return d_expr_substitute(asc_expr, index_map)


def cross_ratio(p):
    assert len(p) % 2 == 0
    ret = Linear()
    n = len(p)
    for i in range(n):
        ret += (-1)**i * X(p[i], p[(i+1)%n])
    return ret

# Symbol for (1 - cross_ratio(p))
def neg_cross_ratio(p):
    assert len(p) == 4
    return cross_ratio([p[0], p[2], p[1], p[3]])

def qli_node_func(p):
    assert len(p) == 4
    return (
        neg_cross_ratio([p[0][0], p[1][0], p[2][0], p[3][0]])
        if p[0][1] else
        -neg_cross_ratio([p[1][0], p[2][0], p[3][0], p[0][0]])
    )

def qli_impl(weight, points):
    num_points = len(points)
    assert num_points >= 4 and num_points % 2 == 0
    min_weight = (num_points - 2) / 2
    assert weight >= min_weight
    def subsums():
        nonlocal weight, points
        ret = Linear()
        for i in range(num_points - 3):
            foundation = points[:i+1] + points[i+3:]
            ret += symbol_product(qli_node_func(points[i:i+4]), qli_impl(weight - 1, foundation))
        return ret
    if weight == min_weight:
        if num_points == 4:
            return qli_node_func(points)
        else:
            return subsums()
    else:
        ret = symbol_product(cross_ratio([p[0] for p in points]), qli_impl(weight - 1, points))
        if num_points > 4:
            ret += subsums()
        return ret

def QLi(weight, points):
    tagged_points = []
    for i in range(len(points)):
        tagged_points.append((points[i], (i+1) % 2 == 1))
    return qli_impl(weight, tagged_points)


def QLi2(*points): return QLi(2, args_to_iterable(points))
def QLi3(*points): return QLi(3, args_to_iterable(points))
def QLi4(*points): return QLi(4, args_to_iterable(points))
def QLi5(*points): return QLi(5, args_to_iterable(points))
def QLi6(*points): return QLi(6, args_to_iterable(points))
def QLi7(*points): return QLi(7, args_to_iterable(points))
def QLi8(*points): return QLi(8, args_to_iterable(points))


def QLiSymm(weight, points):
    assert isinstance(points, (list, tuple))
    assert weight > 1, "QLiSymm is not defined for weight 1"
    num_points = len(points)
    assert num_points >= 4 and num_points % 2 == 0, f"Bad number of points: {num_points}"
    ret = Linear()
    num_pairs = num_points // 2
    for include_pair in itertools.product([False, True], repeat=num_pairs):
        num_pairs_included = sum(include_pair)
        if num_pairs_included < 2:
            continue
        sign = (-1) ** (num_pairs - num_pairs_included)
        args = []
        for pair_idx in range(num_pairs):
            if include_pair[pair_idx]:
                args.append(points[2*pair_idx  ])
                args.append(points[2*pair_idx+1])
        ret += sign * QLi(weight, args)
    return ret

def QLiSymm2(*points): return QLiSymm(2, args_to_iterable(points))
def QLiSymm3(*points): return QLiSymm(3, args_to_iterable(points))
def QLiSymm4(*points): return QLiSymm(4, args_to_iterable(points))
def QLiSymm5(*points): return QLiSymm(5, args_to_iterable(points))
def QLiSymm6(*points): return QLiSymm(6, args_to_iterable(points))
def QLiSymm7(*points): return QLiSymm(7, args_to_iterable(points))
def QLiSymm8(*points): return QLiSymm(8, args_to_iterable(points))
