pub mod base;
pub mod math;
pub mod polylog;

use math::*;


fn main() {
    // let mut timer = base::Timer::new();
    // let mut checksum = 0;
    // for _ in 0..100 {
    //     checksum += to_lyndon_basis(QLi4!(1,2,3,4,5,6,7,8)).l1_norm();
    // }
    // println!("Checksum: {}", checksum);
    // timer.finish("QLi");

    println!("{}", QLi2!(1,2,3,4));
    println!("{}", QLi2!(1,2,3,Inf));
    println!("{}", to_lyndon_basis(QLi2!(1,2,3,4)));
    println!("{}", pullback(QLi2!(1,2,3,4), &[5,6]));
    println!("{}", GrQLi2!(1,2,3,4; 5,6));
    println!("{}", GrQLi2!(1,2,3,4; ));
}
