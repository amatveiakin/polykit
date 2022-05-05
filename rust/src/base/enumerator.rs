use std::hash::Hash;
use std::collections::HashMap;


pub struct Enumerator<T: Eq + Hash> {
    indices: HashMap<T, usize>,
}

impl<T: Eq + Hash> Enumerator<T> {
    pub fn new() -> Self {
        Enumerator{ indices: HashMap::new() }
    }

    pub fn index(&mut self, key: T) -> usize {
        let new_index = self.indices.len();
        *self.indices.entry(key).or_insert(new_index)
    }
    pub fn const_index(&self, key: &T) -> Option<usize> {
        self.indices.get(key).copied()
    }
    pub fn len(&self) -> usize {
        self.indices.len()
    }
}
