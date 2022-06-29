import sys

for line in sys.stdin:
    line = line.rstrip()
    first = True
    for p in range(1, 9):
        if str(p) not in line:
            if first:
                line += "   "
                first = False
            else:
                line += ","
            line += str(p)
    print(line)
