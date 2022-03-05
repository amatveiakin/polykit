use std::ops::{Index, Range, RangeFrom, RangeTo};

use smallvec::SmallVec;


// A trait for objects that behave like vectors. It's not intended to provide
// complete vector API, feel free to add new fields as necessary.
pub trait VectorLike<T: Clone>:
    Index<usize, Output = T> +
    Index<Range<usize>, Output = [T]> +
    Index<RangeFrom<usize>, Output = [T]> +
    Index<RangeTo<usize>, Output = [T]> +
    // Why doesn't this work?
    //   TODO: Debug and use instead of manual `from(...)`; remove `as From` casts
    //   in various places afterwards.
    // From<&[T]>
{
    fn from(s: &[T]) -> Self;
    fn len(&self) -> usize;
    fn is_empty(&self) -> bool;
    fn pop(&mut self) -> Option<T>;
    fn push(&mut self, value: T);
}

#[macro_export]
macro_rules! vector_like_impl {
    () => {
        fn from(s: &[T]) -> Self { <Self as From<&[T]>>::from(s) }
        fn len(&self) -> usize { self.len() }
        fn is_empty(&self) -> bool { self.is_empty() }
        fn pop(&mut self) -> Option<T> { self.pop() }
        fn push(&mut self, value: T) { self.push(value) }
    };
}

impl<T: Clone> VectorLike<T> for Vec<T> { vector_like_impl!(); }
impl<T: Clone, const N: usize> VectorLike<T> for SmallVec<[T; N]> { vector_like_impl!(); }
