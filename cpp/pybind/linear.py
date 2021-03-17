# TODO: Dedup against /python/linear.h

from dataclasses import dataclass

import format


@dataclass(order=True, frozen=True)
class Annotation:
    name: str
    def __str__(self):
        return self.name


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

    # Example:  [("a", 2), ("b", 1)]  ->  {"a": 2, "b": 1}
    @staticmethod
    def from_pairs(l):
        data = {}
        for obj, coeff in l:
            data[obj] = coeff
        return Linear(data)

    def copy(self):
        return Linear(self.data.copy())

    # TODO: Split into annotations and no annotations
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

    def __eq__(self, other):
        return self.data == other.data

    def __pos__(self):
        return self

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
        return self.mapped_coeff(lambda coeff: coeff * scalar)
    __rmul__ = __mul__

    def div_int(self, scalar):
        return self.mapped_coeff(lambda coeff: _div_int(coeff, scalar))

    def l1_norm(self):
        assert not self.has_annotations()
        return sum([abs(coeff) for _, coeff in self.items()])

    def mapped_obj(self, func):
        # Don't use a list comprehension in case func is not injective
        ret = Linear()
        for obj, coeff in self.items():
            ret += Linear({func(obj): coeff})
        return ret

    def mapped_coeff(self, func):
        return Linear({obj: func(coeff) for obj, coeff in self.items()})

    def filtered_obj(self, predicate):
        return Linear({obj: coeff for obj, coeff in self.items() if predicate(obj)})

    def has_annotations(self):
        return len(self.annotations()) > 0

    def annotations(self):
        return self.filtered_obj(lambda e: isinstance(e, Annotation))

    def without_annotations(self):
        return self.filtered_obj(lambda e: not isinstance(e, Annotation))

    def annotated(self, s):
        return self + Linear({Annotation(s): 1})

    def annotated_with_function(self, name, args):
        return self.annotated(name + "(" + ",".join([str(x) for x in args]) + ")")

    def to_str(self, element_to_str):
        return (
            "\n".join([format.coeff(coeff) + element_to_str(obj) for obj, coeff in sorted(self.items())])
            if self.data
            else format.coeff(0)
        )

    def __str__(self):
        return self.to_str(str)


# Returns a / b; asserts that the result is an integer.
def _div_int(x, y):
    result, reminder = divmod(x, y)
    assert reminder == 0
    return result
