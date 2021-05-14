import time

from polypy import QLi


for weight in range(4, 8):
  iterations = 20 if weight < 6 else 5
  num_points = 8
  start = time.time()
  for _ in range(iterations):
    QLi(weight, list(range(1, num_points+1)))
  duration = (time.time() - start) / iterations
  print(f"({weight}, {duration:.3f})")
