pub fn unicode_to_subscript(ch: char) -> Option<char> {
    match ch {
        '+' => Some('₊'),
        '-' => Some('₋'),
        '0' => Some('₀'),
        '1' => Some('₁'),
        '2' => Some('₂'),
        '3' => Some('₃'),
        '4' => Some('₄'),
        '5' => Some('₅'),
        '6' => Some('₆'),
        '7' => Some('₇'),
        '8' => Some('₈'),
        '9' => Some('₉'),
        _ => None
    }
}

pub fn unicode_to_superscript(ch: char) -> Option<char> {
    match ch {
        '+' => Some('⁺'),
        '-' => Some('⁻'),
        '0' => Some('⁰'),
        '1' => Some('¹'),
        '2' => Some('²'),
        '3' => Some('³'),
        '4' => Some('⁴'),
        '5' => Some('⁵'),
        '6' => Some('⁶'),
        '7' => Some('⁷'),
        '8' => Some('⁸'),
        '9' => Some('⁹'),
        _ => None
    }
}
