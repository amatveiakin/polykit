import functools

from . import format
from .util import args_to_iterable, get_one_item, to_hashable


# Represents a linear combination of any hashable objects.
# Interface mirros C++ implementation (see pybind/cpp_lib/py_util.h).
# Does not support annotations.
class Linear:
    def __init__(self, data=None):
        if data is None:
            data = {}
        assert isinstance(data, dict)
        self.data = data

    @staticmethod
    def single(obj):
        return Linear({obj: 1})

    @staticmethod
    def from_cpp(expr):
        ret = Linear()
        for term, coeff in expr:
            ret.add_to(to_hashable(term), coeff)
        return ret

    def copy(self):  return Linear(self.data.copy())

    def is_zero(self):  return not self.data
    def is_blank(self):  return self.is_zero()

    def l0_norm(self):  return len(self.data)
    def num_terms(self):  return self.l0_norm()
    def l1_norm(self):  return sum([abs(coeff) for _, coeff in self])

    def __iter__(self):  return iter(self.data.items())
    def __getitem__(self, obj):  return self.data.get(obj, 0)

    def add_to(self, obj, additive):
        value = self[obj] + additive
        if value != 0:
            self.data[obj] = value
        elif obj in self.data:
            del self.data[obj]

    def element(self):  return get_one_item(self.data)

    def __eq__(self, other): return self.data == other.data

    def __pos__(self):  return self
    def __neg__(self):  return Linear() - self

    def __add__(self, other):
        ret = self.copy()
        ret += other
        return ret

    def __sub__(self, other):
        ret = self.copy()
        ret -= other
        return ret

    def __iadd__(self, other):
        assert isinstance(other, Linear)
        for obj, coeff in other:
            self.add_to(obj, coeff)
        return self

    def __isub__(self, other):
        assert isinstance(other, Linear)
        for obj, coeff in other:
            self.add_to(obj, -coeff)
        return self

    def __mul__(self, scalar):
        return self._mapped_coeff(lambda coeff: coeff * scalar)
    __rmul__ = __mul__

    def div_int(self, scalar):
        return self._mapped_coeff(lambda coeff: _div_int(coeff, scalar))

    def dived_int(self, scalar):
        ret = self.copy()
        ret.dived_int(scalar)
        return ret

    def mapped(self, func):
        # Don't use a list comprehension in case func is not injective
        ret = Linear()
        for obj, coeff in self:
            ret.add_to(func(obj), coeff)
        return ret

    def mapped_expanding(self, func):
        ret = Linear()
        for obj, coeff in self:
            ret += coeff * func(obj)
        return ret

    def filtered(self, predicate):
        return Linear({obj: coeff for obj, coeff in self if predicate(obj)})

    def to_str(self, element_to_str):
        LINE_LIMIT = 300
        if self.is_zero():
            return "\n" + format.coeff(0) + "\n"
        header = f"# {self.num_terms()} terms, |coeff| = {self.l1_norm()}:\n"
        lines = []
        for obj, coeff in sorted(self):
            if len(lines) < LINE_LIMIT:
                lines.append(format.coeff(coeff) + element_to_str(obj))
            else:
                lines.append("...")
                break
        return header + "\n".join(lines) + "\n"

    def __str__(self):
        return self.to_str(str)

    def _mapped_coeff(self, func):
        return Linear({obj: func(coeff) for obj, coeff in self})


def _tensor_product_two(
        lhs,      # Linear[A]
        rhs,      # Linear[B]
        product,  # function: A, B -> C
    ):
    ret = Linear()
    for obj_l, coeff_l in lhs:
        for obj_r, coeff_r in rhs:
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
