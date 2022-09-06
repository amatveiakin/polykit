import math

from sortedcontainers import SortedDict

from .linear import Linear
from .shuffle import shuffle_product


# def _gen_lyndon_words(alphabet_size, max_length):
#     last_w = [0]
#     words = [tuple(last_w)]
#     while True:
#         w = [last_w[i % len(last_w)] for i in range(max_length)]
#         while w and w[-1] == alphabet_size - 1:
#             w.pop()
#         if not w:
#             break
#         w[-1] += 1
#         words.append(tuple(w))
#         last_w = w
#     return words

# class Lyndon:
#     def __init__(self, alphabet_size, max_length):
#         self.alphabet_size = alphabet_size
#         self.max_length = max_length
#         self.words = _gen_lyndon_words(alphabet_size, max_length)


# Splits the word into a sequence of nonincreasing Lyndon words using Duval algorithm.
# Such split always exists and is unique (Chen–Fox–Lyndon theorem).
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
def to_lyndon_basis(
        words,  # Linear[word]
    ):
    assert isinstance(words, Linear)
    terms_to_convert = SortedDict(words.data.items())
    terms_converted = Linear()
    while terms_to_convert:
        word_orig, coeff = terms_to_convert.popitem(-1)
        lyndon_words = lyndon_factorize(word_orig)
        assert len(lyndon_words) > 0
        lyndon_word_expr = Linear.from_collection(lyndon_words)
        if len(lyndon_words) == 1:
            terms_converted.add_to(word_orig, coeff)
            continue
        denominator = 1
        for _, count in lyndon_word_expr.data.items():
            denominator *= math.factorial(count)
        expanded_word_expr = Linear.from_collection(shuffle_product(lyndon_words)).div_int(denominator)
        assert expanded_word_expr[word_orig] == 1, f"{word_orig} not in {expanded_word_expr}"
        # print(f"Lyndon transform: {word_orig} => {lyndon_words} =>\n{expanded_word_expr}")
        expanded_word_expr.add_to(word_orig, -1)
        for word, inner_coeff in expanded_word_expr.data.items():
            assert terms_converted[word] == 0
            if not word in terms_to_convert:
                terms_to_convert[word] = 0
            terms_to_convert[word] -= coeff * inner_coeff
    return terms_converted
