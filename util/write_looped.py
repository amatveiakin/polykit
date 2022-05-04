import re


tmpl = """
    tensor_product(
      QLiVec   (2, choose_indices_one_based(points, std::vector{$3,$4,$5,$6,$7,$2})),
      typeC_QLi(1, choose_indices_one_based(points, std::vector{$1,$2,$7,$8}))
    ),
""".strip('\n')
num_vars = 12
num_summands = 6

vars_re = re.compile('(\$[0-9]+)')
split = re.split(vars_re, tmpl)
for shift in range(num_summands):
    s = ""
    for segment in split:
        if segment.startswith("$"):
            old_var = int(segment[1:])
            assert 1 <= old_var <= num_vars
            new_var = (old_var + shift - 1) % num_vars + 1
            s += str(new_var)
        else:
            s += segment
    print(s)
