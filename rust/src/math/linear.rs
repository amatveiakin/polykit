use std::collections::{HashMap, hash_map};
use std::fmt::{self, Debug, Display};
use std::hash::Hash;
use std::ops;
use trait_set::trait_set;


use i32 as Coeff;

trait_set! {
    // TODO: Rename `Object` to something more meaningful here and in other places (`Monom`?)
    // TODO: Use a trait alias when they are stable: https://github.com/rust-lang/rust/issues/41517.
    // Note: `Ord` is required only for printing.
    pub trait LinearObject = Eq + Ord + Hash + Clone + Debug;
}

pub trait TensorProduct {
    fn tensor_product(&self, rhs: &Self) -> Self;
}

#[derive(Clone)]
pub struct Linear<ObjectT: LinearObject> {
    data: HashMap<ObjectT, Coeff>,
}

impl<ObjectT: LinearObject> Linear<ObjectT> {
    pub fn zero() -> Self { Self{ data: HashMap::new() } }
    pub fn single(object: ObjectT) -> Self {
        let mut ret = Self::zero();
        ret.add_to(object, 1);
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
    pub fn mapped<F: Fn(&ObjectT) -> ObjectT>(&self, f: F) -> Self {
        let mut ret = Self::zero();
        for (obj, coeff) in self.into_iter() {
            ret.add_to(f(obj), *coeff);
        }
        ret
    }
    // TODO: Support different destination type
    pub fn mapped_expanding<F: Fn(&ObjectT) -> Self>(&self, f: F) -> Self {
        let mut ret = Self::zero();
        for (obj, coeff) in self.into_iter() {
            ret += f(obj) * *coeff;
        }
        ret
    }

    pub fn add_to(&mut self, object: ObjectT, v: Coeff) {
        match self.data.entry(object) {
            hash_map::Entry::Occupied(occupied) => {
                let new_value = occupied.get() + v;
                if new_value != 0 {
                    *occupied.into_mut() += new_value;
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

impl<'a, ObjectT: LinearObject> IntoIterator for &'a Linear<ObjectT> {
    type Item = (&'a ObjectT, &'a Coeff);
    type IntoIter = hash_map::Iter<'a, ObjectT, Coeff>;
    #[inline]
    fn into_iter(self) -> Self::IntoIter {
        self.data.iter()
    }
}
impl<ObjectT: LinearObject> IntoIterator for Linear<ObjectT> {
    type Item = (ObjectT, Coeff);
    type IntoIter = hash_map::IntoIter<ObjectT, Coeff>;
    #[inline]
    fn into_iter(self) -> Self::IntoIter {
        self.data.into_iter()
    }
}

impl<ObjectT: LinearObject> ops::Neg for Linear<ObjectT> {
    type Output = Self;
    fn neg(mut self) -> Self {
        for (_k, v) in &mut self.data {
            *v = -*v;
        }
        self
    }
}

impl<ObjectT: LinearObject> ops::AddAssign for Linear<ObjectT> {
    fn add_assign(&mut self, rhs: Self) {
        for (k, v) in rhs.data {
            self.add_to(k, v);
        }
    }
}
impl<ObjectT: LinearObject> ops::SubAssign for Linear<ObjectT> {
    fn sub_assign(&mut self, rhs: Self) {
        for (k, v) in rhs.data {
            self.add_to(k, -v);
        }
    }
}

impl<ObjectT: LinearObject> ops::Add for Linear<ObjectT> {
    type Output = Self;
    fn add(mut self, rhs: Self) -> Self {
        self += rhs;
        self
    }
}
impl<ObjectT: LinearObject> ops::Sub for Linear<ObjectT> {
    type Output = Self;
    fn sub(mut self, rhs: Self) -> Self {
        self -= rhs;
        self
    }
}

impl<ObjectT: LinearObject> ops::Mul<i32> for Linear<ObjectT> {
    type Output = Self;
    fn mul(mut self, factor: i32) -> Self {
        for (_k, v) in &mut self.data {
            *v *= factor;
        }
        self
    }
}
impl<ObjectT: LinearObject> ops::Mul<Linear<ObjectT>> for i32 {
    type Output = Linear<ObjectT>;
    fn mul(self, expr: Linear<ObjectT>) -> Linear<ObjectT> {
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

fn write_linear<ObjectT: LinearObject>(
    expr: &Linear<ObjectT>,
    f: &mut fmt::Formatter<'_>,
    obj_printer: fn(&ObjectT) -> String
) -> fmt::Result {
    let mut elements: Vec<_> = expr.data.iter().collect();
    elements.sort();
    for (obj, coeff) in elements.iter() {
        writeln!(f, "{} {}", coeff_to_string(**coeff), obj_printer(*obj))?;
    }
    Ok(())
}
impl<ObjectT: LinearObject + Display> Display for Linear<ObjectT> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write_linear(self, f, |obj| obj.to_string())
    }
}
impl<ObjectT: LinearObject> Debug for Linear<ObjectT> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write_linear(self, f, |obj| format!("{:?}", obj))
    }
}
