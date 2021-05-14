import itertools
import time

from polypy import Linear, to_lyndon_basis, QLiSymm


def check_qlisymm_sum(weight, num_points):
  expr = Linear()
  for num_args in range(4, num_points+1, 2):
    for args in itertools.combinations(range(1, num_points+1), num_args):
      expr += (-1) ** (sum(args) + num_args // 2) * QLiSymm(weight, args)
  assert(to_lyndon_basis(expr) == Linear())


for weight in range(3, 7):
  iterations = 10 if weight < 5 else (3 if weight < 6 else 1)
  num_points = 9
  start = time.time()
  for _ in range(iterations):
    check_qlisymm_sum(weight, num_points)
  duration = (time.time() - start) / iterations
  print(f"({weight}, {duration:.3f})")
