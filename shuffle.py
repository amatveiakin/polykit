from util import flatten


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
