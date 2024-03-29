use std::fmt;
use std::ops;

use bitvec::array::{BitArray};
use itertools::Itertools;
use smallvec::{SmallVec, smallvec};

use crate::{vector_like_impl};
use crate::base::{VectorLike};
use crate::math::{Linear, MonomVectorizable, TensorProduct, Delta, DeltaExpr};


type Indices = BitArray<[u16; 1]>;
const BITSET_OFFSET: i32 = 1;

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Gamma {
    indices: Indices,
}

fn variables_to_bitset(variables: &[i32]) -> Option<Indices> {
    let mut indices = Indices::ZERO;
    for v in variables {
        let idx = (*v - BITSET_OFFSET).try_into().unwrap();
        if indices[idx] {
            return None;
        }
        indices.set(idx, true);
    }
    Some(indices)
}

impl Gamma {
    pub fn new() -> Self { Gamma{ indices: Indices::ZERO } }
    pub fn from_bitset(indices: Indices) -> Self { Gamma{ indices: indices } }
    pub fn from_vector(variables: &[i32]) -> Self {
        match variables_to_bitset(variables) {
            None => Gamma::new(),
            Some(indices) => Gamma::from_bitset(indices),
        }
    }

    pub fn is_nil(&self) -> bool { self.indices.not_any() }
    pub fn index_bitset(&self) -> &Indices { &self.indices }
    pub fn index_iter(&self) -> impl Iterator<Item = i32> + '_ {
        self.indices.iter_ones().map(|idx| (i32::try_from(idx).unwrap() + BITSET_OFFSET))
    }
}

impl fmt::Display for Gamma {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "({})", self.index_iter().map(|d| d.to_string()).join(","))
    }
}


// TODO: All code for GammaProduct is copied verbatim from DeltaProduct.
//   Can the amount of copy-paste be reduced?
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct GammaProduct(pub SmallVec<[Gamma; 8]>);

impl GammaProduct {
    pub fn new() -> Self {
        GammaProduct(SmallVec::<[Gamma; 8]>::new())
    }
}

impl ops::Deref for GammaProduct {
    type Target = [Gamma];
    fn deref(&self) -> &Self::Target { &self.0 }
}
impl From<&[Gamma]> for GammaProduct {
    fn from(slice: &[Gamma]) -> Self {
        GammaProduct(SmallVec::<[Gamma; 8]>::from(slice))
    }
}
impl IntoIterator for GammaProduct {
    type Item = Gamma;
    type IntoIter = smallvec::IntoIter<[Gamma; 8]>;
    fn into_iter(self) -> Self::IntoIter { self.0.into_iter() }
}
impl FromIterator<Gamma> for GammaProduct {
    fn from_iter<T: IntoIterator<Item = Gamma>>(iter: T) -> Self {
        GammaProduct(SmallVec::<[Gamma; 8]>::from_iter(iter))
    }
}

impl MonomVectorizable for GammaProduct {
    type Element = Gamma;
    type AsVector = Self;
    fn to_vector(self) -> Self { self }
    fn from_vector(v: Self) -> Self { v }
}

impl TensorProduct for GammaProduct {
    fn tensor_product(&self, rhs: &GammaProduct) -> GammaProduct {
        let mut ret = self.0.clone();
        for v in &rhs.0 {
            ret.push(*v);
        }
        GammaProduct(ret)
    }
}

impl fmt::Display for GammaProduct {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.0.iter().map(|d| d.to_string()).join(" * "))
    }
}

impl VectorLike<Gamma> for GammaProduct {
    vector_like_impl!(Self, self => self.0, element_type = Gamma);
}


pub type GammaExpr = Linear<GammaProduct>;

pub trait ConvertibleToGammaExpr {
    fn to_gamma_expr(self) -> GammaExpr;
}
impl ConvertibleToGammaExpr for GammaExpr {
    fn to_gamma_expr(self) -> GammaExpr { self }
}
impl ConvertibleToGammaExpr for DeltaExpr {
    fn to_gamma_expr(self) -> GammaExpr { delta_expr_to_gamma_expr(self) }
}


#[allow(non_snake_case)]
pub fn G(variables: &[i32]) -> GammaExpr {
    let g = Gamma::from_vector(variables);
    if g.is_nil() {
        GammaExpr::zero()
    } else {
        GammaExpr::single(GammaProduct(smallvec![g]))
    }
}


pub fn delta_expr_to_gamma_expr(expr: DeltaExpr) -> GammaExpr {
    expr.map(|monom| {
        monom.iter().map(|d| Gamma::from_vector(&[d.a(), d.b()])).collect()
    })
}

pub fn gamma_expr_to_delta_expr(expr: GammaExpr) -> DeltaExpr {
    expr.map(|monom| {
        monom.iter().map(|g| {
            if let Some((a, b)) = g.index_iter().collect_tuple() {
                Delta::new(a, b)
            } else {
                panic!("Delta expression requires dimension two, found: {:?}", g.index_iter().collect_vec());
            }
        }).collect()
    })
}

pub fn pullback<E: ConvertibleToGammaExpr>(expr: E, bonus_points: &[i32]) -> GammaExpr {
    let g_expr = expr.to_gamma_expr();
    if bonus_points.is_empty() {
        return g_expr;
    }
    let bonus_points_bitset = match variables_to_bitset(bonus_points) {
        None => { return GammaExpr::zero(); },
        Some(v) => v,
    };
    g_expr.map_expanding(|monom| {
        let mut is_zero = false;
        let new_monom = monom.iter().map(|g| {
            if (*g.index_bitset() & bonus_points_bitset).any() {
                is_zero = true;
            }
            Gamma::from_bitset(*g.index_bitset() | bonus_points_bitset)
        }).collect();
        if is_zero { GammaExpr::zero() } else { GammaExpr::single(new_monom) }
    }).map_annotations(|annotation| {
        format!("pb({}, {})", annotation, bonus_points.iter().sorted().join(","))
    })
}
