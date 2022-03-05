use std::cmp::Ordering;
use std::fmt;
use std::ops::{Index, Range, RangeFrom, RangeTo, RangeFull};
use smallvec::{SmallVec, smallvec};

use crate::base::{VectorLike};
use crate::math::{Linear, TensorProduct};


// TODO: Compact representation (entire Delta in a single i8)
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
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
pub struct DeltaProduct(pub SmallVec<[Delta; 8]>);

impl From<&[Delta]> for DeltaProduct {
    fn from(slice: &[Delta]) -> Self {
        DeltaProduct(<SmallVec::<[Delta; 8]> as From<&[Delta]>>::from(slice))
    }
}

impl Index<usize> for DeltaProduct {
    type Output = Delta;
    fn index(&self, idx: usize) -> &Self::Output {
        &self.0[idx]
    }
}
impl Index<Range<usize>> for DeltaProduct {
    type Output = [Delta];
    fn index(&self, range: Range<usize>) -> &Self::Output {
        &self.0[range]
    }
}
impl Index<RangeFrom<usize>> for DeltaProduct {
    type Output = [Delta];
    fn index(&self, range: RangeFrom<usize>) -> &Self::Output {
        &self.0[range]
    }
}
impl Index<RangeTo<usize>> for DeltaProduct {
    type Output = [Delta];
    fn index(&self, range: RangeTo<usize>) -> &Self::Output {
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
        let mut ret = self.0.clone();
        // Note: `ret.extend_from_slice(&rhs.0);` seems to be a bit slower. Not sure, why.
        for v in &rhs.0 {
            ret.push(*v);
        }
        DeltaProduct(ret)
    }
}

impl fmt::Display for DeltaProduct {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let element_strings: Vec<_> = self.0.iter().map(|d| d.to_string()).collect();
        write!(f, "{}", element_strings.join(" * "))
    }
}

impl VectorLike<Delta> for DeltaProduct {
    fn from(s: &[Delta]) -> Self { <Self as From<&[Delta]>>::from(s) }
    fn len(&self) -> usize { self.0.len() }
    fn is_empty(&self) -> bool { self.0.is_empty() }
    fn pop(&mut self) -> Option<Delta> { self.0.pop() }
    fn push(&mut self, value: Delta) { self.0.push(value) }
}


pub type DeltaExpr = Linear<DeltaProduct>;

#[allow(non_snake_case)]
pub fn D(a: i32, b: i32) -> DeltaExpr {
    DeltaExpr::single(DeltaProduct(smallvec![Delta::new(a, b)]))
}
