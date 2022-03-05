use std::collections::{HashMap, hash_map};
use std::fmt::{self, Debug, Display};
use std::hash::Hash;
use std::ops;
use trait_set::trait_set;


use i32 as Coeff;

trait_set! {
    // TODO: Use a trait alias when they are stable: https://github.com/rust-lang/rust/issues/41517.
    // Note: `Ord` is required only for printing.
    pub trait LinearMonom = Eq + Ord + Hash + Clone + Debug;
}

pub trait TensorProduct {
    fn tensor_product(&self, rhs: &Self) -> Self;
}

#[derive(Clone)]
pub struct Linear<MonomT: LinearMonom> {
    data: HashMap<MonomT, Coeff>,
}

impl<MonomT: LinearMonom> Linear<MonomT> {
    pub fn zero() -> Self { Self{ data: HashMap::new() } }
    pub fn single(monom: MonomT) -> Self {
        let mut ret = Self::zero();
        ret.add_to(monom, 1);
        ret
    }

    pub fn l1_norm(&self) -> Coeff {
        let mut norm: Coeff = 0;
        for (_, coeff) in (&self).into_iter() {
            norm += coeff.abs();
        }
        norm
    }

    // TODO: Support different destination type
    pub fn mapped<F: Fn(&MonomT) -> MonomT>(&self, f: F) -> Self {
        let mut ret = Self::zero();
        for (monom, coeff) in self.into_iter() {
            ret.add_to(f(monom), *coeff);
        }
        ret
    }
    // TODO: Support different destination type
    pub fn mapped_expanding<F: Fn(&MonomT) -> Self>(&self, f: F) -> Self {
        let mut ret = Self::zero();
        for (monom, coeff) in self.into_iter() {
            ret += f(monom) * *coeff;
        }
        ret
    }

    pub fn add_to(&mut self, monom: MonomT, v: Coeff) {
        match self.data.entry(monom) {
            hash_map::Entry::Occupied(occupied) => {
                let new_value = occupied.get() + v;
                if new_value != 0 {
                    *occupied.into_mut() = new_value;
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

impl<'a, MonomT: LinearMonom> IntoIterator for &'a Linear<MonomT> {
    type Item = (&'a MonomT, &'a Coeff);
    type IntoIter = hash_map::Iter<'a, MonomT, Coeff>;
    #[inline]
    fn into_iter(self) -> Self::IntoIter {
        self.data.iter()
    }
}
impl<MonomT: LinearMonom> IntoIterator for Linear<MonomT> {
    type Item = (MonomT, Coeff);
    type IntoIter = hash_map::IntoIter<MonomT, Coeff>;
    #[inline]
    fn into_iter(self) -> Self::IntoIter {
        self.data.into_iter()
    }
}

impl<MonomT: LinearMonom> ops::Neg for Linear<MonomT> {
    type Output = Self;
    fn neg(mut self) -> Self {
        for (_k, v) in &mut self.data {
            *v = -*v;
        }
        self
    }
}

impl<MonomT: LinearMonom> ops::AddAssign for Linear<MonomT> {
    fn add_assign(&mut self, rhs: Self) {
        for (k, v) in rhs.data {
            self.add_to(k, v);
        }
    }
}
impl<MonomT: LinearMonom> ops::SubAssign for Linear<MonomT> {
    fn sub_assign(&mut self, rhs: Self) {
        for (k, v) in rhs.data {
            self.add_to(k, -v);
        }
    }
}

impl<MonomT: LinearMonom> ops::Add for Linear<MonomT> {
    type Output = Self;
    fn add(mut self, rhs: Self) -> Self {
        self += rhs;
        self
    }
}
impl<MonomT: LinearMonom> ops::Sub for Linear<MonomT> {
    type Output = Self;
    fn sub(mut self, rhs: Self) -> Self {
        self -= rhs;
        self
    }
}

impl<MonomT: LinearMonom> ops::Mul<i32> for Linear<MonomT> {
    type Output = Self;
    fn mul(mut self, factor: i32) -> Self {
        for (_k, v) in &mut self.data {
            *v *= factor;
        }
        self
    }
}
impl<MonomT: LinearMonom> ops::Mul<Linear<MonomT>> for i32 {
    type Output = Linear<MonomT>;
    fn mul(self, expr: Linear<MonomT>) -> Linear<MonomT> {
        expr * self
    }
}

fn coeff_to_string(coeff: i32) -> String {
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

fn write_linear<MonomT: LinearMonom>(
    expr: &Linear<MonomT>,
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
