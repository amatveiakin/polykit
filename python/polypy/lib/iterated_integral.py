from .delta import X, symbol_product
from .linear import Linear
from .util import removed_indices


def I(*points):
    num_points = len(points)
    assert num_points >= 3, f"Bad number of points: {num_points}"
    if num_points == 3:
        return _I_3_point(points)
    else:
        ret = Linear()
        for i in range(num_points - 2):
            ret += symbol_product(
                _I_3_point(points[i:i+3]),
                I(*removed_indices(points, (i+1,))),
            )
        return ret


def _I_3_point(points):
    a, b, c = points
    return X(c, b) - X(b, a)
