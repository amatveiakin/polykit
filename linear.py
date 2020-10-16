import functools

import format

from util import args_to_iterable


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

    def __neg__(self):
        return Linear() - self

    def __add__(self, other):
        ret = self.copy()
        ret += other
        return ret

    def __sub__(self, other):
        ret = self.copy()
        ret -= other
        return ret

    def __iadd__(self, other):
        for obj, coeff in other.items():
            self[obj] += coeff
        return self

    def __isub__(self, other):
        for obj, coeff in other.items():
            self[obj] -= coeff
        return self

    def __mul__(self, scalar):
        return self.map_coeff(lambda coeff: coeff * scalar)
    __rmul__ = __mul__

    def div_int(self, scalar):
        return self.map_coeff(lambda coeff: _div_int(coeff, scalar))

    def map_obj(self, func):
        return Linear({func(obj): coeff for obj, coeff in self.items()})

    def map_coeff(self, func):
        return Linear({obj: func(coeff) for obj, coeff in self.items()})

    def to_str(self, element_to_str):
        return (
            "\n".join([format.coeff(coeff) + element_to_str(obj) for obj, coeff in sorted(self.items())])
            if self.data
            else format.coeff(0)
        )

    def __str__(self):
        return self.to_str(str)


def _tensor_product_two(
        lhs,      # Linear[A]
        rhs,      # Linear[B]
        product,  # function: A, B -> C
    ):
    ret = Linear({})
    for obj_l, coeff_l in lhs.items():
        for obj_r, coeff_r in rhs.items():
            obj = product(obj_l, obj_r)
            assert ret[obj] == 0, f"Tensor product is not unique: {obj} = ({obj_l}) * ({obj_r})"
            ret[obj] = coeff_l * coeff_r
    return ret

def tensor_product(
        *multipliers,  # iterable[Linear[A]]
        product,       # function: A, A -> A
    ):
    return functools.reduce(
        lambda lhs, rhs: _tensor_product_two(lhs, rhs, product),
        args_to_iterable(multipliers)
    )


# Returns a / b; asserts that the result is an integer.
def _div_int(x, y):
    result, reminder = divmod(x, y)
    assert reminder == 0
    return result
