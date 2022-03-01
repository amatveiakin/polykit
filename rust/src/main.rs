pub mod base;
pub mod math;
pub mod polylog;


fn main() {
    println!("{}", polylog::QLi(2, &[1,2,3,4,5,6]));
    println!("{}", math::Linear::single("test") * 200);
}
