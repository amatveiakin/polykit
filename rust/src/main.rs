pub mod base;
pub mod math;
pub mod polylog;


fn main() {
    let mut timer = base::Timer::new();
    let mut checksum = 0;
    for _ in 0..1000 {
        checksum += polylog::QLi(4, &[1,2,3,4,5,6,7,8]).l1_norm();
    }
    println!("Checksum: {}", checksum);
    timer.finish("QLi");
}
