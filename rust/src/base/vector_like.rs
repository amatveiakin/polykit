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
    // `$self_object` must be `self`
    ($self_object:ident => $vector_object:expr, element_type = $element_type:ty) => {
        fn pop(&mut $self_object) -> Option<$element_type> { $vector_object.pop() }
        fn push(&mut $self_object, value: $element_type) { $vector_object.push(value) }
    };
}

impl<T: Clone> VectorLike<T> for Vec<T> {
    vector_like_impl!(self => self, element_type = T);
}
impl<T: Clone, const N: usize> VectorLike<T> for SmallVec<[T; N]> {
    vector_like_impl!(self => self, element_type = T);
}
