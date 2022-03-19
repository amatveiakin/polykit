use std::ops;

use smallvec::SmallVec;


// A trait for objects that behave like vectors. It's not intended to provide
// complete vector API, feel free to add new fields as necessary.
pub trait VectorLike<T: Clone>:
    ops::Deref<Target = [T]> +
    IntoIterator<Item = T> +
    FromIterator<T> +
    for <'a> From<&'a [T]>
{
    fn pop(&mut self) -> Option<T>;
    fn push(&mut self, value: T);
}

#[macro_export]
macro_rules! vector_like_impl {
    () => {
        fn pop(&mut self) -> Option<T> { self.pop() }
        fn push(&mut self, value: T) { self.push(value) }
    };
}

impl<T: Clone> VectorLike<T> for Vec<T> { vector_like_impl!(); }
impl<T: Clone, const N: usize> VectorLike<T> for SmallVec<[T; N]> { vector_like_impl!(); }
