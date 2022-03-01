use crate::math::{Linear, LinearObject, TensorProduct};


pub fn tensor_product<ObjectT>(lhs: Linear<ObjectT>, rhs: Linear<ObjectT>) -> Linear<ObjectT>
where
    ObjectT: LinearObject + TensorProduct,
{
    let mut ret = Linear::<ObjectT>::new();
    for (l_obj, l_coeff) in (&lhs).into_iter() {
        for (r_obj, r_coeff) in (&rhs).into_iter() {
            ret.add_to(l_obj.tensor_product(r_obj), l_coeff * r_coeff);
        }
    }
    ret
}
