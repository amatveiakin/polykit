extern crate num;

use num::{Integer, NumCast};


pub fn sort_two<T: Ord>((a, b): (T, T)) -> (T, T) {
    if a < b { (a, b) } else { (b, a) }
}

pub fn neg_one_pow<T: Integer + NumCast>(x: T) -> i32 {
    if x % NumCast::from(2).unwrap() == T::one() { -1 } else { 1 }
}
