use std::fmt;
use std::hash;

use math_format::{MathFormat, FormatNode};
use math_format_macro::math_format_node;


#[derive(Clone, Copy, PartialEq, PartialOrd, Eq, Ord, Debug, Hash)]
pub enum X {
    Infinity,
    Variable(i32),
}

impl From<i32> for X {
    fn from(idx: i32) -> X { X::Variable(idx) }
}
impl MathFormat for X {
    fn to_format_node(self) -> FormatNode {
        match self {
            X::Infinity => math_format_node!(r"\inf"),
            X::Variable(idx) => math_format_node!(r"<>", idx),
        }
    }
}

#[allow(non_upper_case_globals)]
pub const Inf: X = X::Infinity;


pub trait XArg : Clone + Copy + PartialEq + PartialOrd + Eq + Ord + fmt::Debug + hash::Hash {
    fn as_x(self) -> X;
}

impl XArg for X {
    fn as_x(self) -> X { self }
}
impl XArg for i32 {
    fn as_x(self) -> X { X::Variable(self) }
}


pub fn to_x_vec(variables: &[i32]) -> Vec<X> {
    variables.iter().map(|v| X::Variable(*v)).collect()
}
