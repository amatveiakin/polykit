from polypy import *


profiler = Profiler()
expr = QLi5(1,2,3,4,5,6)
profiler.finish("expr")
print(f"Num terms before Lyndon = {expr.num_terms()}")
lyndon = d_expr_to_lyndon_basis(expr)
profiler.finish("lyndon")
print(f"Num terms after Lyndon = {lyndon.num_terms()}")
