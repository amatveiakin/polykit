use crate::base::VectorLike;
use crate::math::{Linear, LinearMonom};


// TODO: Unroll
pub fn shuffle_product<T, MonomT>(u: MonomT, v: MonomT) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    if u.is_empty() && v.is_empty() {
        return Linear::<MonomT>::zero();
    }
    if u.is_empty() {
        return Linear::<MonomT>::single(v);
    }
    if v.is_empty() {
        return Linear::<MonomT>::single(u);
    }
    let mut u_trunc = u.clone();
    let mut v_trunc = v.clone();
    let a: T = u_trunc.pop().unwrap();
    let b: T = v_trunc.pop().unwrap();
    let mut ret = Linear::<MonomT>::zero();
    for (mut w, coeff) in shuffle_product(u, v_trunc).into_iter() {
        w.push(b.clone());
        ret.add_to(w, coeff);
    }
    for (mut w, coeff) in shuffle_product(u_trunc, v).into_iter() {
        w.push(a.clone());
        ret.add_to(w, coeff);
    }
    return ret;
}

// TODO: Unroll
pub fn shuffle_product_multi<T, MonomT>(words: &[MonomT]) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    match words.len() {
        0 => Linear::<MonomT>::zero(),
        1 => Linear::<MonomT>::single(words[0].clone()),
        2 => shuffle_product(words[0].clone(), words[1].clone()),
        _ => {
            let w_tail = words.last().unwrap();
            let shuffle_product_head = shuffle_product_multi(&words[0 .. words.len()-1]);
            shuffle_product_head.mapped_expanding(|w_head| {
                shuffle_product(w_head.clone(), w_tail.clone())
            })
        },
    }
}
