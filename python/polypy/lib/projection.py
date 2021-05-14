from .delta import d_expr_dimension
from .linear import Linear
from .lyndon import to_lyndon_basis


# Projects a linear combination of tensor products of (x_i - x_j) on x_index.
# Keeps only the products that contain x_index in every bracket. Turns each
# such product into a word containing the other indices.
#
# Example:
#     + (x1 - x2)*(x1 - x3)
#     + (x1 - x2)*(x2 - x3)
#     - (x1 - x2)*(x1 - x4)
#   projected on index == 1 gives:
#     (2, 3) - (2, 4)
#
def project_on_xi(
        expr,   # Linear[D]
        index,  # int
    ):
    words = Linear()
    for multipliers, coeff in expr.items():
        w = []
        for d in multipliers:
            if d.a == index:
                w.append(d.b)
            elif d.b == index:
                w.append(d.a)
            else:
                break
        if len(w) == len(multipliers):
            words += Linear({tuple(w): coeff})
    return words

def project_on_x1(expr): return project_on_xi(expr, 1)
def project_on_x2(expr): return project_on_xi(expr, 2)
def project_on_x3(expr): return project_on_xi(expr, 3)
def project_on_x4(expr): return project_on_xi(expr, 4)
def project_on_x5(expr): return project_on_xi(expr, 5)
def project_on_x6(expr): return project_on_xi(expr, 6)
def project_on_x7(expr): return project_on_xi(expr, 7)
def project_on_x8(expr): return project_on_xi(expr, 8)


# Equivalent to "lyndon_basis(expr) == Linear()", but faster
def d_expr_is_zero_in_lyndon(expr):
    if expr == Linear():
        return True
    num_points = d_expr_dimension(expr)
    for i in range(1, num_points - 2):
        if to_lyndon_basis(project_on_xi(expr, i)) != Linear():
            return False
    return True
