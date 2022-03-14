use crate::math::{Linear, LinearMonom, TensorProduct};


pub fn tensor_product<MonomT>(lhs: Linear<MonomT>, rhs: Linear<MonomT>) -> Linear<MonomT>
where
    MonomT: LinearMonom + TensorProduct,
{
    let mut ret = Linear::<MonomT>::zero();
    for (l, l_coeff) in (&lhs).iter() {
        for (r, r_coeff) in (&rhs).iter() {
            ret.add_to(l.tensor_product(r), l_coeff * r_coeff);
        }
    }
    ret
}
