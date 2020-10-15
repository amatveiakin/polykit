import functools

import format


# Represents a linear combination of any hashable objects
class Linear:
    def __init__(self, data=None):
        if data is None:
            data = {}
        assert isinstance(data, dict)
        self.data = data

    # Counts the number of each element in the list
    # Example:  ["a", "b", "a"]  ->  {"a": 2, "b": 1}
    @staticmethod
    def count(l):
        cnt = {}
        for item in l:
            cnt[item] = cnt.get(item, 0) + 1
        return Linear(cnt)

    def copy(self):
        return Linear(self.data.copy())

    def items(self):
        return self.data.items()

    # Use items() to iterate over Linear
    __iter__ = None

    def __len__(self):
        return len(self.data)

    def __getitem__(self, key):
        return self.data.get(key, 0)

    def __setitem__(self, key, value):
        if value != 0:
            self.data[key] = value
        elif key in self.data:
            del self.data[key]

    def __add__(self, other):
        ret = self.copy()
        ret += other
        return ret

    def __sub__(self, other):
        ret = self.copy()
        ret -= other
        return ret

    def __iadd__(self, other):
        for k, v in other.items():
            self[k] += v
        return self

    def __isub__(self, other):
        for k, v in other.items():
            self[k] -= v
        return self

    def __mul__(self, scalar):
        return Linear({k : v * scalar for k, v in self.items()})
    __rmul__ = __mul__

    def div_int(self, scalar):
        return Linear({k : _div_int(v, scalar) for k, v in self.items()})

    def to_str(self, element_to_str):
        return (
            "\n".join([format.coeff(v) + element_to_str(k) for k, v in sorted(self.items())])
            if self.data
            else format.coeff(0)
        )

    def __str__(self):
        return self.to_str(str)


def tensor_product(
        lhs,            # Linear[A]
        rhs,            # Linear[B]
        basis_product,  # function: A, B -> C
    ):
    ret = Linear({})
    for kl, vl in lhs.items():
        for kr, vr in rhs.items():
            k = basis_product(kl, kr)
            assert ret[k] == 0, f"Tensor product is not unique: {k} = ({kl}) * ({kr})"
            ret[k] = vl * vr
    return ret

def tensor_product_many(
        multipliers,    # iterable[Linear[A]]
        basis_product,  # function: A, A -> A,
    ):
    return functools.reduce(
        lambda lhs, rhs: tensor_product(lhs, rhs, basis_product),
        multipliers
    )


# Returns a / b; asserts that the result is an integer.
def _div_int(x, y):
    result, reminder = divmod(x, y)
    assert reminder == 0
    return result
