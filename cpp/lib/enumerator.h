#pragma once

#include "absl/container/flat_hash_map.h"


template<typename T>
class Enumerator {
public:
  int index(const T& key) {
    const auto it = index_map_.find(key);
    if (it != index_map_.end()) {
      return it->second;
    }
    const int new_index = index_map_.size();
    index_map_[key] = new_index;
    return new_index;
  }
  int c_index(const T& key) const {
    return index_map_.at(key);
  }

  int size() const { return index_map_.size(); }

private:
  absl::flat_hash_map<T, int> index_map_;
};
