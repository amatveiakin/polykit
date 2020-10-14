import format


# Represents a linear combination of any hashable objects
class Linear:
    def __init__(self, data):
        assert isinstance(data, dict)
        self.__data = data

    # Counts the number of each element in the list
    # Example:  ["a", "b", "a"]  ->  {"a": 2, "b": 1}
    @staticmethod
    def count(l):
        cnt = {}
        for item in l:
            cnt[item] = cnt.get(item, 0) + 1
        return Linear(cnt)

    def copy(self):
        return Linear(self.__data.copy())

    def items(self):
        return self.__data.items()

    # Use items() to iterate over Linear
    __iter__ = None

    def __getitem__(self, key):
        return self.__data.get(key, 0)

    def __setitem__(self, key, value):
        if value != 0:
            self.__data[key] = value
        elif key in self.__data:
            del self.__data[key]

    def __add__(self, other):
        ret = self.copy()
        ret += other
        return ret

    def __iadd__(self, other):
        for k, v in other.items():
            self[k] += v
        return self

    def __mul__(self, scalar):
        return Linear({k : v * scalar for k, v in self.items()})
    __rmul__ = __mul__

    def div_int(self, scalar):
        return Linear({k : _div_int(v, scalar) for k, v in self.items()})

    def to_str(self, element_to_str):
        return "\n".join([format.coeff(v) + element_to_str(k) for k, v in self.items()])


# Returns a / b; asserts that the result is an integer.
def _div_int(x, y):
    result, reminder = divmod(x, y)
    assert reminder == 0
    return result
