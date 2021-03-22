# TODO: Dedup against /python/util.h

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
