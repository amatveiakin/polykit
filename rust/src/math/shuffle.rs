use crate::base::{VectorLike, vec_like};
use crate::math::{Linear, LinearMonom};


pub fn shuffle_product<T, MonomT>(u: &MonomT, v: &MonomT) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    if u.is_empty() && v.is_empty() {
        return Linear::<MonomT>::zero();
    }
    if u.is_empty() {
        return Linear::<MonomT>::single(v.clone());
    }
    if v.is_empty() {
        return Linear::<MonomT>::single(u.clone());
    }
    if let Some(ret) = shuffle_product_unrolled(u, v) {
        return ret;
    }
    let mut u_trunc = u.clone();
    let mut v_trunc = v.clone();
    let a: T = u_trunc.pop().unwrap();
    let b: T = v_trunc.pop().unwrap();
    let mut ret = Linear::<MonomT>::zero();
    for (mut w, coeff) in shuffle_product(u, &v_trunc).into_iter() {
        w.push(b.clone());
        ret.add_to(w, coeff);
    }
    for (mut w, coeff) in shuffle_product(&u_trunc, v).into_iter() {
        w.push(a.clone());
        ret.add_to(w, coeff);
    }
    return ret;
}

pub fn shuffle_product_multi<T, MonomT>(words: &[MonomT]) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    match words.len() {
        0 => Linear::<MonomT>::zero(),
        1 => Linear::<MonomT>::single(words[0].clone()),
        2 => shuffle_product(&words[0], &words[1]),
        _ => {
            let w_tail = words.last().unwrap();
            let shuffle_product_head = shuffle_product_multi(&words[0 .. words.len()-1]);
            shuffle_product_head.map_expanding(|w_head| {
                shuffle_product(&w_head, w_tail)
            })
        },
    }
}


// Speed up shuffle product.
// Improvement potential: Generate via Rust declarative macro (it would then be possible
//   to specify max unrolling weight in as compilation param).
fn shuffle_product_unrolled<T, MonomT>(u: &MonomT, v: &MonomT) -> Option<Linear<MonomT>>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    match (u.len(), v.len()) {
        (1, 1) => Some(shuffle_product_unrolled_1_1(u, v)),
        (1, 2) => Some(shuffle_product_unrolled_1_2(u, v)),
        (1, 3) => Some(shuffle_product_unrolled_1_3(u, v)),
        (1, 4) => Some(shuffle_product_unrolled_1_4(u, v)),
        (1, 5) => Some(shuffle_product_unrolled_1_5(u, v)),
        (2, 1) => Some(shuffle_product_unrolled_1_2(v, u)),
        (2, 2) => Some(shuffle_product_unrolled_2_2(u, v)),
        (2, 3) => Some(shuffle_product_unrolled_2_3(u, v)),
        (2, 4) => Some(shuffle_product_unrolled_2_4(u, v)),
        (3, 1) => Some(shuffle_product_unrolled_1_3(v, u)),
        (3, 2) => Some(shuffle_product_unrolled_2_3(v, u)),
        (3, 3) => Some(shuffle_product_unrolled_3_3(u, v)),
        (4, 1) => Some(shuffle_product_unrolled_1_4(v, u)),
        (4, 2) => Some(shuffle_product_unrolled_2_4(v, u)),
        (5, 1) => Some(shuffle_product_unrolled_1_5(v, u)),
        (_, _) => None,
    }
}

macro_rules! vec_like_cloned {
    ($type:ty; $($x:expr),+ $(,)?) => {
        vec_like!( $type; $( ($x).clone(), )* )
    };
}

fn shuffle_product_unrolled_1_1<T, MonomT>(u: &MonomT, v: &MonomT) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    let mut ret = Linear::<MonomT>::zero();
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0]], 1);
    ret
}

fn shuffle_product_unrolled_1_2<T, MonomT>(u: &MonomT, v: &MonomT) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    let mut ret = Linear::<MonomT>::zero();
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0]], 1);
    ret
}

fn shuffle_product_unrolled_1_3<T, MonomT>(u: &MonomT, v: &MonomT) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    let mut ret = Linear::<MonomT>::zero();
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], v[2], u[0]], 1);
    ret
}

fn shuffle_product_unrolled_1_4<T, MonomT>(u: &MonomT, v: &MonomT) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    let mut ret = Linear::<MonomT>::zero();
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1], v[2], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1], v[2], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0], v[2], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], v[2], u[0], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], v[2], v[3], u[0]], 1);
    ret
}

fn shuffle_product_unrolled_1_5<T, MonomT>(u: &MonomT, v: &MonomT) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    let mut ret = Linear::<MonomT>::zero();
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1], v[2], v[3], v[4]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1], v[2], v[3], v[4]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0], v[2], v[3], v[4]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], v[2], u[0], v[3], v[4]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], v[2], v[3], u[0], v[4]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], v[2], v[3], v[4], u[0]], 1);
    ret
}

fn shuffle_product_unrolled_2_2<T, MonomT>(u: &MonomT, v: &MonomT) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    let mut ret = Linear::<MonomT>::zero();
    ret.add_to(vec_like_cloned![MonomT; u[0], u[1], v[0], v[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], u[1], v[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1], u[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], u[1], v[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1], u[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0], u[1]], 1);
    ret
}

fn shuffle_product_unrolled_2_3<T, MonomT>(u: &MonomT, v: &MonomT) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    let mut ret = Linear::<MonomT>::zero();
    ret.add_to(vec_like_cloned![MonomT; u[0], u[1], v[0], v[1], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], u[1], v[1], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1], u[1], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1], v[2], u[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], u[1], v[1], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1], u[1], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1], v[2], u[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0], u[1], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0], v[2], u[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], v[2], u[0], u[1]], 1);
    ret
}

fn shuffle_product_unrolled_2_4<T, MonomT>(u: &MonomT, v: &MonomT) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    let mut ret = Linear::<MonomT>::zero();
    ret.add_to(vec_like_cloned![MonomT; u[0], u[1], v[0], v[1], v[2], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], u[1], v[1], v[2], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1], u[1], v[2], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1], v[2], u[1], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1], v[2], v[3], u[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], u[1], v[1], v[2], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1], u[1], v[2], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1], v[2], u[1], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1], v[2], v[3], u[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0], u[1], v[2], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0], v[2], u[1], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0], v[2], v[3], u[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], v[2], u[0], u[1], v[3]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], v[2], u[0], v[3], u[1]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], v[2], v[3], u[0], u[1]], 1);
    ret
}

fn shuffle_product_unrolled_3_3<T, MonomT>(u: &MonomT, v: &MonomT) -> Linear<MonomT>
where
    T: Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    let mut ret = Linear::<MonomT>::zero();
    ret.add_to(vec_like_cloned![MonomT; u[0], u[1], u[2], v[0], v[1], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], u[1], v[0], u[2], v[1], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], u[1], v[0], v[1], u[2], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], u[1], v[0], v[1], v[2], u[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], u[1], u[2], v[1], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], u[1], v[1], u[2], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], u[1], v[1], v[2], u[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1], u[1], u[2], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1], u[1], v[2], u[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; u[0], v[0], v[1], v[2], u[1], u[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], u[1], u[2], v[1], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], u[1], v[1], u[2], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], u[1], v[1], v[2], u[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1], u[1], u[2], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1], u[1], v[2], u[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], u[0], v[1], v[2], u[1], u[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0], u[1], u[2], v[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0], u[1], v[2], u[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], u[0], v[2], u[1], u[2]], 1);
    ret.add_to(vec_like_cloned![MonomT; v[0], v[1], v[2], u[0], u[1], u[2]], 1);
    ret
}


#[cfg(test)]
mod tests {
    use super::*;

    fn monom(v: &[i32]) -> Linear<Vec<i32>> {
        Linear::<Vec::<i32>>::single(v.to_vec())
    }

    #[test]
    fn shuffle_product_1_1() {
        assert_eq!(
            shuffle_product(vec![1], vec![2]),
              monom(&[1, 2])
            + monom(&[2, 1])
        );
    }

    #[test]
    fn shuffle_product_2_1() {
        assert_eq!(
            shuffle_product(vec![1,2], vec![3]),
              monom(&[1, 2, 3])
            + monom(&[1, 3, 2])
            + monom(&[3, 1, 2])
        );
    }

    #[test]
    fn shuffle_product_2_2() {
        assert_eq!(
            shuffle_product(vec![1,2], vec![3,4]),
              monom(&[1, 2, 3, 4])
            + monom(&[1, 3, 2, 4])
            + monom(&[3, 1, 2, 4])
            + monom(&[1, 3, 4, 2])
            + monom(&[3, 1, 4, 2])
            + monom(&[3, 4, 1, 2])
        );
    }
}
