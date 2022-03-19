use std::collections::{HashMap, hash_map};
use std::fmt::{self, Debug, Display};
use std::hash::Hash;
use std::ops;
use trait_set::trait_set;

use crate::base::{VectorLike};


pub use i32 as Coeff;

trait_set! {
    // TODO: Use a trait alias when they are stable: https://github.com/rust-lang/rust/issues/41517.
    // Note: `Ord` is required only for printing.
    pub trait LinearMonom = Eq + Ord + Hash + Clone + Debug;
}

pub trait TensorProduct {
    fn tensor_product(&self, rhs: &Self) -> Self;
}

pub trait MonomVectorizable: LinearMonom {
    type Element: Ord + Clone;  // `Ord` required for Lyndon basis
    type AsVector: LinearMonom + VectorLike<Self::Element>;
    fn to_vector(self) -> Self::AsVector;
    fn from_vector(v: Self::AsVector) -> Self;
}

impl<T: Eq + Ord + Hash + Clone + Debug> MonomVectorizable for Vec<T> {
    type Element = T;
    type AsVector = Self;
    fn to_vector(self) -> Self { self }
    fn from_vector(v: Self) -> Self { v }
}


#[derive(PartialEq, Eq, Clone)]
pub struct BasicLinear<MonomT: LinearMonom> {
    data: HashMap<MonomT, Coeff>,
}

impl<MonomT: LinearMonom> BasicLinear<MonomT> {
    pub fn zero() -> Self { Self{ data: HashMap::new() } }
    pub fn single(monom: MonomT) -> Self {
        let mut ret = Self::zero();
        ret.add_to(monom, 1);
        ret
    }
    pub fn from_collection(collection: &[MonomT]) -> Self {
        let mut ret = Self::zero();
        for monom in collection {
            ret.add_to(monom.clone(), 1);
        }
        ret
    }

    #[inline]
    pub fn iter(&self) -> hash_map::Iter<'_, MonomT, Coeff> { self.data.iter() }

    pub fn is_zero(&self) -> bool { self.data.is_empty() }
    pub fn l1_norm(&self) -> Coeff {
        let mut norm: Coeff = 0;
        for (_, coeff) in (&self).iter() {
            norm += coeff.abs();
        }
        norm
    }

    pub fn div_int(&mut self, denominator: Coeff) {
        for (_k, v) in &mut self.data {
            assert!(*v % denominator == 0);
            *v /= denominator;
        }
    }

    #[inline]
    pub fn coeff_for(&self, monom: &MonomT) -> Coeff {
        *self.data.get(monom).unwrap_or(&0)
    }
    #[inline]
    pub fn add_to(&mut self, monom: MonomT, v: Coeff) {
        match self.data.entry(monom) {
            hash_map::Entry::Occupied(mut occupied) => {
                let new_value = occupied.get() + v;
                if new_value != 0 {
                    *occupied.get_mut() = new_value;
                } else {
                    occupied.remove();
                }
            }
            hash_map::Entry::Vacant(vacant) => {
                if v != 0 {
                    vacant.insert(v);
                }
            }
        }
    }
}

impl<MonomT: LinearMonom> IntoIterator for BasicLinear<MonomT> {
    type Item = (MonomT, Coeff);
    type IntoIter = hash_map::IntoIter<MonomT, Coeff>;
    #[inline]
    fn into_iter(self) -> Self::IntoIter {
        self.data.into_iter()
    }
}

impl<MonomT: LinearMonom> ops::Neg for BasicLinear<MonomT> {
    type Output = Self;
    fn neg(mut self) -> Self {
        for (_k, v) in &mut self.data {
            *v = -*v;
        }
        self
    }
}

impl<MonomT: LinearMonom> ops::AddAssign for BasicLinear<MonomT> {
    fn add_assign(&mut self, rhs: Self) {
        for (k, v) in rhs.data {
            self.add_to(k, v);
        }
    }
}
impl<MonomT: LinearMonom> ops::SubAssign for BasicLinear<MonomT> {
    fn sub_assign(&mut self, rhs: Self) {
        for (k, v) in rhs.data {
            self.add_to(k, -v);
        }
    }
}

impl<MonomT: LinearMonom> ops::Add for BasicLinear<MonomT> {
    type Output = Self;
    fn add(mut self, rhs: Self) -> Self {
        self += rhs;
        self
    }
}
impl<MonomT: LinearMonom> ops::Sub for BasicLinear<MonomT> {
    type Output = Self;
    fn sub(mut self, rhs: Self) -> Self {
        self -= rhs;
        self
    }
}

impl<MonomT: LinearMonom> ops::Mul<Coeff> for BasicLinear<MonomT> {
    type Output = Self;
    fn mul(mut self, factor: Coeff) -> Self {
        if factor == 0 {
            return Self::zero();
        }
        for (_k, v) in &mut self.data {
            *v *= factor;
        }
        self
    }
}
impl<MonomT: LinearMonom> ops::Mul<BasicLinear<MonomT>> for Coeff {
    type Output = BasicLinear<MonomT>;
    fn mul(self, expr: BasicLinear<MonomT>) -> BasicLinear<MonomT> {
        expr * self
    }
}

fn coeff_to_string(coeff: Coeff) -> String {
    if coeff == 1 {
        String::from(" +")
    } else if coeff == -1 {
        String::from(" -")
    } else if coeff == 0 {
        String::from(" 0")
    } else {
        // Use two separate `format` calls to prevent the plus sign from being discarded.
        format!("{:>2}", format!("{:+}", coeff))
    }
}

fn write_basic_linear<MonomT: LinearMonom>(
    expr: &BasicLinear<MonomT>,
    f: &mut fmt::Formatter<'_>,
    monom_printer: fn(&MonomT) -> String
) -> fmt::Result {
    let mut elements: Vec<_> = expr.data.iter().collect();
    elements.sort();
    for (monom, coeff) in elements.iter() {
        writeln!(f, "{} {}", coeff_to_string(**coeff), monom_printer(*monom))?;
    }
    Ok(())
}
impl<MonomT: LinearMonom + Display> Display for BasicLinear<MonomT> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write_basic_linear(self, f, |monom| monom.to_string())
    }
}
impl<MonomT: LinearMonom> Debug for BasicLinear<MonomT> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write_basic_linear(self, f, |monom| format!("{:?}", monom))
    }
}


#[derive(PartialEq, Eq, Clone)]
pub struct Linear<MonomT: LinearMonom> {
    main: BasicLinear<MonomT>,
    annotations: BasicLinear<String>,
}

impl<MonomT: LinearMonom> Linear<MonomT> {
    pub fn zero() -> Self {
        Self{ main: BasicLinear::zero(), annotations: BasicLinear::zero() }
    }
    pub fn single(monom: MonomT) -> Self {
        Self{ main: BasicLinear::single(monom), annotations: BasicLinear::zero() }
    }
    pub fn from_collection(collection: &[MonomT]) -> Self {
        Self{ main: BasicLinear::from_collection(collection), annotations: BasicLinear::zero() }
    }

    #[inline]
    pub fn iter(&self) -> hash_map::Iter<'_, MonomT, Coeff> { self.main.iter() }

    pub fn is_zero(&self) -> bool { self.main.is_zero() }
    pub fn is_blank(&self) -> bool { self.main.is_zero() && self.annotations.is_zero() }
    pub fn l1_norm(&self) -> Coeff { self.main.l1_norm() }

    // TODO: Copy annotations in `map` and `map_expanding`
    pub fn map<NewMonomT, F>(self, f: F) -> Linear<NewMonomT>
    where
        NewMonomT: LinearMonom,
        F: Fn(MonomT) -> NewMonomT,
    {
        let mut ret = Linear::<NewMonomT>::zero();
        for (monom, coeff) in self.into_iter() {
            ret.add_to(f(monom), coeff);
        }
        ret
    }
    pub fn map_expanding<NewMonomT, F>(self, f: F) -> Linear<NewMonomT>
    where
        NewMonomT: LinearMonom,
        F: Fn(MonomT) -> Linear<NewMonomT>,
    {
        let mut ret = Linear::<NewMonomT>::zero();
        for (monom, coeff) in self.into_iter() {
            ret += f(monom) * coeff;
        }
        ret
    }

    pub fn div_int(&mut self, denominator: Coeff) { self.main.div_int(denominator) }

    #[inline]
    pub fn coeff_for(&self, monom: &MonomT) -> Coeff { self.main.coeff_for(monom) }
    #[inline]
    pub fn add_to(&mut self, monom: MonomT, v: Coeff) { self.main.add_to(monom, v) }

    pub fn annotate(mut self, annotation: String) -> Self {
        self.annotations.add_to(annotation, 1);
        self
    }
}

impl<MonomT: LinearMonom> IntoIterator for Linear<MonomT> {
    type Item = (MonomT, Coeff);
    type IntoIter = hash_map::IntoIter<MonomT, Coeff>;
    #[inline]
    fn into_iter(self) -> Self::IntoIter { self.main.into_iter() }
}

impl<MonomT: LinearMonom> ops::Neg for Linear<MonomT> {
    type Output = Self;
    fn neg(self) -> Self {
        Self{ main: -self.main, annotations: -self.annotations }
    }
}

impl<MonomT: LinearMonom> ops::AddAssign for Linear<MonomT> {
    fn add_assign(&mut self, rhs: Self) {
        self.main += rhs.main;
        self.annotations += rhs.annotations;
    }
}
impl<MonomT: LinearMonom> ops::SubAssign for Linear<MonomT> {
    fn sub_assign(&mut self, rhs: Self) {
        self.main -= rhs.main;
        self.annotations -= rhs.annotations;
    }
}

impl<MonomT: LinearMonom> ops::Add for Linear<MonomT> {
    type Output = Self;
    fn add(self, rhs: Self) -> Self {
        Self{ main: self.main + rhs.main, annotations: self.annotations + rhs.annotations }
    }
}
impl<MonomT: LinearMonom> ops::Sub for Linear<MonomT> {
    type Output = Self;
    fn sub(self, rhs: Self) -> Self {
        Self{ main: self.main - rhs.main, annotations: self.annotations - rhs.annotations }
    }
}

impl<MonomT: LinearMonom> ops::Mul<Coeff> for Linear<MonomT> {
    type Output = Self;
    fn mul(self, factor: Coeff) -> Self {
        Self{ main: self.main * factor, annotations: self.annotations * factor }
    }
}
impl<MonomT: LinearMonom> ops::Mul<Linear<MonomT>> for Coeff {
    type Output = Linear<MonomT>;
    fn mul(self, expr: Linear<MonomT>) -> Linear<MonomT> {
        expr * self
    }
}

fn write_linear<MonomT: LinearMonom>(
    expr: &Linear<MonomT>,
    f: &mut fmt::Formatter<'_>,
    monom_printer: fn(&MonomT) -> String
) -> fmt::Result {
    write_basic_linear(&expr.main, f, monom_printer)?;
    if !expr.annotations.is_zero() {
        writeln!(f, "===")?;
        write_basic_linear(&expr.annotations, f, |monom| monom.clone())?;
    }
    Ok(())
}
impl<MonomT: LinearMonom + Display> Display for Linear<MonomT> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write_linear(self, f, |monom| monom.to_string())
    }
}
impl<MonomT: LinearMonom> Debug for Linear<MonomT> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write_linear(self, f, |monom| format!("{:?}", monom))
    }
}
