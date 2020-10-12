# def single_element(container):
#     assert len(container) == 1
#     for element in container:
#         return element

kMinus = "−"

def as_substript(s):
    return (str(s)
        .replace("+", "₊")
        .replace("-", "₋")
        .replace("0", "₀")
        .replace("1", "₁")
        .replace("2", "₂")
        .replace("3", "₃")
        .replace("4", "₄")
        .replace("5", "₅")
        .replace("6", "₆")
        .replace("7", "₇")
        .replace("8", "₈")
        .replace("9", "₉")
    )


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
        return f"(x{as_substript(self.a)} {kMinus} x{as_substript(self.b)})"


def common_indices(d1, d2):
    return set(d1.as_tuple()) & set(d2.as_tuple())

def all_indices(d1, d2):
    return set(d1.as_tuple()) | set(d2.as_tuple())

# def num_common_indices(d1, d2):
#     return len(common_indices(d1, d2))

# def other_index(d, index):
#     assert d.a == index or d.b == index
#     return d.a if d.b == index else d.b


# class Product:
#     def __init__(
#             self,
#             multipliers,  
#         ):
#         self.multipliers = tuple(sorted(multipliers, D.as_tuple))
#         # self.multipliers = tuple(sorted(multipliers))


def format_coeff(coeff):
    if coeff == 0:
        return " 0 "
    if coeff == 1:
        return " + "
    elif coeff == -1:
        return " - "
    else:
        return "{:+} ".format(coeff)
        

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
        return format_coeff(self.coeff) + "⊗ ".join(str(s) for s in self.multipliers)

def change_multipliers(
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
    
    def check_criterion_condition(
            self,
            summand,        # Summand
            substitutions,  # as in change_multipliers
        ):
        new_multipliers = change_multipliers(summand.multipliers, substitutions)
        new_coeff = self.summands_dict.get(new_multipliers) or 0
        new_summand = Summand(new_multipliers, coeff=new_coeff)
        assert summand.coeff == new_summand.coeff, f"Criterion failed:\n  {summand}\nvs\n  {new_summand}"

    def check_criterion(self):
        for k in range(0, self.weight - 1):
            l = k+1
            for s in self.summands:
                d1 = s.multipliers[k]
                d2 = s.multipliers[l]
                common = common_indices(d1, d2)
                num_common = len(common)
                if num_common == 0:
                    # other = list(s.multipliers)
                    # other[k], other[l] = other[l], other[k]
                    # assert self.summands_dict[tuple(other)] == s.coeff
                    self.check_criterion_condition(s, {
                        k: s.multipliers[l],
                        l: s.multipliers[k],
                    })
                if num_common == 1:
                    indices = all_indices(d1, d2)
                    assert len(indices) == 3
                    # b = single_element(common_indices)
                    # a = other_index(d0, b)
                    # c = other_index(d1, b)
                    a, b, c = tuple(indices)
                    self.check_criterion_condition(s, {
                        k: D(a, b),
                        l: D(a, c),
                    })
                    self.check_criterion_condition(s, {
                        k: D(b, a),
                        l: D(b, c),
                    })
                    self.check_criterion_condition(s, {
                        k: D(c, a),
                        l: D(c, b),
                    })
                elif num_common == 2:
                    pass
                else:
                    assert False, f"Common indices == {num_common}"

    def __str__(self):
        return "\n".join(str(s) for s in self.summands)



t = Tensor([
    Summand((D(1,2), D(1,3))),
    Summand((D(2,3), D(2,1))),
    Summand((D(3,1), D(3,2))),
])

print(str(t))
