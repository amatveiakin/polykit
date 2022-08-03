import re
import sys


# Parameters. The app will try to get from `src` to `dst`.
src = 'a0 a1+ b0-- b1+'
dst = 'a0 b1 a0++ b1-'
# dst = 'a0 a1- b0-- b1+'

equation_strs = '''
a0 a1 == 0
a0- a1- == 0
a0- a1-- == 0
a0+ a1+ == 0
a0+ a1++ == 0
b0 b1 == 0
b0- b1- == 0
b0- b1-- == 0
b0+ b1+ == 0
b0+ b1++ == 0

a0 a1+ + a0 a1- == 0
a0 a1- - a0 a1-- == 0
a0 a1++ + a0 a1- == 0
a0 a1- + a0-- a1+ == 0
a0 a1- + a0-- a1++ == 0
a0 a1- - a0++ a1- == 0
a0 a1- - a0++ a1-- == 0
a0 a1+ + a0 a1-- == 0
a0 a1+ - a0 a1++ == 0
a0 a1+ - a0-- a1+ == 0
a0 a1+ - a0-- a1++ == 0
a0 a1+ + a0++ a1- == 0
a0 a1+ + a0++ a1-- == 0
a0 b1 + b0 a1 == 0
a0 b1- - a0 b1-- == 0
a0 b1+ - a0 b1++ == 0
a0 a1++ + a0 a1-- == 0
a0 a1-- + a0-- a1+ == 0
a0 a1-- + a0-- a1++ == 0
a0 a1-- - a0++ a1- == 0
a0 a1-- - a0++ a1-- == 0
a0 a1++ - a0-- a1+ == 0
a0 a1++ - a0-- a1++ == 0
a0 a1++ + a0++ a1- == 0
a0 a1++ + a0++ a1-- == 0
a0- a1 - a0- a1+ == 0
a0- a1 - a0- a1++ == 0
a0++ a1 + a0- a1 == 0
a0- a1+ - a0- a1++ == 0
a0++ a1 + a0- a1+ == 0
a0- b1- - a0- b1-- == 0
a0- b1- + b0- a1- == 0
a0- b1- + b0- a1-- == 0
a0- b1+ - a0- b1++ == 0
a0++ a1 + a0- a1++ == 0
a0- b1-- + b0- a1- == 0
a0- b1-- + b0- a1-- == 0
a0+ a1 - a0+ a1- == 0
a0+ a1 - a0+ a1-- == 0
a0+ a1 + a0-- a1 == 0
a0+ a1- - a0+ a1-- == 0
a0+ a1- + a0-- a1 == 0
a0+ b1- - a0+ b1-- == 0
a0+ b1+ - a0+ b1++ == 0
a0+ b1+ + b0+ a1+ == 0
a0+ b1+ + b0+ a1++ == 0
a0+ a1-- + a0-- a1 == 0
a0+ b1++ + b0+ a1+ == 0
a0+ b1++ + b0+ a1++ == 0
b0 a1- - b0 a1-- == 0
b0 a1+ - b0 a1++ == 0
b0 b1+ + b0 b1- == 0
b0 b1- - b0 b1-- == 0
b0 b1++ + b0 b1- == 0
b0 b1- + b0-- b1+ == 0
b0 b1- + b0-- b1++ == 0
b0 b1- - b0++ b1- == 0
b0 b1- - b0++ b1-- == 0
b0 b1+ + b0 b1-- == 0
b0 b1+ - b0 b1++ == 0
b0 b1+ - b0-- b1+ == 0
b0 b1+ - b0-- b1++ == 0
b0 b1+ + b0++ b1- == 0
b0 b1+ + b0++ b1-- == 0
b0 b1++ + b0 b1-- == 0
b0 b1-- + b0-- b1+ == 0
b0 b1-- + b0-- b1++ == 0
b0 b1-- - b0++ b1- == 0
b0 b1-- - b0++ b1-- == 0
b0 b1++ - b0-- b1+ == 0
b0 b1++ - b0-- b1++ == 0
b0 b1++ + b0++ b1- == 0
b0 b1++ + b0++ b1-- == 0
b0- a1- - b0- a1-- == 0
b0- a1+ - b0- a1++ == 0
b0- b1 - b0- b1+ == 0
b0- b1 - b0- b1++ == 0
b0++ b1 + b0- b1 == 0
b0- b1+ - b0- b1++ == 0
b0++ b1 + b0- b1+ == 0
b0++ b1 + b0- b1++ == 0
b0+ a1- - b0+ a1-- == 0
b0+ a1+ - b0+ a1++ == 0
b0+ b1 - b0+ b1- == 0
b0+ b1 - b0+ b1-- == 0
b0+ b1 + b0-- b1 == 0
b0+ b1- - b0+ b1-- == 0
b0+ b1- + b0-- b1 == 0
b0+ b1-- + b0-- b1 == 0
a0-- a1- - a0-- a1-- == 0
a0-- a1+ - a0-- a1++ == 0
a0++ a1- + a0-- a1+ == 0
a0++ a1-- + a0-- a1+ == 0
a0-- b1- - a0-- b1-- == 0
a0-- b1+ - a0-- b1++ == 0
a0-- b1+ + b0++ a1- == 0
a0-- b1+ + b0++ a1-- == 0
a0++ a1- + a0-- a1++ == 0
a0++ a1-- + a0-- a1++ == 0
a0-- b1++ + b0++ a1- == 0
a0-- b1++ + b0++ a1-- == 0
a0++ a1- - a0++ a1-- == 0
a0++ a1+ - a0++ a1++ == 0
a0++ b1- - a0++ b1-- == 0
a0++ b1- + b0-- a1+ == 0
a0++ b1- + b0-- a1++ == 0
a0++ b1+ - a0++ b1++ == 0
a0++ b1-- + b0-- a1+ == 0
a0++ b1-- + b0-- a1++ == 0
b0-- a1- - b0-- a1-- == 0
b0-- a1+ - b0-- a1++ == 0
b0-- b1- - b0-- b1-- == 0
b0-- b1+ - b0-- b1++ == 0
b0++ b1- + b0-- b1+ == 0
b0++ b1-- + b0-- b1+ == 0
b0++ b1- + b0-- b1++ == 0
b0++ b1-- + b0-- b1++ == 0
b0++ a1- - b0++ a1-- == 0
b0++ a1+ - b0++ a1++ == 0
b0++ b1- - b0++ b1-- == 0
b0++ b1+ - b0++ b1++ == 0



a1 a0 == 0
a1- a0-- == 0
a1+ a0++ == 0
b1 b0 == 0
b1- b0-- == 0
b1+ b0++ == 0
a1-- a0-- == 0
a1++ a0++ == 0
b1-- b0-- == 0
b1++ b0++ == 0

a1 a0++ + a1 a0- == 0
a1 a0- + a1- a0++ == 0
a1 a0- + a1-- a0++ == 0
a1 a0+ + a1 a0-- == 0
a1 a0+ + a1+ a0-- == 0
a1 a0+ + a1++ a0-- == 0
a1 b0 + b1 a0 == 0
a1 a0-- - a1+ a0-- == 0
a1 a0-- - a1++ a0-- == 0
a1 a0++ - a1- a0++ == 0
a1 a0++ - a1-- a0++ == 0
a1- a0 - a1- a0+ == 0
a1+ a0 + a1- a0 == 0
a1+ a0- + a1- a0 == 0
a1- a0 - a1-- a0 == 0
a1- a0 - a1-- a0+ == 0
a1++ a0 + a1- a0 == 0
a1++ a0- + a1- a0 == 0
a1- a0- - a1-- a0- == 0
a1+ a0 + a1- a0+ == 0
a1+ a0- + a1- a0+ == 0
a1- a0+ - a1-- a0 == 0
a1- a0+ - a1-- a0+ == 0
a1++ a0 + a1- a0+ == 0
a1++ a0- + a1- a0+ == 0
a1- b0 - a1-- b0 == 0
a1- b0- - a1-- b0- == 0
a1- b0+ + b1+ a0- == 0
a1- b0+ - a1-- b0+ == 0
a1- b0+ + b1++ a0- == 0
a1- a0++ - a1-- a0++ == 0
a1- b0-- + b1- a0-- == 0
a1- b0-- - a1-- b0-- == 0
a1- b0-- + b1-- a0-- == 0
a1- b0++ - a1-- b0++ == 0
a1+ a0 - a1+ a0- == 0
a1+ a0 + a1-- a0 == 0
a1+ a0 + a1-- a0+ == 0
a1+ a0 - a1++ a0 == 0
a1+ a0 - a1++ a0- == 0
a1+ a0- + a1-- a0 == 0
a1+ a0- + a1-- a0+ == 0
a1+ a0- - a1++ a0 == 0
a1+ a0- - a1++ a0- == 0
a1+ a0+ - a1++ a0+ == 0
a1+ b0 - a1++ b0 == 0
a1+ b0- + b1- a0+ == 0
a1+ b0- - a1++ b0- == 0
a1+ b0- + b1-- a0+ == 0
a1+ b0+ - a1++ b0+ == 0
a1+ a0-- - a1++ a0-- == 0
a1+ b0-- - a1++ b0-- == 0
a1+ b0++ + b1+ a0++ == 0
a1+ b0++ - a1++ b0++ == 0
a1+ b0++ + b1++ a0++ == 0
b1 b0++ + b1 b0- == 0
b1 b0- + b1- b0++ == 0
b1 b0- + b1-- b0++ == 0
b1 b0+ + b1 b0-- == 0
b1 b0+ + b1+ b0-- == 0
b1 b0+ + b1++ b0-- == 0
b1 b0-- - b1+ b0-- == 0
b1 b0-- - b1++ b0-- == 0
b1 b0++ - b1- b0++ == 0
b1 b0++ - b1-- b0++ == 0
b1- a0 - b1-- a0 == 0
b1- a0- - b1-- a0- == 0
a1++ b0- + b1- a0+ == 0
b1- a0+ - b1-- a0+ == 0
b1- b0 - b1- b0+ == 0
b1+ b0 + b1- b0 == 0
b1+ b0- + b1- b0 == 0
b1- b0 - b1-- b0 == 0
b1- b0 - b1-- b0+ == 0
b1++ b0 + b1- b0 == 0
b1++ b0- + b1- b0 == 0
b1- b0- - b1-- b0- == 0
b1+ b0 + b1- b0+ == 0
b1+ b0- + b1- b0+ == 0
b1- b0+ - b1-- b0 == 0
b1- b0+ - b1-- b0+ == 0
b1++ b0 + b1- b0+ == 0
b1++ b0- + b1- b0+ == 0
a1-- b0-- + b1- a0-- == 0
b1- a0-- - b1-- a0-- == 0
b1- a0++ - b1-- a0++ == 0
b1- b0++ - b1-- b0++ == 0
b1+ a0 - b1++ a0 == 0
a1-- b0+ + b1+ a0- == 0
b1+ a0- - b1++ a0- == 0
b1+ a0+ - b1++ a0+ == 0
b1+ b0 - b1+ b0- == 0
b1+ b0 + b1-- b0 == 0
b1+ b0 + b1-- b0+ == 0
b1+ b0 - b1++ b0 == 0
b1+ b0 - b1++ b0- == 0
b1+ b0- + b1-- b0 == 0
b1+ b0- + b1-- b0+ == 0
b1+ b0- - b1++ b0 == 0
b1+ b0- - b1++ b0- == 0
b1+ b0+ - b1++ b0+ == 0
b1+ a0-- - b1++ a0-- == 0
a1++ b0++ + b1+ a0++ == 0
b1+ a0++ - b1++ a0++ == 0
b1+ b0-- - b1++ b0-- == 0
a1-- a0 - a1-- a0+ == 0
a1++ a0 + a1-- a0 == 0
a1++ a0- + a1-- a0 == 0
a1++ a0 + a1-- a0+ == 0
a1++ a0- + a1-- a0+ == 0
a1-- b0+ + b1++ a0- == 0
a1-- b0-- + b1-- a0-- == 0
a1++ a0 - a1++ a0- == 0
a1++ b0- + b1-- a0+ == 0
a1++ b0++ + b1++ a0++ == 0
b1-- b0 - b1-- b0+ == 0
b1++ b0 + b1-- b0 == 0
b1++ b0- + b1-- b0 == 0
b1++ b0 + b1-- b0+ == 0
b1++ b0- + b1-- b0+ == 0
b1++ b0 - b1++ b0- == 0
'''


def sign_to_str(sign: int):
    if sign == 1:
        return ''
    elif sign == -1:
        return '-'
    else:
        raise Exception('This is a bad sign')

class Expr:
    def __init__(self, funcs: tuple, sign: int, origin: tuple):
        self.funcs = tuple(funcs)
        self.sign = sign
        self.origin = origin

    def __eq__(self, other):
        return self.as_pair() == other.as_pair()

    def __hash__(self):
        return hash(self.as_pair())

    def as_pair(self):
        return (self.funcs, self.sign)

class Eqn:
    def __init__(self, lhs, rhs, sign):
        self.lhs = lhs
        self.rhs = rhs
        self.sign = sign


one_term_eqns = set()
two_term_eqns = set()

one_term_eqn_re = re.compile('(\S+) +(\S+) +== +0')
two_term_eqn_re = re.compile('(\S+) +(\S+) +([+-]) +(\S+) +(\S+) +== +0')

# for line in sys.stdin:
for line in equation_strs.splitlines():
    line = line.strip()
    if not line:
        continue
    if m := one_term_eqn_re.match(line):
        one_term_eqns.add((m.group(1), m.group(2)))
    elif m := two_term_eqn_re.match(line):
        lhs = (m.group(1), m.group(2))
        rhs = (m.group(4), m.group(5))
        sign = -1 if m.group(3) == '+' else 1
        two_term_eqns.add(Eqn(lhs, rhs, sign))

src_expr = Expr(src.split(), 1, None)
dst_funcs = tuple(dst.split())

current_gen = {src_expr.funcs: src_expr}
old_gens = {}
i_gen = 0

def backtrace(expr: Expr, bt: list):
    bt.append(sign_to_str(expr.sign) + ' '.join(expr.funcs))
    if origin := expr.origin:
        origin_expr, origin_eqn = origin
        # origin_eqn_str = f"{' '.join(origin_eqn.lhs)} == {sign_to_str(origin_eqn.sign)} {' '.join(origin_eqn.lhs)}"
        # bt.append('  : ' + origin_eqn_str)
        backtrace(old_gens[origin_expr], bt)

while current_gen:
    print(f'Generation {i_gen}')
    old_gens.update(current_gen)
    next_gen = {}
    for expr in current_gen.values():
        for i in range(len(expr.funcs) - 1):
            func_pair = (expr.funcs[i], expr.funcs[i + 1])
            for eqn in two_term_eqns:
                replacement = None
                if func_pair == eqn.lhs:
                    replacement = eqn.rhs
                elif func_pair == eqn.rhs:
                    replacement = eqn.lhs
                if replacement:
                    funcs_new = list(expr.funcs)
                    assert(len(replacement) == 2)
                    funcs_new[i] = replacement[0]
                    funcs_new[i + 1] = replacement[1]
                    origin = (expr.funcs, eqn)
                    expr_new = Expr(funcs_new, expr.sign * eqn.sign, origin)
                    funcs_new = expr_new.funcs
                    # print(expr_new.funcs, expr_new.sign)
                    if funcs_new in old_gens:
                        assert(expr_new == old_gens[funcs_new])
                    elif funcs_new in next_gen:
                        assert(expr_new == next_gen[funcs_new])
                    else:
                        next_gen[funcs_new] = expr_new
                        if funcs_new == dst_funcs:
                            print(f'Found in {i_gen} iterations')
                            history = []
                            backtrace(expr_new, history)
                            history.reverse()
                            print('\n'.join(history))
                            exit(0)
    print([e.funcs for e in next_gen.values()])
    current_gen = next_gen
    i_gen += 1

print('Not found')
