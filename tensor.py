import math

import format

from alphabet_mapping import AlphabetMapping
from lyndon import lyndon_factorize, shuffle_product_many
from util import count_items, append_counting_dict


# Represents a single difference (x_i - x_j)
class D:
    def __init__(self, a, b):  # a, b are integers
        assert a != b
        (self.a, self.b) = (a, b) if a < b else (b, a)

    @staticmethod
    def from_tuple(tpl):
        assert len(tpl) == 2
        return D(tpl[0], tpl[1])

    def as_tuple(self):
        return (self.a, self.b)

    def __eq__(self, other):
        return isinstance(other, D) and self.as_tuple() == other.as_tuple()

    def __hash__(self):
        return hash(self.as_tuple())

    def __str__(self):
        return f"(x{format.substript(self.a)} {format.minus} x{format.substript(self.b)})"


def _common_indices(d1, d2):
    return set(d1.as_tuple()) & set(d2.as_tuple())

def _all_indices(d1, d2):
    return set(d1.as_tuple()) | set(d2.as_tuple())


class Summand:
    def __init__(
            self,
            multipliers,  # iterable[D]
            coeff = 1,    # integer  (rational ?)
        ):
        self.multipliers = tuple(sorted(multipliers, key=D.as_tuple))
        self.coeff = coeff

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

# Returns a / b; asserts that the result is an integer.
def _div_int(x, y):
    result, reminder = divmod(x, y)
    assert reminder == 0
    return result


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
        # print("convert_to_lyndon_basis - before:\n" + str(self))
        alphabet_mapping = AlphabetMapping(self.dimension)
        word_summands = {_to_word(alphabet_mapping, s.multipliers): s.coeff for s in self.summands}
        finished = False
        # Optimization potential: Cache word_orig => words_expanded.
        while not finished:
            word_summands_new = {}
            finished = True
            for word_orig in word_summands:
                lyndon_words = lyndon_factorize(word_orig)
                lyndon_word_counts = count_items(lyndon_words)
                # TODO: What about len(lyndon_words) > 1 and len(lyndon_word_counts) == 1 ?
                # Is this ok that Lyndon_word^N is also in basis?
                if len(lyndon_word_counts) == 1:
                    append_counting_dict(word_summands_new, {word_orig: 1})
                    continue
                finished = False
                # denominator = 1
                # numerator = []
                # for word, count in lyndon_word_counts.items():
                #     denominator *= math.factorial(count)
                #     numerator.append(word * count)
                # expanded_word_counts = {
                #     word: _div_int(count, denominator)
                #     for (word, count)
                #     in count_items(shuffle_product_many(numerator)).items()
                # }
                denominator = 1
                for count in lyndon_word_counts.values():
                    denominator *= math.factorial(count)
                # Optimization potential: Don't generate all N! results for each word^N.
                expanded_word_counts = {
                    word: _div_int(count, denominator)
                    for (word, count)
                    in count_items(shuffle_product_many(lyndon_words)).items()
                }
                assert expanded_word_counts.get(word_orig) == 1, str(word_orig) + " not in " + str(expanded_word_counts)
                # print("Lyndon transform: " + str(word_orig) + " => " + str(expanded_word_counts))
                del expanded_word_counts[word_orig]
                append_counting_dict(word_summands_new, expanded_word_counts)
            word_summands = word_summands_new
        self.summands = [
            Summand(_from_word(alphabet_mapping, word), count)
            for word, count
            in word_summands.items()
        ]
        # print("convert_to_lyndon_basis - after:\n" + str(self))
    
    def __check_criterion_condition(
            self,
            summands_dict,  # multipliers -> coeff
            summand,        # Summand
            substitutions,  # as in _change_multipliers
        ):
        new_multipliers = _change_multipliers(summand.multipliers, substitutions)
        new_coeff = summands_dict.get(new_multipliers) or 0
        new_summand = Summand(new_multipliers, coeff=new_coeff)
        assert summand.coeff == new_summand.coeff, f"Criterion failed:\n  {summand}\nvs\n  {new_summand}"

    def check_criterion(self):
        summands_dict = {s.multipliers : s.coeff for s in self.summands}
        for k in range(0, self.weight - 1):
            l = k+1
            for s in self.summands:
                d1 = s.multipliers[k]
                d2 = s.multipliers[l]
                common = _common_indices(d1, d2)
                num_common = len(common)
                if num_common == 0:
                    self.__check_criterion_condition(summands_dict, s, {
                        k: s.multipliers[l],
                        l: s.multipliers[k],
                    })
                if num_common == 1:
                    indices = _all_indices(d1, d2)
                    assert len(indices) == 3
                    a, b, c = tuple(indices)
                    self.__check_criterion_condition(summands_dict, s, {
                        k: D(a, b),
                        l: D(a, c),
                    })
                    self.__check_criterion_condition(summands_dict, s, {
                        k: D(b, a),
                        l: D(b, c),
                    })
                    self.__check_criterion_condition(summands_dict, s, {
                        k: D(c, a),
                        l: D(c, b),
                    })
                elif num_common == 2:
                    pass
                else:
                    assert False, f"Number of common indices == {num_common}"

    def __str__(self):
        return "\n".join(str(s) for s in self.summands)
