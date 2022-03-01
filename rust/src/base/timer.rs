use std::time::Instant;


pub struct Timer {
    start: Instant,
}

impl Timer {
    pub fn new() -> Self { Timer{ start: Instant::now(), } }

    pub fn finish(&mut self, operation: &str) {
        let now = Instant::now();
        let duration = now.duration_since(self.start);
        println!("Timer: {} took {:.3} seconds", operation, duration.as_secs_f64());
        self.start = now;
    }
}
