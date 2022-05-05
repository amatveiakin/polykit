#[repr(C)]
pub struct Triplet {
  pub row: ::std::os::raw::c_int,
  pub col: ::std::os::raw::c_int,
  pub value: ::std::os::raw::c_int,
};

extern "C" {
    pub fn linbox_matrix_rank(
        num_rows: ::std::os::raw::c_int,
        num_cols: ::std::os::raw::c_int,
        num_triplets: ::std::os::raw::c_int,
        triplets: *Triplet
    ) -> ::std::os::raw::c_int;
}
