extern crate num;

use num::{Integer, NumCast};


pub fn neg_one_pow<T: Integer + NumCast>(x: T) -> i32 {
    if x % NumCast::from(2).unwrap() == T::one() { -1 } else { 1 }
}
