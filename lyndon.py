from util import flatten


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


# TODO: Add unit tests
# Splits the word into a sequence of nonincreasing Lyndon words using Duval algorithm.
# Such split always exists and is unique (Chen–Fox–Lyndon theorem).
def lyndon_factorize(
        word,  # List[x]  where  x is integer  and  0 <= x < alphabet_size
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


def _concat_many(word_collection, suffix):
    return [w + suffix for w in word_collection]

# Returns shuffle product of two words as a List of words.
# Rules:
#   1 ⧢ v = v
#   u ⧢ 1 = u
#   ua ⧢ vb = (u ⧢ vb)a + (ua ⧢ v)b
# Optimization potential: Cache results.
def shuffle_product(u, v):
    if not u:
        return [v]
    if not v:
        return [u]
    a = u[-1]
    b = v[-1]
    u = u[:-1]
    v = v[:-1]
    return (
        _concat_many(shuffle_product(u, v+(b,)), (a,)) +
        _concat_many(shuffle_product(u+(a,), v), (b,))
    )

# Returns shuffle product of a list of words as a List of words.
# Optimization potential: Do de-duping in the process.
def shuffle_product_many(words):
    l = len(words)
    assert l > 0
    if l == 1:
        return words
    else:
        return flatten(
            [shuffle_product(w, words[-1]) for w in shuffle_product_many(words[:-1])]
        )
