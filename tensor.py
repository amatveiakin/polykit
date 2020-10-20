import math

from dataclasses import dataclass

import format

from alphabet_mapping import AlphabetMapping
from linear import Linear
from lyndon import to_lyndon_basis
from util import get_one_item


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


def d_monom_to_str(
        multipliers  # Tuple[D]
    ):
    return format.otimes.join(str(d) for d in multipliers)


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

def d_expr_dimension(
        expr,  # Linear[Tuple[D]]
    ):
    return max([
        max([
            max(d.a, d.b)
            for d in multipliers
        ])
        for multipliers, _ in expr.items()
    ])


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
        alphabet_mapping,  # AlphabetMapping
        pairs,             # Tuple[D]
    ):
    return tuple([alphabet_mapping.to_alphabet(d.as_tuple()) for d in pairs])

def _from_word(
        alphabet_mapping,  # AlphabetMapping
        word,              # Tuple[int]
    ):
    return tuple([D.from_tuple(alphabet_mapping.from_alphabet(c)) for c in word])


class Tensor:
    def __init__(
            self,
            summands,  # Linear[Tuple[D]]
        ):
        assert isinstance(summands, Linear)
        self.summands = summands
        self.weight = None
        for multipliers, _ in summands.items():
            for d in multipliers:
                assert not d.is_nil()
            if self.weight is None:
                self.weight = len(multipliers)
            else:
                assert len(multipliers) == self.weight
        self.dimension = d_expr_dimension(summands) if len(summands) > 0 else None
        # self.convert_to_lyndon_basis()
        # self.check_criterion()

    def convert_to_lyndon_basis(self):
        # print("convert_to_lyndon_basis - before:\n" + self.to_str_with_alphabet_mapping() + "\n")
        alphabet_mapping = AlphabetMapping(self.dimension)
        summand_words = Linear({
            _to_word(alphabet_mapping, multipliers): coeff
            for multipliers, coeff
            in self.summands.items()
        })
        self.summands = Linear({
            _from_word(alphabet_mapping, word): coeff
            for word, coeff
            in to_lyndon_basis(summand_words).items()
        })
        # print("convert_to_lyndon_basis - after:\n" + self.to_str_with_alphabet_mapping() + "\n")
    
    def __check_criterion_condition_no_swap(
            self,
            multipliers,    # Tuple[D]
            coeff,          # expected coeff (integer)
            substitutions,  # as in _change_multipliers
        ):
        new_multipliers = _change_multipliers(multipliers, substitutions)
        new_coeff = self.summands[new_multipliers]
        assert coeff == new_coeff, (
            f"Criterion failed:\n  {Linear({multipliers: coeff})}"
            f"\nvs\n  {Linear({new_multipliers: new_coeff})}"
        )

    def __check_criterion_condition_allow_swap(
            self,
            multipliers,    # Tuple[D]
            coeff,          # expected coeff (integer)
            substitutions,  # as in _change_multipliers
        ):
        assert(len(substitutions) == 2)
        subs = list(substitutions.items())
        substitutions_2 = {
            subs[0][0]: subs[1][1],
            subs[1][0]: subs[0][1],
        }
        new_multipliers_1 = _change_multipliers(multipliers, substitutions)
        new_coeff_1 = self.summands[new_multipliers_1]
        new_multipliers_2 = _change_multipliers(multipliers, substitutions_2)
        new_coeff_2 = -self.summands[new_multipliers_2]
        assert (
            (coeff == new_coeff_1 and new_coeff_2 == 0) or
            (coeff == new_coeff_2 and new_coeff_1 == 0)
        ), (
            f"Criterion failed:\n  {Linear({multipliers: coeff})}"
            f"\nvs\n  {Linear({new_multipliers_1: new_coeff_1})}"
            f"\nvs\n  {Linear({new_multipliers_2: new_coeff_2})}"
        )

    def check_criterion(self):
        for k1 in range(0, self.weight - 1):
            k2 = k1 + 1
            for multipliers, coeff in self.summands.items():
                d1 = multipliers[k1]
                d2 = multipliers[k2]
                common = _common_indices(d1, d2)
                num_common = len(common)
                if num_common == 0:
                    self.__check_criterion_condition_no_swap(multipliers, coeff, {
                        k1: d2,
                        k2: d1,
                    })
                elif num_common == 1:
                    swapped_multipliers = _change_multipliers(multipliers, {
                        k1: d2,
                        k2: d1,
                    })
                    swapped_coeff = self.summands[swapped_multipliers]
                    if coeff != swapped_coeff:
                        b = get_one_item(common)
                        a = _other_index(d1, b)
                        c = _other_index(d2, b)
                        self.__check_criterion_condition_allow_swap(multipliers, coeff, {
                            k1: D(a, b),
                            k2: D(b, c),
                        })
                        self.__check_criterion_condition_allow_swap(multipliers, coeff, {
                            k1: D(b, c),
                            k2: D(c, a),
                        })
                        self.__check_criterion_condition_allow_swap(multipliers, coeff, {
                            k1: D(c, a),
                            k2: D(a, b),
                        })
                elif num_common == 2:
                    pass
                else:
                    assert False, f"Number of common indices == {num_common}"

    def __str__(self):
        return self.summands.to_str(d_monom_to_str)

    def to_str_with_alphabet_mapping(self):
        alphabet_mapping = AlphabetMapping(self.dimension)
        return self.summands.to_str(lambda multipliers:
            d_monom_to_str(multipliers) +
            "  <=>  " +
            str(_to_word(alphabet_mapping, multipliers))
        )
