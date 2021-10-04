// Like `std::variant`, but has zero overhead in case of a single element.
// Deliberately doesn't implement `valueless_by_exception`, because it has different
// semantics around this.

#pragma once

#include <variant>


template<typename... Ts>
class CompactVariant : public std::variant<Ts...> {
public:
  using std::variant<Ts...>::variant;
};

// TODO: Implement other `std::variant` methods and relevant non-member function.
template<typename T>
class CompactVariant<T> {
public:
  constexpr CompactVariant(T value) : value_(std::move(value)) {}
  constexpr explicit CompactVariant(std::in_place_index_t<0>, T value) : value_(std::move(value)) {}

  constexpr std::size_t index() const noexcept { return 0; }

  constexpr bool operator==(const CompactVariant<T>& other) const { return value_ == other.value_; }
  constexpr bool operator!=(const CompactVariant<T>& other) const { return value_ != other.value_; }
  constexpr bool operator< (const CompactVariant<T>& other) const { return value_ <  other.value_; }
  constexpr bool operator<=(const CompactVariant<T>& other) const { return value_ <= other.value_; }
  constexpr bool operator> (const CompactVariant<T>& other) const { return value_ >  other.value_; }
  constexpr bool operator>=(const CompactVariant<T>& other) const { return value_ >= other.value_; }

  template <typename H>
  friend H AbslHashValue(H h, const CompactVariant& var) {
    return H::combine(std::move(h), var.value_);
  }

private:
  T value_;
};
