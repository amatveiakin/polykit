pub mod base;
pub mod math;
pub mod polylog;


fn main() {
    // let mut timer = base::Timer::new();
    // let mut checksum = 0;
    // for _ in 0..1000 {
    //     checksum += polylog::QLi4((1,2,3,4,5,6,7,8)).l1_norm();
    // }
    // println!("Checksum: {}", checksum);
    // timer.finish("QLi");

    println!("{:?}", math::shuffle_product(vec![1,2], vec![1,3]));
    println!("{:?}", math::shuffle_product_multi(&[vec![1,2], vec![3], vec![4]]));
}
