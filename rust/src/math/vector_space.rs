use crate::math::{LinearMonom, Linear, ExprMatrixBuilder, matrix_rank};


pub fn space_rank<A, B, F>(space: Vec<Linear<A>>, prepare: F) -> i32
where
    A: LinearMonom,
    B: LinearMonom,
    F: Fn(Linear<A>) -> Linear<B>,
{
    // TODO: Check that the space homogeneous.
    // TODO: Parallelize.
    let mut matrix_builder = ExprMatrixBuilder::new();
    for expr in space.into_iter() {
        matrix_builder.add_expr(prepare(expr));
    }
    matrix_rank(&matrix_builder.make_matrix())
}
