# Unpacks one level of nesting.
# Example:  [[1], [3, 2]]  ->  [1, 3, 2]
def flatten(l):
    return [item for sublist in l for item in sublist]

# Gets an from list or set.
def get_one_item(container):
    for x in container:
        break
    return x

# Counts the number of each element in the list
# Example:  ["a", "b", "a"]  ->  {"a": 2, "b": 1}
def count_items(l):
    result = {}
    for item in l:
        result[item] = result.get(item, 0) + 1
    return result

# Unites two dicts produced by `count_items`
# Invariant:
#   dict = count_items(a)
#   append_counting_dict(dict, count_items(b))
#   assert dict == count_items(a + b)
# Example:  {"a": 1, "b": 1}, {"a": 2, "c": 2}  ->  {"a": 3, "b": 1, "c": 2}
def append_counting_dict(dst, src):
    for k, v in src.items():
        new_val = dst.get(k, 0) + v
        if new_val == 0:
            del dst[k]
        else:
            dst[k] = new_val
