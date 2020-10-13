from tensor import *


t = Tensor([
    Summand((D(1,2), D(1,3))),
    Summand((D(2,3), D(2,1))),
    Summand((D(3,1), D(3,2))),
])

print(str(t))
