enable_unicode = False
enable_monospace_font_correction = True

def set_enable_unicode(v):
    global enable_unicode
    enable_unicode = v
    _gen_ops()

def set_enable_monospace_font_correction(v):
    global enable_monospace_font_correction
    enable_monospace_font_correction = v
    _gen_ops()


minus = None
otimes = None
shuffle = None

# add spaces to prevent wide characters from overlapping when using monospace fonts
def _monofont_op_correction(op):
    return op + (" " if enable_monospace_font_correction else "")

def _gen_ops():
    global minus, otimes, shuffle
    minus = "−" if enable_unicode else "-"
    otimes = _monofont_op_correction("⊗") if enable_unicode else "*"
    shuffle = _monofont_op_correction("⧢") if enable_unicode else "#"

_gen_ops()

_subscript_map = {
    "+": "₊",
    "-": "₋",
    minus: "₋",
    "0": "₀",
    "1": "₁",
    "2": "₂",
    "3": "₃",
    "4": "₄",
    "5": "₅",
    "6": "₆",
    "7": "₇",
    "8": "₈",
    "9": "₉",
}


def fix_minus(s):
    return s.replace("-", minus)

def substript(n):
    return "".join([_subscript_map[c] for c in str(n)]) if enable_unicode else str(n)


def coeff(x):
    def impl(x):
        if x == 0:
            return " 0 "
        if x == 1:
            return " + "
        elif x == -1:
            return " - "
        else:
            return "{:+} ".format(x)
    return fix_minus(impl(x))


def print_expression(title, expr):
    print(f"{title} - {len(expr)} terms:\n{expr}\n")
