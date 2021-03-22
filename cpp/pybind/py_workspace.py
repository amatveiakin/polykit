from polykit import X, Inf, Delta, eval_expr, to_lyndon_basis
from polykit import QLi, QLi1, QLi2, QLi3, QLi4, QLi5, QLi6, QLi7, QLi8
from polykit import QLiNeg, QLiNeg1, QLiNeg2, QLiNeg3, QLiNeg4, QLiNeg5, QLiNeg6, QLiNeg7, QLiNeg8
from polykit import QLiSymm, QLiSymm1, QLiSymm2, QLiSymm3, QLiSymm4, QLiSymm5, QLiSymm6, QLiSymm7, QLiSymm8


# TODO: Benchmark full computations in python VS python arithmetic VS everything in C++

expr = QLi1(1,2,3,4) + QLi1(1,3,2,4)
print(expr.description())
print(eval_expr(expr))

print("===")

expr2 = eval_expr(QLi1(1,2,3,4)) + eval_expr(QLi1(1,3,2,4))
print(expr2)
