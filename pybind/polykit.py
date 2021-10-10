from python.polypy.lib.util import args_to_iterable
from pybind import py_bindings as pb


Encoder = pb.Encoder
RichTextFormat = pb.RichTextFormat
AnnotationSorting = pb.AnnotationSorting
NoLineLimit = pb.NoLineLimit
set_formatting = pb.set_formatting
reset_formatting = pb.reset_formatting

to_lyndon_basis = pb.to_lyndon_basis

X = pb.X
Inf = pb.Inf
Zero = pb.Zero
x1 = pb.x1
x2 = pb.x2
x3 = pb.x3
x4 = pb.x4
x5 = pb.x5
x6 = pb.x6
x7 = pb.x7
x8 = pb.x8
x1s = pb.x1s
x2s = pb.x2s
x3s = pb.x3s
x4s = pb.x4s
x5s = pb.x5s
x6s = pb.x6s
x7s = pb.x7s
x8s = pb.x8s

Delta = pb.Delta
DeltaExpr = pb.DeltaExpr
DeltaICoExpr = pb.DeltaICoExpr
ProjectionExpr = pb.ProjectionExpr

substitute_variables = pb.substitute_variables
icomultiply = pb.icomultiply
ncomultiply = pb.ncomultiply
involute = pb.involute
project_on = pb.project_on
terms_with_num_distinct_variables = pb.terms_with_num_distinct_variables
terms_with_min_distinct_variables = pb.terms_with_min_distinct_variables
terms_containing_only_variables = pb.terms_containing_only_variables
terms_without_variables = pb.terms_without_variables
sorted_by_num_distinct_variables = pb.sorted_by_num_distinct_variables

CrossRatio = pb.CrossRatio
CompoundRatio = pb.CompoundRatio

QLi = pb.QLi
QLiNeg = pb.QLiNeg
QLiSymm = pb.QLiSymm
# TODO: Make 'projector' optional argument instead, here and in QLiN
QLiPr = pb.QLiPr
QLiNegPr = pb.QLiNegPr
QLiSymmPr = pb.QLiSymmPr

Lira = pb.Lira

loops_matrix = pb.loops_matrix

def CR(*points): return CompoundRatio(CrossRatio(args_to_iterable(points)))

def tensor_product(*parts): return pb.tensor_product_vec(args_to_iterable(parts))
def icoproduct(*parts): return pb.icoproduct_vec(args_to_iterable(parts))
def ncoproduct(*parts): return pb.ncoproduct_vec(args_to_iterable(parts))

def I(*points): return pb.IVec(args_to_iterable(points))
def Corr(*points): return pb.CorrVec(args_to_iterable(points))

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

def Log(*points): return pb.Log(args_to_iterable(points))

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

def A2(*points): return pb.A2(args_to_iterable(points))

def Lira0(*weights): return lambda *ratios: Lira(0, args_to_iterable(weights), args_to_iterable(ratios))
def Lira1(*weights): return lambda *ratios: Lira(1, args_to_iterable(weights), args_to_iterable(ratios))
def Lira2(*weights): return lambda *ratios: Lira(2, args_to_iterable(weights), args_to_iterable(ratios))
def Lira3(*weights): return lambda *ratios: Lira(3, args_to_iterable(weights), args_to_iterable(ratios))
def Lira4(*weights): return lambda *ratios: Lira(4, args_to_iterable(weights), args_to_iterable(ratios))
def Lira5(*weights): return lambda *ratios: Lira(5, args_to_iterable(weights), args_to_iterable(ratios))
def Lira6(*weights): return lambda *ratios: Lira(6, args_to_iterable(weights), args_to_iterable(ratios))
def Lira7(*weights): return lambda *ratios: Lira(7, args_to_iterable(weights), args_to_iterable(ratios))
def Lira8(*weights): return lambda *ratios: Lira(8, args_to_iterable(weights), args_to_iterable(ratios))
