from linear import Linear
from tensor import X, symbol_product
from util import removed_indices


def I(*points):
    num_points = len(points)
    assert num_points >= 3, f"Bad number of points: {num_points}"
    ret = Linear()
    if num_points == 3:
        ret = _I_3_point(points)
    else:
        for i in range(num_points - 2):
            ret += symbol_product(
                _I_3_point(points[i:i+3]),
                I(*removed_indices(points, (i+1,))),
            )
    return ret.annotated_with_function("I", points)


def _I_3_point(points):
    a, b, c = points
    return X(c, b) - X(b, a)
