#![allow(unused_imports)]

pub mod base;
pub mod math;
pub mod polylog;

use std::convert;

use base::*;
use math::*;
use math_format::*;
use math_format_macro::math_format;
use polylog::*;


fn main() {
    // let triplets = vec![Triplet{row:0, col:0, value:1}];
    // let rank;
    // unsafe {
    //     rank = linbox_matrix_rank(1, 1, triplets.len().try_into().unwrap(), triplets.as_ptr());
    // }
    // println!("rank = {}", rank);

    // let mut timer = Timer::new();
    // let space = CB_naive_via_QLi_fours(5, &[1,2,3,4,5,6,7,8]);
    // timer.finish("space");
    // let rank = space_rank(&space, to_lyndon_basis);
    // timer.finish("rank");
    // println!("rank = {}", rank);


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

    // println!("Rust");
    // for _ in 0..5 {
    //     let mut timer = base::Timer::new();
    //     let expr = QLi5!(1,2,3,4,5,6,7,8,9,10);
    //     timer.finish("expr");
    //     let lyndon = to_lyndon_basis(expr);
    //     timer.finish("lyndon");
    //     println!("checksum = {}", lyndon.l1_norm());
    // }

    let mut timer = base::Timer::new();
    let mut checksum: Coeff = 0;
    let expr = delta_expr_to_gamma_expr(QLi6!(1,2,3,4,5,6,7,8));
    timer.finish("expr");
    for _ in 0..5 {
        let mut timer = base::Timer::new();
        let d_expr = gamma_expr_to_delta_expr(expr.clone());
        timer.finish("convert");
        checksum += d_expr.l1_norm();
    }
    println!("checksum = {}", checksum);

    // println!("{}", QLi2!(1,2,3,4));
    // println!("{}", QLi2!(1,2,3,Inf));
    // println!("{}", to_lyndon_basis(QLi2!(1,2,3,4)));
    // println!("{}", pullback(QLi2!(1,2,3,4), &[5,6]));
    // println!("{}", GrQLi2!(1,2,3,4; 5,6));
    // println!("{}", GrQLi2!(1,2,3,4; ));
}
