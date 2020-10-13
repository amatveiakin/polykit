class AlphabetMapping:
    def __init__(
            self,
            dimension,  # integer
        ):
        self.dimension = dimension
        self.alphabet_size = dimension * (dimension-1) // 2
        self.from_alphabet_mapping = [None] * self.alphabet_size
        for b in range(dimension):
            for a in range(b):
                pair = (a+1, b+1)
                self.from_alphabet_mapping[self.to_alphabet(pair)] = pair
        assert not None in self.from_alphabet_mapping

    def to_alphabet(
            self,
            pair,  # (a, b)  where  a, b are integers  and  1 <= a < b <= dimension
        ):
        (a, b) = pair
        assert 1 <= a and a < b and b <= self.dimension, [a, b, self.dimension]
        a -= 1
        b -= 1
        return b*(b-1)//2 + a

    def from_alphabet(
            self,
            alpha,  # integer;  0 <= alpha < alphabet_size
        ):
        return self.from_alphabet_mapping[alpha]
