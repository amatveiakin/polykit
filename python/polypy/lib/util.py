# Unpacks one level of nesting.
# Example:  [[1], [3, 2]]  ->  [1, 3, 2]
def flatten(l):
    return [item for sublist in l for item in sublist]

def rotate_list(l, n):
    n = n % len(l)
    return l[n:] + l[:n]

# Gets an from list or set.
def get_one_item(container):
    for x in container:
        break
    return x

# Returns a copy of the list without elements at certain indices.
def removed_indices(l, indices):
    return [l[i] for i in range(len(l)) if i not in indices]

def is_iterable(v):
    try:
        iter(v)
    except TypeError:
        return False
    else:
        return True

# For vararg functions that can alternatively accept an iterable.
def args_to_iterable(args):
    assert len(args) > 0
    return args[0] if len(args) == 1 and is_iterable(args[0]) else args

# Attempts to turn value into an immutable object that can be used as dict key.
def to_hashable(v):
    return tuple(map(to_hashable, v)) if isinstance(v, (tuple, list)) else v
