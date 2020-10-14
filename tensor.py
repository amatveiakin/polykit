import math

import format

from alphabet_mapping import AlphabetMapping
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


class Summand:
    def __init__(
            self,
            multipliers,  # iterable[D]
            coeff = 1,    # integer  (rational ?)
        ):
        self.multipliers = tuple(multipliers)
        self.coeff = coeff
        [coeff := coeff * d.normalize() for d in self.multipliers]

    def __neg__(self):
        return Summand(self.multipliers, -self.coeff)

    def __str__(self):
        return format.coeff(self.coeff) + format.otimes.join(str(s) for s in self.multipliers)

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
            summands  # List[Summand]
        ):
        self.summands = summands
        self.weight = len(summands[0].multipliers) if len(summands) > 0 else -1
        for s in self.summands:
            assert len(s.multipliers) == self.weight
        self.dimension = max([max([max(d.a, d.b) for d in s.multipliers]) for s in summands])
        self.convert_to_lyndon_basis()
        self.check_criterion()

    def convert_to_lyndon_basis(self):
        print("convert_to_lyndon_basis - before:\n" + str(self))
        alphabet_mapping = AlphabetMapping(self.dimension)
        summand_words = {
            _to_word(alphabet_mapping, s.multipliers): s.coeff
            for s in self.summands
        }
        self.summands = [
            Summand(_from_word(alphabet_mapping, word), coeff=count)
            for word, count
            in to_lyndon_basis(summand_words).items()
        ]
        print("\nconvert_to_lyndon_basis - after:\n" + str(self))
    
    def __check_criterion_condition(
            self,
            summands_dict,  # multipliers -> coeff
            summand,        # Summand
            substitutions,  # as in _change_multipliers
        ):
        new_summand = Summand(_change_multipliers(summand.multipliers, substitutions))
        new_summand.coeff *= (summands_dict.get(new_summand.multipliers) or 0)
        assert summand.coeff == new_summand.coeff, f"Criterion failed:\n  {summand}\nvs\n  {new_summand}"
        # assert abs(summand.coeff) == abs(new_summand.coeff), f"Criterion failed:\n  {summand}\nvs\n  {new_summand}\n(substitutions: {({x: str(y) for x, y in substitutions.items()})})"

    def check_criterion(self):
        summands_dict = {s.multipliers : s.coeff for s in self.summands}
        for k1 in range(0, self.weight - 1):
            k2 = k1 + 1
            for s in self.summands:
                d1 = s.multipliers[k1]
                d2 = s.multipliers[k2]
                common = _common_indices(d1, d2)
                num_common = len(common)
                if num_common == 0:
                    self.__check_criterion_condition(summands_dict, s, {
                        k1: s.multipliers[k2],
                        k2: s.multipliers[k1],
                    })
                if num_common == 1:
                    b = get_one_item(common)
                    a = _other_index(d1, b)
                    c = _other_index(d2, b)
                    self.__check_criterion_condition(summands_dict, s, {
                        k1: D(a, b),
                        k2: D(b, c),
                    })
                    self.__check_criterion_condition(summands_dict, s, {
                        k1: D(b, c),
                        k2: D(c, a),
                    })
                    self.__check_criterion_condition(summands_dict, s, {
                        k1: D(c, a),
                        k2: D(a, b),
                    })
                elif num_common == 2:
                    pass
                else:
                    assert False, f"Number of common indices == {num_common}"

    def __str__(self):
        return "\n".join(str(s) for s in self.summands)
