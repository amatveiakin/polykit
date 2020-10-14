import math

import format

from alphabet_mapping import AlphabetMapping
from linear import Linear
from lyndon import to_lyndon_basis
from util import get_one_item


# Represents a single difference (x_i - x_j)
class D:
    def __init__(self, a, b):  # a, b are integers
        assert a != b
        self.a = a
        self.b = b

    @staticmethod
    def from_tuple(tpl):
        assert len(tpl) == 2
        return D(tpl[0], tpl[1])

    def as_tuple(self):
        return (self.a, self.b)

    def normalize(self):
        if self.a < self.b:
            return 1
        else:
            self.a, self.b = self.b, self.a
            return -1

    def __eq__(self, other):
        return isinstance(other, D) and self.as_tuple() == other.as_tuple()

    def __hash__(self):
        return hash(self.as_tuple())

    def __str__(self):
        return f"(x{format.substript(self.a)} {format.minus} x{format.substript(self.b)})"


def _common_indices(d1, d2):
    return set(d1.as_tuple()) & set(d2.as_tuple())

def _other_index(d, idx):
    assert d.a == idx or d.b == idx
    return d.a if d.b == idx else d.b


# Product of multiple `D`s. Temporary class for convenient input.
class Product:
    def __init__(
            self,
            multipliers,  # iterable[D]
            coeff = 1,    # integer  (rational ?)
        ):
        self.multipliers = tuple(multipliers)
        self.coeff = coeff
        [coeff := coeff * d.normalize() for d in self.multipliers]

    def __neg__(self):
        return Product(self.multipliers, -self.coeff)

    def __str__(self):
        return format.coeff(self.coeff) + _multipliers_to_str(self.multipliers)

def _multipliers_to_str(multipliers):
    return format.otimes.join(str(d) for d in multipliers)

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
            summands  # List[Product]
        ):
        self.summands = Linear({s.multipliers: s.coeff for s in summands})
        self.weight = len(summands[0].multipliers) if len(summands) > 0 else -1
        for s in summands:
            assert len(s.multipliers) == self.weight
        self.dimension = max([max([max(d.a, d.b) for d in s.multipliers]) for s in summands])
        self.convert_to_lyndon_basis()
        self.check_criterion()

    def convert_to_lyndon_basis(self):
        print("convert_to_lyndon_basis - before:\n" + str(self))
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
        print("\nconvert_to_lyndon_basis - after:\n" + str(self))
    
    def __check_criterion_condition(
            self,
            multipliers,    # Tuple[D]
            coeff,          # expected coeff (integer)
            substitutions,  # as in _change_multipliers
        ):
        product = Product(multipliers, coeff)
        assert product.coeff == coeff
        new_product = Product(_change_multipliers(multipliers, substitutions))
        new_product.coeff *= self.summands[new_product.multipliers]
        assert product.coeff == new_product.coeff, f"Criterion failed:\n  {product}\nvs\n  {new_product}"

    def check_criterion(self):
        for k1 in range(0, self.weight - 1):
            k2 = k1 + 1
            for multipliers, coeff in self.summands.items():
                d1 = multipliers[k1]
                d2 = multipliers[k2]
                common = _common_indices(d1, d2)
                num_common = len(common)
                if num_common == 0:
                    self.__check_criterion_condition(multipliers, coeff, {
                        k1: multipliers[k2],
                        k2: multipliers[k1],
                    })
                if num_common == 1:
                    b = get_one_item(common)
                    a = _other_index(d1, b)
                    c = _other_index(d2, b)
                    self.__check_criterion_condition(multipliers, coeff, {
                        k1: D(a, b),
                        k2: D(b, c),
                    })
                    self.__check_criterion_condition(multipliers, coeff, {
                        k1: D(b, c),
                        k2: D(c, a),
                    })
                    self.__check_criterion_condition(multipliers, coeff, {
                        k1: D(c, a),
                        k2: D(a, b),
                    })
                elif num_common == 2:
                    pass
                else:
                    assert False, f"Number of common indices == {num_common}"

    def __str__(self):
        return self.summands.to_str(_multipliers_to_str)
