import format

from tensor import d_monom_to_str
from linear import Linear, tensor_product
from lyndon import to_lyndon_basis
from word_algebra import W, word_expr_weight


def word_coproduct(*multipliers):
    return tensor_product(*[to_lyndon_basis(m) for m in multipliers], product=lambda a, b: (a, b))


# For Linear.to_str
def d_coproduct_element_to_str(element):
    return f" {format.cotimes} ".join([d_monom_to_str(d_monom) for d_monom in element])


def normalize_square_coproduct(expr: Linear):
    ret = Linear()
    for pair, coeff in expr.items():
        a, b = pair
        assert len(a) == len(b)
        if a < b:
            ret += Linear({(a, b): coeff})
        else:
            ret += Linear({(b, a): -coeff})
    return ret


def word_comultiply(
        expr: Linear,  # Linear[Tuple]
        form: tuple,   # (a, b)
    ):
    expr = expr.without_annotations()
    if expr == Linear():
        return Linear()
    weight = word_expr_weight(expr)
    assert len(form) == 2
    assert sum(form) == weight, f"Expected: sum({form}) == weight"
    # form = tuple(sorted(form))

    ret = Linear()
    for word, coeff in expr.items():
        assert len(word) == weight
        split = form[0]
        ret += coeff * word_coproduct(
            to_lyndon_basis(Linear({word[:split]: 1})),
            to_lyndon_basis(Linear({word[split:]: 1})),
        )
        if form[0] != form[1]:
            split = form[1]
            ret -= coeff * word_coproduct(
                to_lyndon_basis(Linear({word[split:]: 1})),
                to_lyndon_basis(Linear({word[:split]: 1})),
            )
    if form[0] == form[1]:
        ret = normalize_square_coproduct(ret)
    return ret
