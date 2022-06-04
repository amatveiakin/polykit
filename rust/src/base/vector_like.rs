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
    fn new() -> Self;
    fn pop(&mut self) -> Option<T>;
    fn push(&mut self, value: T);
    fn reserve(&mut self, additional: usize);
    fn reserve_exact(&mut self, additional: usize);
}

#[macro_export]
macro_rules! vector_like_impl {
    // `$self_object` must be `self`
    ($self_type:ty, $self_object:ident => $vector_object:expr, element_type = $element_type:ty) => {
        fn new() -> Self { <$self_type>::new() }
        fn pop(&mut $self_object) -> Option<$element_type> { $vector_object.pop() }
        fn push(&mut $self_object, value: $element_type) { $vector_object.push(value) }
        fn reserve(&mut $self_object, additional: usize) { $vector_object.reserve(additional) }
        fn reserve_exact(&mut $self_object, additional: usize) { $vector_object.reserve_exact(additional) }
    };
}

impl<T: Clone> VectorLike<T> for Vec<T> {
    vector_like_impl!(Self, self => self, element_type = T);
}
impl<T: Clone, const N: usize> VectorLike<T> for SmallVec<[T; N]> {
    vector_like_impl!(Self, self => self, element_type = T);
}

macro_rules! count_tts {
    () => { 0usize };
    ($_head:tt $($tail:tt)*) => { 1usize + count_tts!( $( $tail )* ) };
}

#[macro_export]
macro_rules! vec_like {
    ($type:ty; $($x:expr),+ $(,)?) => {
        {
            let mut ret = <$type>::new();
            ret.reserve_exact(count_tts!( $( $x )* ));
            $(
                ret.push($x);
            )*
            ret
        }
    };
}
