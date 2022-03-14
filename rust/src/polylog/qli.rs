#![allow(non_snake_case)]

use tuple_conv::TupleOrVec;

use crate::math::{DeltaExpr, cross_ratio_vec, neg_cross_ratio, tensor_product};


#[derive(Debug, Clone)]
struct Point {
    x: i32,
    odd: bool,
}

fn qli_node_func(p: &[Point]) -> DeltaExpr {
    assert_eq!(p.len(), 4);
    if p[0].odd {
        neg_cross_ratio(p[0].x, p[1].x, p[2].x, p[3].x)
    } else {
        -neg_cross_ratio(p[1].x, p[2].x, p[3].x, p[0].x)
    }
}

fn QLi_impl(weight: i32, points: &[Point]) -> DeltaExpr {
    let num_points = points.len() as i32;
    if num_points < 4 || num_points % 2 != 0 {
        panic!("Bad number of QLi points: {}", num_points);
    }
    let min_weight = (num_points - 2) / 2;
    if weight < min_weight {
        panic!("Weight too low: {}", weight);
    }

    let subsums = || {
        let mut ret = DeltaExpr::zero();
        for i in 0 .. (num_points as usize) - 3 {
            let foundation = [&points[..i+1], &points[i+3..]].concat();
            ret += tensor_product(
                &qli_node_func(&points[i..i+4]),
                &QLi_impl(weight - 1, &foundation)
            );
        }
        ret
    };
    if weight == min_weight {
        if num_points == 4 {
            qli_node_func(points)
        } else {
            subsums()
        }
    } else {
        let cross_ratio_args: Vec<i32> = points.iter().map(|p| p.x).collect();
        let mut ret = tensor_product(
            &cross_ratio_vec(&cross_ratio_args),
            &QLi_impl(weight - 1, points)
        );
        if num_points > 4 {
            ret += subsums();
        }
        ret
    }
}

pub fn QLi(weight: i32, points: &[i32]) -> DeltaExpr {
    let tagged_points: Vec<Point> =
        points.iter().enumerate().map(|(i, p)| Point{ x: *p, odd: (i+1) % 2 == 1 }).collect();
    QLi_impl(weight, &tagged_points).annotated(
        format!("QLi_{}({})", weight, points.iter().map(|p| p.to_string()).collect::<Vec<String>>().join(","))
    )
}

pub fn QLi1<Args: TupleOrVec<i32>>(args: Args) -> DeltaExpr { QLi(1, &args.as_vec()) }
pub fn QLi2<Args: TupleOrVec<i32>>(args: Args) -> DeltaExpr { QLi(2, &args.as_vec()) }
pub fn QLi3<Args: TupleOrVec<i32>>(args: Args) -> DeltaExpr { QLi(3, &args.as_vec()) }
pub fn QLi4<Args: TupleOrVec<i32>>(args: Args) -> DeltaExpr { QLi(4, &args.as_vec()) }
pub fn QLi5<Args: TupleOrVec<i32>>(args: Args) -> DeltaExpr { QLi(5, &args.as_vec()) }
pub fn QLi6<Args: TupleOrVec<i32>>(args: Args) -> DeltaExpr { QLi(6, &args.as_vec()) }
pub fn QLi7<Args: TupleOrVec<i32>>(args: Args) -> DeltaExpr { QLi(7, &args.as_vec()) }
pub fn QLi8<Args: TupleOrVec<i32>>(args: Args) -> DeltaExpr { QLi(8, &args.as_vec()) }

// Alternative:
//   #[macro_export] macro_rules! QLi1 { ( $( $x:expr ),* ) => { polylog::QLi(1, &[ $( $x, )* ]) } }
//   #[macro_export] macro_rules! QLi2 { ( $( $x:expr ),* ) => { polylog::QLi(2, &[ $( $x, )* ]) } }
//   ...
