#![allow(unused_imports)]

pub mod base;
pub mod math;
pub mod polylog;

use math::*;
use math_format::*;
use math_format_macro::math_format;


fn main() {
    // let triplets = vec![Triplet{row:0, col:0, value:1}];
    // let rank;
    // unsafe {
    //     rank = linbox_matrix_rank(1, 1, triplets.len().try_into().unwrap(), triplets.as_ptr());
    // }
    // println!("rank = {}", rank);

    let mut m = Matrix::new();
    m.set(0, 0, 1);
    m.set(0, 1, 2);
    m.set(1, 0, 3);
    m.set(1, 1, 6);
    let rank = matrix_rank(&m);
    println!("rank = {}", rank);


    // scoped_formatting!(
    //     encoder: Encoder::Unicode,
    // );
    // println!("{}", math_format!(r"\frac1{<>^<>}", 2+3, 4+5));
    // println!("{}", math_format!(r"\frac1{<>^9}", 2+3));
    // println!("{}", math_format!(r"a\   \  b\inf"));
    // println!("{}", math_format!(r"\yellow{-7\op{QLi}_<,>(x_1, x_2)}", vec![1,33]));
    // println!("{}", mfmt::concat(vec![mfmt::lit("a"), mfmt::sub("2")]).render());

    // let mut timer = base::Timer::new();
    // let mut checksum = 0;
    // for _ in 0..100 {
    //     checksum += to_lyndon_basis(QLi4!(1,2,3,4,5,6,7,8)).l1_norm();
    // }
    // println!("Checksum: {}", checksum);
    // timer.finish("QLi");

    // println!("{}", QLi2!(1,2,3,4));
    // println!("{}", QLi2!(1,2,3,Inf));
    // println!("{}", to_lyndon_basis(QLi2!(1,2,3,4)));
    // println!("{}", pullback(QLi2!(1,2,3,4), &[5,6]));
    // println!("{}", GrQLi2!(1,2,3,4; 5,6));
    // println!("{}", GrQLi2!(1,2,3,4; ));
}
