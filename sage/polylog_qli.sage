diff_to_generator = {}
generator_names = []
max_variables = 16

for a in range(1, max_variables+1):
    for b in range(1, a):
        generator_index = len(generator_names)
        generator_names.append(f'x{b}_x{a}')
        diff_to_generator[(a,b)] = generator_index
        diff_to_generator[(b,a)] = generator_index


# F = LieAlgebra(ZZ, names=generator_names)
F = FreeAlgebra(ZZ, names=generator_names)
# F = FreeAlgebra(ZZ, names=generator_names, implementation='letterplace')
# M = F.monoid()  # TODO: ?


def cross_ratio(p):
    assert len(p) % 2 == 0
    ret = 0
    n = len(p)
    for i in range(n):
        ret += (-1)**i * F.gen(diff_to_generator[p[i], p[(i+1)%n]])
    return ret

# Symbol for (1 - cross_ratio(p))
def neg_cross_ratio(p):
    assert len(p) == 4
    return cross_ratio([p[0], p[2], p[1], p[3]])

def qli_node_func(p):
    assert len(p) == 4
    return (
        neg_cross_ratio([p[0][0], p[1][0], p[2][0], p[3][0]])
        if p[0][1] else
        -neg_cross_ratio([p[1][0], p[2][0], p[3][0], p[0][0]])
    )

def qli_impl(weight, points):
    num_points = len(points)
    assert num_points >= 4 and num_points % 2 == 0
    min_weight = (num_points - 2) / 2
    assert weight >= min_weight
    def subsums():
        nonlocal weight, points
        ret = 0
        for i in range(num_points - 3):
            foundation = points[:i+1] + points[i+3:]
            ret += qli_node_func(points[i:i+4]) * qli_impl(weight - 1, foundation)
        return ret
    if weight == min_weight:
        if num_points == 4:
            return qli_node_func(points)
        else:
            return subsums()
    else:
        ret = cross_ratio([p[0] for p in points]) * qli_impl(weight - 1, points)
        if num_points > 4:
            ret += subsums()
        return ret

def QLi(weight, points):
    tagged_points = []
    for i in range(len(points)):
        tagged_points.append((points[i], (i+1) % 2 == 1))
    return qli_impl(weight, tagged_points)
