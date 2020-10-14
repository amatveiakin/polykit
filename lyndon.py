import math

from shuffle import shuffle_product_many
from util import count_items, append_counting_dict


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
        words,  # word -> coeff
    ):
    finished = False
    while not finished:
        words_new = {}
        finished = True
        for word_orig, coeff in words.items():
            lyndon_words = lyndon_factorize(word_orig)
            lyndon_word_counts = count_items(lyndon_words)
            # TODO: What about len(lyndon_words) > 1 and len(lyndon_word_counts) == 1 ?
            # We should either allow Lyndon_word^N in the basis or assert that it doesn't happen.
            # if len(lyndon_word_counts) == 1:
            if len(lyndon_words) == 1:
                append_counting_dict(words_new, {word_orig: coeff})
                continue
            finished = False
            # denominator = 1
            # numerator = []
            # for word, count in lyndon_word_counts.items():
            #     denominator *= math.factorial(count)
            #     numerator.append(word * count)
            # expanded_word_counts = {
            #     word: _div_int(count, denominator)
            #     for (word, count)
            #     in count_items(shuffle_product_many(numerator)).items()
            # }
            denominator = 1
            for count in lyndon_word_counts.values():
                denominator *= math.factorial(count)
            expanded_word_counts = {
                word: _div_int(count, denominator)
                for word, count
                in count_items(shuffle_product_many(lyndon_words)).items()
            }
            assert expanded_word_counts.get(word_orig) == 1, str(word_orig) + " not in " + str(expanded_word_counts)
            # print("Lyndon transform: " + str(word_orig) + " => " + str(expanded_word_counts))
            del expanded_word_counts[word_orig]
            append_counting_dict(
                words_new,
                {
                    word: -count * coeff
                    for word, count
                    in expanded_word_counts.items()
                }
            )
        words = words_new
    return words

# Returns a / b; asserts that the result is an integer.
def _div_int(x, y):
    result, reminder = divmod(x, y)
    assert reminder == 0
    return result
