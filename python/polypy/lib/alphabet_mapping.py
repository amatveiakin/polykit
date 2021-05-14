from typing import Tuple


ALPHABET_MAPPING_MAX_D_DIMENSION = 20

def d_to_alphabet(
        pair: Tuple[int, int],  # (a, b)  where  1 <= a < b
    ):
    return _mapping.d_to_alphabet(pair)

def d_from_alphabet(
        alpha: int  # 0 <= alpha
    ):
    return _mapping.d_from_alphabet(alpha)


class _AlphabetMapping:
    def __init__(
            self,
            dimension: int,
        ):
        self.dimension = dimension
        alphabet_size = dimension * (dimension-1) // 2
        self.d_from_alphabet_mapping = [None] * alphabet_size
        for b in range(dimension):
            for a in range(b):
                pair = (a+1, b+1)
                self.d_from_alphabet_mapping[self.d_to_alphabet(pair)] = pair
        assert not None in self.d_from_alphabet_mapping

    def d_to_alphabet(
            self,
            pair: Tuple[int, int],
        ):
        (a, b) = pair
        assert 1 <= a and a < b, f"Invalid coordinates: ({a}, {b})"
        assert b <= self.dimension, f"Coordinate {b} exceeds max supported dimension {self.dimension}"
        a -= 1
        b -= 1
        return b*(b-1)//2 + a

    def d_from_alphabet(
            self,
            alpha: int,
        ):
        return self.d_from_alphabet_mapping[alpha]

_mapping = _AlphabetMapping(ALPHABET_MAPPING_MAX_D_DIMENSION)
