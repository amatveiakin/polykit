use rayon::prelude::*;

use crate::math::{LinearMonom, Linear, ExprMatrixBuilder, matrix_rank};


pub fn space_rank<A, B, F>(space: &[Linear<A>], prepare: F) -> i32
where
    A: LinearMonom + Sync,
    B: LinearMonom + Sync + Send,
    F: Fn(Linear<A>) -> Linear<B> + Sync + Send,
{
    // TODO: Check that the space homogeneous.
    let mut matrix_builder = ExprMatrixBuilder::new();
    // TODO: Why doesn't `into_par_iter` work (even is space is a Vec)?
    let space_prepared: Vec<_> = space.par_iter().map(|expr| prepare(expr.clone())).collect();
    for expr in space_prepared {
        matrix_builder.add_expr(expr);
    }
    matrix_rank(&matrix_builder.make_matrix())
}
