import math

from dataclasses import dataclass

import format

from alphabet_mapping import d_to_alphabet, d_from_alphabet
from linear import Linear
from lyndon import to_lyndon_basis
from util import flatten, get_one_item


class _InfinityType:
    def __str__(self):
        return "Inf"
Inf = _InfinityType()

# Represents a single difference (x_i - x_j)
# Optimization potential: Convert to alphabet mapping immediately
#   (make it start from a 1000 to reduce the chances of messing up)
@dataclass(init=False, eq=True, order=True, frozen=True)
class D:
    a: int
    b: int

    def __init__(self, a, b):
        assert isinstance(a, (int, _InfinityType)), a
        assert isinstance(b, (int, _InfinityType)), b
        if a == Inf or b == Inf:
            (a, b) = (0, 0)
        else:
            (a, b) = (a, b) if a < b else (b, a)
        object.__setattr__(self, "a", a)
        object.__setattr__(self, "b", b)

    @staticmethod
    def from_tuple(tpl):
        assert len(tpl) == 2
        return D(tpl[0], tpl[1])

    def is_nil(self):
        return self.a == self.b

    def as_tuple(self):
        return (self.a, self.b)

    def __str__(self):
        return f"(x{format.substript(self.a)} {format.minus} x{format.substript(self.b)})"


# For Linear.to_str
def d_monom_to_str(
        multipliers  # Tuple[D]
    ):
    return format.otimes.join(str(d) for d in multipliers)

# For Linear.to_str
def d_monom_to_str_with_alphabet_mapping(
        multipliers  # Tuple[D]
    ):
    return (
        d_monom_to_str(multipliers) +
        "  <=>  " +
        str(_to_word(multipliers))
    )


# Replaces each index c with index_map[c]
def d_monom_substitute(
        multipliers,  # Tuple[D]
        index_map,    # int -> int
    ):
    return tuple([D(index_map[d.a], index_map[d.b]) for d in multipliers])

# For each monom, replaces each index c with index_map[c]
def d_expr_substitute(
        expr,       # Linear[Tuple[D]]
        index_map,  # int -> int
    ):
    ret = Linear()
    for multipliers, coeff in expr.items():
        multipliers_new = d_monom_substitute(multipliers, index_map)
        if not any([d.is_nil() for d in multipliers_new]):
            ret += Linear({multipliers_new: coeff})
    return ret


def d_monoms_distinct_chars(
        multipliers,  # Tuple[D]
    ):
    return len(set(flatten([[d.a, d.b] for d in multipliers])))

def d_monoms_with_n_distinct_chars(
        expr,          # Linear[Tuple[D]]
        min_distinct,  # int
    ):
    return expr.without_annotations().filtered_obj(lambda monom: d_monoms_distinct_chars(monom) >= min_distinct)


def d_expr_weight(
        expr: Linear,  # Linear[Tuple[D]]
    ):
    return len(get_one_item(expr.items())[0])

def d_expr_dimension(
        expr: Linear,  # Linear[Tuple[D]]
    ):
    return max([
        max([
            max(d.a, d.b)
            for d in multipliers
        ])
        for multipliers, _ in expr.items()
    ])


# Equivalent to `to_lyndon_basis(expr)`, but hopefully faster
def d_expr_to_lyndon_basis(
        expr: Linear,  # Linear[Tuple[D]]
    ):
    return to_lyndon_basis(
        expr.without_annotations().mapped_obj(_to_word)
    ).mapped_obj(_from_word)


# Checks that:
#   - There are no zero terms (they should be discarded immediately);
#   - All terms have the same weight.
def d_expr_check_simple_invariants(
        expr: Linear,  # Linear[Tuple[D]]
    ):
    expr = expr.without_annotations()
    if expr == Linear():
        return
    weight = d_expr_weight(expr)
    for multipliers, _ in expr.items():
        for d in multipliers:
            assert not d.is_nil()
        assert len(multipliers) == weight
    return weight

def d_expr_check_integratability(
        expr: Linear,  # Linear[Tuple[D]]
    ):
    expr = expr.without_annotations()
    weight = d_expr_weight(expr)
    for k1 in range(0, weight - 1):
        k2 = k1 + 1
        for multipliers, coeff in expr.items():
            d1 = multipliers[k1]
            d2 = multipliers[k2]
            common = _common_indices(d1, d2)
            num_common = len(common)
            if num_common == 0:
                _check_integratability_condition_no_swap(expr, multipliers, coeff, {
                    k1: d2,
                    k2: d1,
                })
            elif num_common == 1:
                swapped_multipliers = _change_multipliers(multipliers, {
                    k1: d2,
                    k2: d1,
                })
                swapped_coeff = expr[swapped_multipliers]
                if coeff != swapped_coeff:
                    b = get_one_item(common)
                    a = _other_index(d1, b)
                    c = _other_index(d2, b)
                    _check_integratability_condition_allow_swap(expr, multipliers, coeff, {
                        k1: D(a, b),
                        k2: D(b, c),
                    })
                    _check_integratability_condition_allow_swap(expr, multipliers, coeff, {
                        k1: D(b, c),
                        k2: D(c, a),
                    })
                    _check_integratability_condition_allow_swap(expr, multipliers, coeff, {
                        k1: D(c, a),
                        k2: D(a, b),
                    })
            elif num_common == 2:
                pass
            else:
                assert False, f"Number of common indices == {num_common}"


def _common_indices(d1, d2):
    return set(d1.as_tuple()) & set(d2.as_tuple())

def _other_index(d, idx):
    assert d.a == idx or d.b == idx
    return d.a if d.b == idx else d.b


def _change_multipliers(
        multipliers,    # Tuple[D]
        substitutions,  # index -> D
    ):
    result = list(multipliers)
    for key, value in substitutions.items():
        result[key] = value
    return tuple(result)

def _to_word(
        pairs,  # Tuple[D]
    ):
    return tuple([d_to_alphabet(d.as_tuple()) for d in pairs])

def _from_word(
        word,  # Tuple[int]
    ):
    return tuple([D.from_tuple(d_from_alphabet(c)) for c in word])

def _check_integratability_condition_no_swap(
        expr: Linear,   # Linear[Tuple[D]]
        multipliers,    # Tuple[D]
        coeff,          # expected coeff (integer)
        substitutions,  # as in _change_multipliers
    ):
    new_multipliers = _change_multipliers(multipliers, substitutions)
    new_coeff = expr[new_multipliers]
    assert coeff == new_coeff, (
        f"Criterion failed:\n  {Linear({multipliers: coeff})}"
        f"\nvs\n  {Linear({new_multipliers: new_coeff})}"
    )

def _check_integratability_condition_allow_swap(
        expr: Linear,   # Linear[Tuple[D]]
        multipliers,    # Tuple[D]
        coeff,          # expected coeff (integer)
        substitutions,  # as in _change_multipliers
    ):
    assert len(substitutions) == 2
    subs = list(substitutions.items())
    substitutions_2 = {
        subs[0][0]: subs[1][1],
        subs[1][0]: subs[0][1],
    }
    new_multipliers_1 = _change_multipliers(multipliers, substitutions)
    new_coeff_1 = expr[new_multipliers_1]
    new_multipliers_2 = _change_multipliers(multipliers, substitutions_2)
    new_coeff_2 = -expr[new_multipliers_2]
    assert (
        (coeff == new_coeff_1 and new_coeff_2 == 0) or
        (coeff == new_coeff_2 and new_coeff_1 == 0)
    ), (
        f"Criterion failed:\n  {Linear({multipliers: coeff})}"
        f"\nvs\n  {Linear({new_multipliers_1: new_coeff_1})}"
        f"\nvs\n  {Linear({new_multipliers_2: new_coeff_2})}"
    )
