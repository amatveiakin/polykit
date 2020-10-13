import format


class D:
    def __init__(self, a, b):  # a, b are integers
        assert a != b
        (self.a, self.b) = (a, b) if a < b else (b, a)

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
            multipliers,  # Tuple[D]
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


class Tensor:
    def __init__(
            self,
            summands  # List[Summand]
        ):
        # TODO: Convert to Lyndon basis
        self.summands = summands
        self.summands_dict = {s.multipliers : s.coeff for s in summands}
        self.weight = len(summands[0].multipliers) if len(summands) > 0 else -1
        for s in self.summands:
            assert len(s.multipliers) == self.weight
        self.check_criterion()
    
    def __check_criterion_condition(
            self,
            summand,        # Summand
            substitutions,  # as in _change_multipliers
        ):
        new_multipliers = _change_multipliers(summand.multipliers, substitutions)
        new_coeff = self.summands_dict.get(new_multipliers) or 0
        new_summand = Summand(new_multipliers, coeff=new_coeff)
        assert summand.coeff == new_summand.coeff, f"Criterion failed:\n  {summand}\nvs\n  {new_summand}"

    def check_criterion(self):
        for k in range(0, self.weight - 1):
            l = k+1
            for s in self.summands:
                d1 = s.multipliers[k]
                d2 = s.multipliers[l]
                common = _common_indices(d1, d2)
                num_common = len(common)
                if num_common == 0:
                    self.__check_criterion_condition(s, {
                        k: s.multipliers[l],
                        l: s.multipliers[k],
                    })
                if num_common == 1:
                    indices = _all_indices(d1, d2)
                    assert len(indices) == 3
                    a, b, c = tuple(indices)
                    self.__check_criterion_condition(s, {
                        k: D(a, b),
                        l: D(a, c),
                    })
                    self.__check_criterion_condition(s, {
                        k: D(b, a),
                        l: D(b, c),
                    })
                    self.__check_criterion_condition(s, {
                        k: D(c, a),
                        l: D(c, b),
                    })
                elif num_common == 2:
                    pass
                else:
                    assert False, f"Number of common indices == {num_common}"

    def __str__(self):
        return "\n".join(str(s) for s in self.summands)
