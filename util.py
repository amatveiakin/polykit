# Unpacks one level of nesting.
# Example:  [[1], [3, 2]]  ->  [1, 3, 2]
def flatten(l):
    return [item for sublist in l for item in sublist]

# Gets an from list or set.
def get_one_item(container):
    for x in container:
        break
    return x
