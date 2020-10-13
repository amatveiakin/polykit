# Unpacks one level of nesting.
# Example:  [[1], [3, 2]]  ->  [1, 3, 2]
def flatten(l):
    return [item for sublist in l for item in sublist]

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
        dst[k] = dst.get(k, 0) + v
