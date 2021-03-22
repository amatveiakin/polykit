import format
import linear
from pybind import x, delta, polylog_qli


Linear = linear.Linear

X = x.X
Inf = x.Inf

Delta = delta.Delta
LazyDeltaExpr = delta.LazyDeltaExpr

class DeltaExpr(Linear):
    def obj_to_str(self, obj):
        return format.otimes.join([str(o) for o in obj])

QLi = polylog_qli.QLi

def eval_expr(lazy_expr):
    if isinstance(lazy_expr, LazyDeltaExpr):
        return DeltaExpr.from_pairs(delta.eval_lazy_delta(lazy_expr).data)
    raise TypeError("`eval_expr` expected a lazy expression type, got {}".format(
        type(lazy_expr).__name__
    ))
