use std::cmp::Ordering;
use std::fmt;
use std::ops::{Index, Range, RangeFull};

use super::linear::{Linear, TensorProduct};


// TODO: Compact representation (entire Delta in a single i8) + inlined vector
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct Delta {
    a: i8,
    b: i8,
}

// Synchronize order with C++ (and thus with encoded Delta).
impl Ord for Delta {
    fn cmp(&self, other: &Self) -> Ordering {
        (self.b, self.a).cmp(&(other.b, other.a))
    }
}
impl PartialOrd for Delta {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl Delta {
    pub fn new(a: i32, b: i32) -> Delta {
        if a < b {
            Delta{ a: a as i8, b: b as i8 }
        } else {
            Delta{ a: b as i8, b: a as i8 }
        }
    }

    pub fn a(&self) -> i8 { self.a }
    pub fn b(&self) -> i8 { self.b }

    pub fn is_nil(&self) -> bool { self.a == self.b }
}

impl fmt::Display for Delta {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "({}-{})", self.a, self.b)
    }
}


#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct DeltaProduct(pub Vec<Delta>);

impl Index<Range<usize>> for DeltaProduct {
    type Output = [Delta];
    fn index(&self, range: Range<usize>) -> &Self::Output {
        &self.0[range]
    }
}
impl Index<RangeFull> for DeltaProduct {
    type Output = [Delta];
    fn index(&self, _: RangeFull) -> &Self::Output {
        &self.0[..]
    }
}

impl TensorProduct for DeltaProduct {
    fn tensor_product(&self, rhs: &DeltaProduct) -> DeltaProduct {
        DeltaProduct([&self[..], &rhs[..]].concat())
    }
}

impl fmt::Display for DeltaProduct {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let element_strings: Vec<_> = self.0.iter().map(|d| d.to_string()).collect();
        write!(f, "{}", element_strings.join(" * "))
    }
}


pub type DeltaExpr = Linear<DeltaProduct>;

#[allow(non_snake_case)]
pub fn D(a: i32, b: i32) -> DeltaExpr {
    DeltaExpr::single(DeltaProduct(Vec::from([Delta::new(a, b)])))
}
