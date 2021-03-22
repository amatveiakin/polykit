from polykit import X, Inf, Delta, QLi, eval_expr


# TODO: Benchmark full computations in python VS python arithmetic VS everything in C++

expr = QLi(1, [1,2,3,4]) + QLi(1, [1,3,2,4])
print(expr.description())
print(eval_expr(expr))

print("===")

expr2 = eval_expr(QLi(1, [1,2,3,4])) + eval_expr(QLi(1, [1,3,2,4]))
print(expr2)
