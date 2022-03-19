use std::cmp::Ordering;
use std::fmt;
use std::ops;

use smallvec::{SmallVec, smallvec};

use crate::base::{VectorLike};
use crate::math::{Linear, MonomVectorizable, TensorProduct};


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
        let (smaller, larger) = if a < b { (a, b) } else { (b, a) };
        Delta{ a: smaller.try_into().unwrap(), b: larger.try_into().unwrap() }
    }

    pub fn a(&self) -> i32 { self.a.into() }
    pub fn b(&self) -> i32 { self.b.into() }

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
        DeltaProduct(SmallVec::<[Delta; 8]>::from(slice))
    }
}

impl ops::Deref for DeltaProduct {
    type Target = [Delta];
    fn deref(&self) -> &Self::Target { &self.0 }
}

impl IntoIterator for DeltaProduct {
    type Item = Delta;
    type IntoIter = smallvec::IntoIter<[Delta; 8]>;
    fn into_iter(self) -> Self::IntoIter { self.0.into_iter() }
}
impl FromIterator<Delta> for DeltaProduct {
    fn from_iter<T: IntoIterator<Item = Delta>>(iter: T) -> Self {
        DeltaProduct(SmallVec::<[Delta; 8]>::from_iter(iter))
    }
}

impl MonomVectorizable for DeltaProduct {
    type Element = Delta;
    type AsVector = Self;
    fn to_vector(self) -> Self { self }
    fn from_vector(v: Self) -> Self { v }
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
    fn pop(&mut self) -> Option<Delta> { self.0.pop() }
    fn push(&mut self, value: Delta) { self.0.push(value) }
}


pub type DeltaExpr = Linear<DeltaProduct>;

#[allow(non_snake_case)]
pub fn D(a: i32, b: i32) -> DeltaExpr {
    DeltaExpr::single(DeltaProduct(smallvec![Delta::new(a, b)]))
}
