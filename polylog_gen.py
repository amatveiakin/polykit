from dataclasses import dataclass

from linear import Linear
from tensor import D, X, d_expr_substitute, symbol_product
from util import args_to_iterable, rotate_list


def cross_ratio(*indexed_points):
    v = args_to_iterable(indexed_points)
    ret = Linear()
    n = len(v)
    for i in range(n):
        ret += (-1)**i * X(v[i], v[(i+1)%n])
    return ret

# Returns
#   1 - cross_ratio(a, b, c, d)
# as symbol
def neg_cross_ratio(a, b, c, d):
    return cross_ratio(a, c, b, d)


_li_cache = {}

# Generates a polylog of a given weight on a given set of points.
# Points can be a list of points or a number, in which case values
# from 1 to points are used.
def Li(weight, points):
    assert isinstance(points, (int, list, tuple))
    num_points = points if isinstance(points, int) else len(points)
    cache_key = (weight, num_points)
    asc_indices = list(range(1, num_points + 1))
    point_indices = asc_indices if isinstance(points, int) else points
    index_map = {
        asc_indices[i]: point_indices[i]
        for i in range(num_points)
    }
    asc_expr = None
    # print(f"Generating Li{weight}({num_points})... ", end="")
    if cache_key in _li_cache:
        asc_expr = _li_cache[cache_key].copy()
        # print("done (cached)")
    else:
        asc_expr = _Li_impl(weight, asc_indices)
        _li_cache[cache_key] = asc_expr.copy()
        # print("done")
    return d_expr_substitute(
        asc_expr,
        index_map
    ).annotated_with_function(f"Li{weight}", point_indices)


def Li2(*points):
    return Li(2, args_to_iterable(points))

def Li3(*points):
    return Li(3, args_to_iterable(points))

def Li4(*points):
    return Li(4, args_to_iterable(points))

def Li5(*points):
    return Li(5, args_to_iterable(points))

def Li6(*points):
    return Li(6, args_to_iterable(points))

def Li7(*points):
    return Li(7, args_to_iterable(points))

def Li8(*points):
    return Li(8, args_to_iterable(points))


def Li_sym_6_points(weight, points):
    assert isinstance(points, (int, list, tuple))
    num_points = points if isinstance(points, int) else len(points)
    assert num_points == 6
    if isinstance(points, int):
        points = list(range(1, num_points + 1))
    x1,x2,x3,x4,x5,x6 = points
    return (
        + Li(weight, [x1,x2,x3,x4,x5,x6])
        - Li(weight, [x1,x2,x3,x4])
        - Li(weight, [x3,x4,x5,x6])
        - Li(weight, [x5,x6,x1,x2])
    ).without_annotations().annotated_with_function(f"Li{weight}_sym", points)

def Li2_sym(*points):
    return Li_sym_6_points(2, args_to_iterable(points))

def Li3_sym(*points):
    return Li_sym_6_points(3, args_to_iterable(points))

def Li4_sym(*points):
    return Li_sym_6_points(4, args_to_iterable(points))

def Li5_sym(*points):
    return Li_sym_6_points(5, args_to_iterable(points))

def Li6_sym(*points):
    return Li_sym_6_points(6, args_to_iterable(points))

def Li7_sym(*points):
    return Li_sym_6_points(7, args_to_iterable(points))

def Li8_sym(*points):
    return Li_sym_6_points(8, args_to_iterable(points))


def _Li_4_point(points):
    assert len(points) == 4
    return (
        neg_cross_ratio(*points)
        if points[0] % 2 == 1 else
        -neg_cross_ratio(*rotate_list(points, 1))
    )

# _li_impl_cache = {}
# def _Li_impl(weight, points):
#     # The only place where particular point values are used is `_Li_4_point`,
#     # and only sign matters there
#     cache_key = tuple([weight] + [p % 2 for p in points])
#     if cache_key in _li_impl_cache:
#         ret = _li_impl_cache[cache_key]
#         ...  # TODO: index mapping
#         return ret
#     else:
#         ret = _Li_impl_no_cache(weight, points)
#         _li_impl_cache[cache_key] = ret
#         return ret

def _Li_impl(weight, points):
    num_points = len(points)
    assert num_points >= 4 and num_points % 2 == 0, f"Bad number of points: {num_points}"
    min_weight = (num_points - 2) // 2
    assert weight >= min_weight, f"Weight {weight} is less than minimum weight {min_weight}"
    def subsums():
        ret = Linear()
        for i in range(num_points - 3):
            ret += symbol_product(
                _Li_4_point(points[i:i+4]),
                _Li_impl(weight - 1, points[:i+1] + points[i+3:]),
            )
        return ret
    if weight == min_weight:
        if num_points == 4:
            return _Li_4_point(points)
        else:
            return subsums()
    else:
        ret = symbol_product(
            cross_ratio(points),
            _Li_impl(weight - 1, points),
        )
        if num_points > 4:
            ret += subsums()
        return ret
