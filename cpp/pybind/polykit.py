from pybind.util import args_to_iterable
from pybind import lyndon, x, delta, projection, ratio, iterated_integral, polylog_qli, polylog_lira


to_lyndon_basis = lyndon.to_lyndon_basis

X = x.X
Inf = x.Inf

Delta = delta.Delta
DeltaExpr = delta.DeltaExpr
DeltaCoExpr = delta.DeltaCoExpr
substitute_variables = delta.substitute_variables
involute = delta.involute
terms_containing_only_variables = delta.terms_containing_only_variables
terms_without_variables = delta.terms_without_variables
coproduct = delta.coproduct
comultiply = delta.comultiply

ProjectionExpr = projection.ProjectionExpr
project_on = projection.project_on
involute_projected = projection.involute_projected

CrossRatio = ratio.CrossRatio
CompoundRatio = ratio.CompoundRatio

QLi = polylog_qli.QLi
QLiNeg = polylog_qli.QLiNeg
QLiSymm = polylog_qli.QLiSymm

Lira = polylog_lira.Lira

def CR(*points): return CompoundRatio(CrossRatio(args_to_iterable(points)))

def I(*points):
    return iterated_integral.IVec(args_to_iterable(points))
def Corr(*points):
    return iterated_integral.CorrVec(args_to_iterable(points))

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

def Lira1(*weights):
    return lambda *ratios: Lira(1, args_to_iterable(weights), args_to_iterable(ratios))
def Lira2(*weights):
    return lambda *ratios: Lira(2, args_to_iterable(weights), args_to_iterable(ratios))
def Lira3(*weights):
    return lambda *ratios: Lira(3, args_to_iterable(weights), args_to_iterable(ratios))
def Lira4(*weights):
    return lambda *ratios: Lira(4, args_to_iterable(weights), args_to_iterable(ratios))
def Lira5(*weights):
    return lambda *ratios: Lira(5, args_to_iterable(weights), args_to_iterable(ratios))
def Lira6(*weights):
    return lambda *ratios: Lira(6, args_to_iterable(weights), args_to_iterable(ratios))
def Lira7(*weights):
    return lambda *ratios: Lira(7, args_to_iterable(weights), args_to_iterable(ratios))
def Lira8(*weights):
    return lambda *ratios: Lira(8, args_to_iterable(weights), args_to_iterable(ratios))
