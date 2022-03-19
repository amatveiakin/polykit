use std::fmt::{Display};

use itertools::Itertools;


pub trait StringTools : Iterator {
    fn join_non_empty(&mut self, sep: &str) -> String
        where Self::Item: Display
    {
        self.filter_map(|v| {
            let s = v.to_string();
            if s.is_empty() { None } else { Some(s) }
        }).join(sep)
    }
}

impl<T: ?Sized> StringTools for T where T: Iterator { }
