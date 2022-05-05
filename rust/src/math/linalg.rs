use std::cmp;
use std::os;


pub struct Matrix {
    triplets: Vec<Triplet>,
    num_rows: u32,
    num_cols: u32,
}

impl Matrix {
    pub fn new() -> Self {
        Matrix {
            triplets: vec![],
            num_rows: 0,
            num_cols: 0,
        }
    }

    pub fn set(&mut self, row: u32, col: u32, value: i32) {
        self.triplets.push(Triplet{
            row: row.try_into().unwrap(),
            col: col.try_into().unwrap(),
            value: value.try_into().unwrap()
        });
        self.num_rows = cmp::max(self.num_rows, row + 1);
        self.num_cols = cmp::max(self.num_cols, col + 1);
    }
}

pub fn matrix_rank(matrix: &Matrix) -> i32 {
    unsafe {
        linbox_matrix_rank(
            matrix.num_rows.try_into().unwrap(),
            matrix.num_cols.try_into().unwrap(),
            matrix.triplets.len().try_into().unwrap(),
            matrix.triplets.as_ptr()
        )
    }
}


#[repr(C)]
struct Triplet {
  row: os::raw::c_int,
  col: os::raw::c_int,
  value: os::raw::c_int,
}

extern "C" {
    fn linbox_matrix_rank(
        num_rows: os::raw::c_int,
        num_cols: os::raw::c_int,
        num_triplets: os::raw::c_int,
        triplets: *const Triplet
    ) -> os::raw::c_int;
}
