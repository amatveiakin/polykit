import format
import linear
from util import args_to_iterable
from pybind import lyndon, x, delta, polylog_qli


Linear = linear.Linear

to_lyndon_basis = lyndon.to_lyndon_basis

X = x.X
Inf = x.Inf

Delta = delta.Delta
LazyDeltaExpr = delta.LazyDeltaExpr

class DeltaExpr(Linear):
    def obj_to_str(self, obj):
        return format.otimes.join([str(o) for o in obj])

def eval_expr(lazy_expr):
    if isinstance(lazy_expr, LazyDeltaExpr):
        return DeltaExpr.from_pairs(delta.eval_lazy_delta(lazy_expr).data)
    raise TypeError("`eval_expr` expected a lazy expression type, got {}".format(
        type(lazy_expr).__name__
    ))

QLi = polylog_qli.QLi
def QLi1(*points): return QLi(1, args_to_iterable(points))
def QLi2(*points): return QLi(2, args_to_iterable(points))
def QLi3(*points): return QLi(3, args_to_iterable(points))
def QLi4(*points): return QLi(4, args_to_iterable(points))
def QLi5(*points): return QLi(5, args_to_iterable(points))
def QLi6(*points): return QLi(6, args_to_iterable(points))
def QLi7(*points): return QLi(7, args_to_iterable(points))
def QLi8(*points): return QLi(8, args_to_iterable(points))

QLiNeg = polylog_qli.QLiNeg
def QLiNeg1(*points): return QLiNeg(1, args_to_iterable(points))
def QLiNeg2(*points): return QLiNeg(2, args_to_iterable(points))
def QLiNeg3(*points): return QLiNeg(3, args_to_iterable(points))
def QLiNeg4(*points): return QLiNeg(4, args_to_iterable(points))
def QLiNeg5(*points): return QLiNeg(5, args_to_iterable(points))
def QLiNeg6(*points): return QLiNeg(6, args_to_iterable(points))
def QLiNeg7(*points): return QLiNeg(7, args_to_iterable(points))
def QLiNeg8(*points): return QLiNeg(8, args_to_iterable(points))

QLiSymm = polylog_qli.QLiSymm
def QLiSymm1(*points): return QLiSymm(1, args_to_iterable(points))
def QLiSymm2(*points): return QLiSymm(2, args_to_iterable(points))
def QLiSymm3(*points): return QLiSymm(3, args_to_iterable(points))
def QLiSymm4(*points): return QLiSymm(4, args_to_iterable(points))
def QLiSymm5(*points): return QLiSymm(5, args_to_iterable(points))
def QLiSymm6(*points): return QLiSymm(6, args_to_iterable(points))
def QLiSymm7(*points): return QLiSymm(7, args_to_iterable(points))
def QLiSymm8(*points): return QLiSymm(8, args_to_iterable(points))
