minus = "−"

# add spaces to prevent wide characters from overlapping when using monospace fonts
otimes = "⊗ "
shuffle = "⧢ "

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
    return "".join([_subscript_map[c] for c in str(n)])


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
