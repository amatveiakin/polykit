use std::collections::HashSet;

use crate::base::Enumerator;
use crate::math::{LinearMonom, Linear, Matrix};


pub struct ExprMatrixBuilder<T: LinearMonom> {
    monom_enumerator: Enumerator<T>,
    // For each row: for each non-zero value: (column, value)
    rows: HashSet<Vec<SparseElement>>,
}

impl<T: LinearMonom> ExprMatrixBuilder<T> {
    pub fn new() -> Self {
        ExprMatrixBuilder {
            monom_enumerator: Enumerator::new(),
            rows: HashSet::new(),
        }
    }

    pub fn add_expr(&mut self, expr: Linear<T>) {
        let mut row = Vec::new();
        for (monom, coeff) in expr.into_iter() {
            let col_idx = self.monom_enumerator.index(monom).try_into().unwrap();
            row.push((col_idx, coeff));
        }
        self.rows.insert(row);
    }

    pub fn make_matrix(&self) -> Matrix {
        let mut m = Matrix::new();
        for (col_idx, col) in self.unique_columns().iter().enumerate() {
            for (row_idx, v) in col {
                let col_idx = col_idx.try_into().unwrap();
                m.set(*row_idx, col_idx, *v);
            }
        }
        m
    }

    fn unique_columns(&self) -> HashSet<Vec<SparseElement>> {
        let mut cols = vec![vec![]; self.monom_enumerator.len()];
        for (row_idx, row) in self.rows.iter().enumerate() {
            for (col_idx, v) in row {
                let col_idx: usize = (*col_idx).try_into().unwrap();
                let row_idx = row_idx.try_into().unwrap();
                cols[col_idx].push((row_idx, *v));
            }
        }
        HashSet::from_iter(cols.into_iter())
    }
}

type SparseElement = (u32, i32);  // (row/col, value)
