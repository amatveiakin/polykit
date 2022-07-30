import more_itertools
import re
import sys

# Parameters. The app will replace all instances of `variable` with `variable + offset`.
variable = 'p'
offset = 1

variable_expr_re = re.compile(f'^(?:\\{{)?([+-]?[0-9]+)?{variable}([+-][0-9]+)?(?:\\}})?$')


def value_or(x, default):
    return x if x is not None else default

def read_atom(text_iter):
    ch = next(text_iter)
    if ch == '{':
        ret = ch
        while (ch := text_iter.peek(None)) != '}':
            ret += read_atom(text_iter)
        ret += next(text_iter)
        return ret
    else:
        return ch

def apply_offset(expr):
    if variable in expr:
        m = variable_expr_re.match(expr)
        if m is None:
            raise Exception(f'Cannot update this expression: "{expr}"')
        a = int(value_or(m.group(1), 1))
        assert(a >= 1)
        b_old = int(value_or(m.group(2), 0))
        b_new = b_old + a * offset
        ret = variable
        if a != 1:
            ret = f'{a}{ret}'
        if b_new != 0:
            ret = f'{ret}{b_new:+d}'
        if len(ret) > 1:
            ret = f'{{{ret}}}'
        return ret
    else:
        return expr

after_backslash = False
output = ''
text = sys.stdin.read()
text_iter = more_itertools.peekable(iter(text))
for ch in text_iter:
    if after_backslash:
        output += ch
        after_backslash = False
    elif ch == '\\':
        output += ch
        after_backslash = True
    elif ch in ['_', '^']:
        output += ch
        output += apply_offset(read_atom(text_iter))
    else:
        output += ch
print(output)
