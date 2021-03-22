import format
import linear
from util import args_to_iterable
from pybind import lyndon, x, delta, projection, polylog_qli


Linear = linear.Linear

to_lyndon_basis = lyndon.to_lyndon_basis

X = x.X
Inf = x.Inf

Delta = delta.Delta
LazyDeltaExpr = delta.LazyDeltaExpr

LazyProjectionExpr = projection.LazyProjectionExpr
project_on = projection.project_on

class DeltaExpr(Linear):
    def obj_to_str(self, obj):
        return format.otimes.join([str(o) for o in obj])

class ProjectionExpr(Linear):
    def obj_to_str(self, obj):
        return "({})".format(", ".join([str(o) for o in obj]))

def eval_expr(lazy_expr):
    if isinstance(lazy_expr, LazyDeltaExpr):
        return DeltaExpr.from_pairs(delta.eval_lazy_delta(lazy_expr).data)
    if isinstance(lazy_expr, LazyProjectionExpr):
        return ProjectionExpr.from_pairs(projection.eval_lazy_projection(lazy_expr).data)
    raise TypeError("`eval_expr` expected a lazy expression type, got {}".format(
        type(lazy_expr).__name__
    ))

QLi = polylog_qli.QLi
QLiNeg = polylog_qli.QLiNeg
QLiSymm = polylog_qli.QLiSymm

def project_on_x1 (expr): return project_on(1,  expr)
def project_on_x2 (expr): return project_on(2,  expr)
def project_on_x3 (expr): return project_on(3,  expr)
def project_on_x4 (expr): return project_on(4,  expr)
def project_on_x5 (expr): return project_on(5,  expr)
def project_on_x6 (expr): return project_on(6,  expr)
def project_on_x7 (expr): return project_on(7,  expr)
def project_on_x8 (expr): return project_on(8,  expr)
def project_on_x9 (expr): return project_on(9,  expr)
def project_on_x10(expr): return project_on(10, expr)
def project_on_x11(expr): return project_on(11, expr)
def project_on_x12(expr): return project_on(12, expr)
def project_on_x13(expr): return project_on(13, expr)
def project_on_x14(expr): return project_on(14, expr)
def project_on_x15(expr): return project_on(15, expr)

def QLi1(*points): return QLi(1, args_to_iterable(points))
def QLi2(*points): return QLi(2, args_to_iterable(points))
def QLi3(*points): return QLi(3, args_to_iterable(points))
def QLi4(*points): return QLi(4, args_to_iterable(points))
def QLi5(*points): return QLi(5, args_to_iterable(points))
def QLi6(*points): return QLi(6, args_to_iterable(points))
def QLi7(*points): return QLi(7, args_to_iterable(points))
def QLi8(*points): return QLi(8, args_to_iterable(points))

def QLiNeg1(*points): return QLiNeg(1, args_to_iterable(points))
def QLiNeg2(*points): return QLiNeg(2, args_to_iterable(points))
def QLiNeg3(*points): return QLiNeg(3, args_to_iterable(points))
def QLiNeg4(*points): return QLiNeg(4, args_to_iterable(points))
def QLiNeg5(*points): return QLiNeg(5, args_to_iterable(points))
def QLiNeg6(*points): return QLiNeg(6, args_to_iterable(points))
def QLiNeg7(*points): return QLiNeg(7, args_to_iterable(points))
def QLiNeg8(*points): return QLiNeg(8, args_to_iterable(points))

def QLiSymm1(*points): return QLiSymm(1, args_to_iterable(points))
def QLiSymm2(*points): return QLiSymm(2, args_to_iterable(points))
def QLiSymm3(*points): return QLiSymm(3, args_to_iterable(points))
def QLiSymm4(*points): return QLiSymm(4, args_to_iterable(points))
def QLiSymm5(*points): return QLiSymm(5, args_to_iterable(points))
def QLiSymm6(*points): return QLiSymm(6, args_to_iterable(points))
def QLiSymm7(*points): return QLiSymm(7, args_to_iterable(points))
def QLiSymm8(*points): return QLiSymm(8, args_to_iterable(points))
