from util import args_to_iterable, flatten


def _concat_to_each(words, suffix):
    return [w + suffix for w in words]

# Returns shuffle product of two words as a List of words.
# Rules:
#   1 ⧢ v = v
#   u ⧢ 1 = u
#   ua ⧢ vb = (u ⧢ vb)a + (ua ⧢ v)b
# Optimization potential: Cache results.
def _shuffle_product_two(u, v):
    if not u:
        return [v]
    if not v:
        return [u]
    a = u[-1]
    b = v[-1]
    u = u[:-1]
    v = v[:-1]
    return (
        _concat_to_each(_shuffle_product_two(u, v+(b,)), (a,)) +
        _concat_to_each(_shuffle_product_two(u+(a,), v), (b,))
    )

# Returns shuffle product of a list of words as a List of words.
# Optimization potential: Do de-duping in the process.
def _shuffle_product_many(words):
    l = len(words)
    assert l > 0
    if l == 1:
        return words
    else:
        return flatten(
            [_shuffle_product_two(w, words[-1]) for w in _shuffle_product_many(words[:-1])]
        )

def shuffle_product(*words):
    return _shuffle_product_many(args_to_iterable(words))
