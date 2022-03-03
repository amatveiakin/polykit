use smallvec::SmallVec;


// A trait for objects that behave like vectors. It's not intended to provide
// complete vector API, feel free to add new fields as necessary.
pub trait VectorLike<T> {
    fn len(&self) -> usize;
    fn is_empty(&self) -> bool;
    fn pop(&mut self) -> Option<T>;
    fn push(&mut self, value: T);
}

macro_rules! vector_like_impl {
    () => {
        fn len(&self) -> usize { self.len() }
        fn is_empty(&self) -> bool { self.is_empty() }
        fn pop(&mut self) -> Option<T> { self.pop() }
        fn push(&mut self, value: T) { self.push(value) }
    };
}

impl<T> VectorLike<T> for Vec<T> { vector_like_impl!(); }
impl<T, const N: usize> VectorLike<T> for SmallVec<[T; N]> { vector_like_impl!(); }

