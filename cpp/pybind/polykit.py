import linear
from pybind import delta, polylog_qli


Linear = linear.Linear

Delta = delta.Delta
LazyDeltaExpr = delta.LazyDeltaExpr

QLi = polylog_qli.QLi

def eval_expr(lazy_expr):
    if isinstance(lazy_expr, LazyDeltaExpr):
        return Linear.from_pairs(delta.eval_lazy_delta(lazy_expr).data)
    raise TypeError("`eval_expr` expected a lazy expression type, got {}".format(
        type(lazy_expr).__name__
    ))
