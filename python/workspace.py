from polypy import *


profiler = Profiler()
expr = Li6(1,2,3,4,5,6)
profiler.finish("expr")
print(f"Num terms before Lyndon = {len(expr.without_annotations())}")
lyndon = d_expr_to_lyndon_basis(expr)
profiler.finish("lyndon")
print(f"Num terms after Lyndon = {len(lyndon.without_annotations())}")
