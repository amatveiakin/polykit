# Unpacks one level of nesting.
# Example:  [[1], [3, 2]]  ->  [1, 3, 2]
def flatten(l):
    return [item for sublist in l for item in sublist]

# Gets an from list or set.
def get_one_item(container):
    for x in container:
        break
    return x

# Generates all possible words of a given length in an alphabet
# of a given size, in lexicographic order.
# Example:  generate_all_words(2, 3)  ->
#           [(0, 0, 0), (0, 0, 1), (0, 1, 0), ..., (1, 1, 1)]
def generate_all_words(alphabet_size, length):
    ret = [0] * length
    while ret[0] < alphabet_size:
        yield tuple(ret)
        i = len(ret) - 1
        ret[i] += 1
        while i > 0 and ret[i] == alphabet_size:
            ret[i] = 0
            i -= 1
            ret[i] += 1
