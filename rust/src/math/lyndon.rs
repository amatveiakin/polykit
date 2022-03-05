use std::collections::BTreeMap;

use crate::base::VectorLike;
use crate::math::{Coeff, Linear, LinearMonom, shuffle_product_multi};


fn factorial(x: Coeff) -> Coeff {
    let mut ret: Coeff = 1;
    for i in 2..=x { ret *= i; }
    ret
}

// Splits the word into a sequence of nonincreasing Lyndon words using Duval algorithm.
// Such split always exists and is unique (Chen–Fox–Lyndon theorem).
pub fn lyndon_factorize<T, Container>(word: &Container) -> Vec<Container>
where
    T: Ord + Clone,
    Container: VectorLike<T>,
{
    let n = word.len();
    let mut start = 0;
    let mut k = start;
    let mut m = start + 1;
    let mut ret = Vec::<Container>::new();
    while k < n {
        if m >= n || word[m] < word[k] {
            let l = m - k;
            ret.push(Container::from(&word[start .. start+l]));
            start += l;
            k = start;
            m = start + 1;
        } else if word[k] < word[m] {
            k = start;
            m += 1;
        } else {
            k += 1;
            m += 1;
        }
    }
    if start < n {
        ret.push(Container::from(&word[start..]));
    }
    ret
}

// Converts a linear combination of words to Lyndon basis.
//
// Algorithm. Put expression terms into a priority queue, largest term first.
// Since shuffle equation for Lyndon factorization always produces smaller words
// than the original word, this guarantees we'll process every word only once.
// For each term in the queue:
//   * If the term is already a Lyndon word, move it to the list of known Lyndon
//     words.
//   * If the term is not a Lyndon word, factorize it into Lyndon words and apply
//     shuffle equation. Replace the term with shuffle result. Put known Lyndon
//     words directly to Lyndon words list - no need to process them again. Put
//     everything else into the common queue.
// Continue until the queue is empty. Now the list of known Lyndon words contains
// entire expression in Lyndon basis.
//
// Optimization potential: Unroll Lyndon for words of 3-4 characters.
// Optimization potential: In case of words of 1-2 characters there is no need
//   to build the priority queue. In order to keep the ability to compute Lyndon
//   basis for a mixed-weight expression, we could check length when filling the
//   queue.
pub fn to_lyndon_basis<T, MonomT>(expr: Linear<MonomT>) -> Linear<MonomT>
where
    T: Ord + Clone,
    MonomT: LinearMonom + VectorLike<T>,
{
    let mut terms_to_convert = BTreeMap::<MonomT, Coeff>::new();
    for (monom, coeff) in expr.into_iter() {
        terms_to_convert.insert(monom, coeff);
    }
    let mut terms_converted = Linear::<MonomT>::zero();

    while let Some((word, coeff)) = terms_to_convert.pop_last() {
        if coeff == 0 {
            continue;
        }

        if word.len() == 1 {
            terms_converted.add_to(word, coeff);
            continue;
        }
        // TODO: Optimized solution for word.len() == 2  (see C++)

        let lyndon_words = lyndon_factorize(&word);
        assert!(!lyndon_words.is_empty());
        if lyndon_words.len() == 1 {
            terms_converted.add_to(word, coeff);
            continue;
        }

        let lyndon_expr = Linear::<MonomT>::from_collection(&lyndon_words);
        let mut denominator: Coeff = 1;
        for (_, coeff) in lyndon_expr {
            denominator *= factorial(coeff);
        }

        let mut shuffle_expr = shuffle_product_multi(&lyndon_words);
        shuffle_expr.div_int(denominator);
        assert_eq!(shuffle_expr.coeff_for(&word), 1);
        shuffle_expr.add_to(word, -1);
        for (monom, inner_coeff) in shuffle_expr {
            debug_assert_eq!(terms_converted.coeff_for(&monom), 0);
            *terms_to_convert.entry(monom).or_insert(0) -= coeff * inner_coeff;
        }
    }

    terms_converted
}


#[cfg(test)]
mod tests {
    use super::*;

    fn monom(v: &[i32]) -> Linear<Vec<i32>> {
        Linear::<Vec::<i32>>::single(v.to_vec())
    }

    #[test]
    fn lyndon_factorize_golden() {
        assert_eq!(
            lyndon_factorize(&vec![1, 2, 3, 2, 1]),
            vec![vec![1, 2, 3, 2], vec![1]]
        );
        assert_eq!(
            lyndon_factorize(&vec![1, 2, 2, 1, 1, 2]),
            vec![vec![1, 2, 2], vec![1, 1, 2]]
        );
    }

    #[test]
    fn lyndon_basis_len0() {
        assert_eq!(
            to_lyndon_basis(Linear::<Vec::<i32>>::zero()),
            Linear::<Vec::<i32>>::zero()
        );
    }

    #[test]
    fn lyndon_basis_len1() {
        let expr =
              monom(&[1])
            + monom(&[3])
            + monom(&[7])
        ;
        assert_eq!(
            to_lyndon_basis(expr.clone()),
            expr
        );
    }

    #[test]
    fn lyndon_basis_len2() {
        assert_eq!(
            to_lyndon_basis(
                  monom(&[1, 2])
                + monom(&[7, 5])
            ),
              monom(&[1, 2])
            - monom(&[5, 7])
        );
    }

    #[test]
    fn lyndon_basis_len3() {
        assert_eq!(
            to_lyndon_basis(
                  monom(&[2, 1, 3])
                + monom(&[7, 6, 5])
            ),
            - monom(&[1, 2, 3])
            - monom(&[1, 3, 2])
            + monom(&[5, 6, 7])
        );
    }
}
