use std::fmt;
use std::hash;


#[derive(Clone, Copy, PartialEq, PartialOrd, Eq, Ord, Debug, Hash)]
pub enum X {
    Infinity,
    Variable(i32),
}

impl From<i32> for X {
    fn from(idx: i32) -> X { X::Variable(idx) }
}
impl fmt::Display for X {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            X::Infinity => write!(f, "Inf"),
            X::Variable(idx) => write!(f, "{}", idx),
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
