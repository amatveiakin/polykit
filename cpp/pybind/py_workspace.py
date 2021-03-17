import polykit as pk


# TODO: Benchmark full computations in python VS python arithmetic VS everything in C++

expr = pk.QLi(1, [1,2,3,4]) + pk.QLi(1, [1,3,2,4])
print(expr.description())
print(pk.eval_expr(expr))

print("===")

expr2 = pk.eval_expr(pk.QLi(1, [1,2,3,4])) + pk.eval_expr(pk.QLi(1, [1,3,2,4]))
print(expr2)
