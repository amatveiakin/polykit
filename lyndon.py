import math

from linear import Linear
from shuffle import shuffle_product_many


def _gen_lyndon_words(alphabet_size, max_length):
    last_w = [0]
    words = [tuple(last_w)]
    while True:
        w = [last_w[i % len(last_w)] for i in range(max_length)]
        while w and w[-1] == alphabet_size - 1:
            w.pop()
        if not w:
            break
        w[-1] += 1
        words.append(tuple(w))
        last_w = w
    return words

class Lyndon:
    def __init__(self, alphabet_size, max_length):
        self.alphabet_size = alphabet_size
        self.max_length = max_length
        self.words = _gen_lyndon_words(alphabet_size, max_length)


# Splits the word into a sequence of nonincreasing Lyndon words using Duval algorithm.
# Such split always exists and is unique (Chen–Fox–Lyndon theorem).
#
# TODO: Add unit tests
def lyndon_factorize(
        word,  # Tuple[x]  where  x is integer  and  0 <= x < alphabet_size
    ):
    result = []
    start = 0
    k = start
    m = start + 1
    n = len(word)
    while k < n:
        if m >= n or word[k] > word[m]:
            l = m - k
            result.append(tuple(word[start:(start+l)]))
            start += l
            k = start
            m = start + 1
        elif word[k] < word[m]:
            m += 1
            k = start
        else:
            k += 1
            m += 1
    if start < n:
        result.append(tuple(word[start:]))
    return result


# Converts a linear combination of words to Lyndon basis.
#
# TODO: Add unit tests
# Optimization potential: Cache word_orig => words_expanded.
# Optimization potential: Don't generate all (N!) results for each (word^N).
def to_lyndon_basis(
        words,  # Linear[word]
    ):
    finished = False
    while not finished:
        words_new = Linear()
        finished = True
        for word_orig, coeff in words.items():
            lyndon_words = lyndon_factorize(word_orig)
            lyndon_word_sum = Linear.count(lyndon_words)
            # TODO: What about len(lyndon_words) > 1 and len(lyndon_word_sum) == 1 ?
            # We should either allow Lyndon_word^N in the basis or assert that it doesn't happen.
            # if len(lyndon_word_sum) == 1:
            if len(lyndon_words) == 1:
                words_new[word_orig] += coeff
                continue
            finished = False
            denominator = 1
            for _, count in lyndon_word_sum.items():
                denominator *= math.factorial(count)
            expanded_word_counts = Linear.count(shuffle_product_many(lyndon_words)).div_int(denominator)
            assert expanded_word_counts[word_orig] == 1, str(word_orig) + " not in " + str(expanded_word_counts)
            # print("Lyndon transform: " + str(word_orig) + " => " + str(expanded_word_counts))
            expanded_word_counts[word_orig] = 0
            words_new += (-count * coeff) * expanded_word_counts
        words = words_new
    return words
