// General philosophy behind `to_string` and `dump_to_string`.
//
// `to_string` gives a nicely formatted representation of a object that can
// and should be used for output. `to_string` is extensible: just define
// `to_string(const MyClass&)`.
//
// `dump_to_string` gives a human-readable but not necessarily elegant
// representation of an object that can be used for debugging or for CHECK-s
// that are not expected to fail. `dump_to_string(obj)` tries to print `obj`
// with various printers in the following order:
//   * if `dump_to_string_impl(obj)` exists, call it;
//   * if `to_string(obj)` exists, call it;
//   * if `obj` is a supported std/absl container, print its content;
//   * print implementation-defined `obj` type name (this is not particularly
//     useful, but it allows to freely use `dump_to_string` in generic code
//     without worrying that a non-printable type breaks it).
// In order to extend `dump_to_string`, define `dump_to_string_impl(const MyClass&)`,
// not `dump_to_string(const MyClass&)`!

#pragma once

#include <string>
#include <typeinfo>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/container/inlined_vector.h"
#include "absl/strings/str_join.h"


// For generic programming like str_join
inline std::string to_string(int x) { return std::to_string(x); }
inline std::string to_string(long x) { return std::to_string(x); }
inline std::string to_string(long long x) { return std::to_string(x); }
inline std::string to_string(unsigned x) { return std::to_string(x); }
inline std::string to_string(unsigned long x) { return std::to_string(x); }
inline std::string to_string(unsigned long long x) { return std::to_string(x); }
inline std::string to_string(float x) { return std::to_string(x); }
inline std::string to_string(double x) { return std::to_string(x); }
inline std::string to_string(long double x) { return std::to_string(x); }
inline std::string to_string(std::string s) { return s; }


template<typename T, typename F>
std::string str_join(const T& container, std::string separator, F element_to_string) {
  return absl::StrJoin(container, separator, [&](std::string* out, const auto& value) {
    out->append(element_to_string(value));
  });
}

template<typename T>
std::string str_join(const T& container, std::string separator) {
  return str_join(container, separator, [](const auto& value) {
    return to_string(value);
  });
}


namespace internal {
// Returns type name, unmangled if possible. Unmangling is supported for clang, gcc and MSVC.
// From https://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname
template<typename T>
constexpr std::string_view get_type_name() {
#if defined(__clang__)
  constexpr std::string_view prefix = "[T = ";
  constexpr std::string_view suffix = "]";
  constexpr std::string_view function = __PRETTY_FUNCTION__;
#elif defined(__GNUC__)
  constexpr std::string_view prefix = "with T = ";
  constexpr std::string_view suffix = "; ";
  constexpr std::string_view function = __PRETTY_FUNCTION__;
#elif defined(__MSC_VER)
  constexpr std::string_view prefix = "get_type_name<";
  constexpr std::string_view suffix = ">(void)";
  constexpr std::string_view function = __FUNCSIG__;
#else
  return typeid(T).name();
#endif
  const auto start = function.find(prefix) + prefix.size();
  const auto end = function.find(suffix);
  if (start == std::string_view::npos || end == std::string_view::npos || start >= end) {
    return typeid(T).name();
  }
  const auto size = end - start;
  return function.substr(start, size);
}

class DumpToStringHelper {
private:
  template<typename Container>
  static auto map_to_vector(const Container& c) {
    std::vector<std::pair<typename Container::key_type, typename Container::mapped_type>> ret(c.begin(), c.end());
    absl::c_sort(ret, [](const auto& lhs, const auto& rhs) {
      return lhs.first < rhs.first;
    });
    return ret;
  }
  template<typename Container>
  static auto set_to_vector(const Container& c) {
    std::vector<typename Container::value_type> ret(c.begin(), c.end());
    absl::c_sort(ret);
    return ret;
  }

  template <typename T>
  class HasDumpToStringImpl {
  private:
    typedef char YesType[1];
    typedef char NoType[2];
    template <typename U> static YesType& test(decltype(dump_to_string_impl(std::declval<U>())));
    template <typename U> static NoType& test(...);
  public:
    enum { value = sizeof(test<T>("")) == sizeof(YesType) };
  };

  template <typename T>
  class HasToString {
  private:
    typedef char YesType[1];
    typedef char NoType[2];
    template <typename U> static YesType& test(decltype(to_string(std::declval<U>())));
    template <typename U> static NoType& test(...);
  public:
    enum { value = sizeof(test<T>("")) == sizeof(YesType) };
  };

  template<typename T>
  static std::string auto_dump(const T&) {
    return absl::StrCat("<", get_type_name<T>(), ">");
  }

  template<typename T, typename U>
  static std::string auto_dump(const std::pair<T, U>& v) {
    return absl::StrCat("(", dump_impl(v.first), ", ", dump_impl(v.second), ")");
  }
  template<typename... Args>
  static std::string auto_dump(const std::tuple<Args...>& v) {
    return absl::StrCat("(", absl::StrJoin(v, ", ", [](std::string* out, const auto& e) {
      out->append(dump_impl(e));
    }), ")");
  }

  template<typename T>
  static std::string auto_dump(const std::vector<T>& v) {
    return absl::StrCat("[", str_join(v, ", ", [](const auto& e) { return dump_impl(e); }), "]");
  }
  template<typename T, size_t N>
  static std::string auto_dump(const std::array<T, N>& v) {
    return absl::StrCat("[", str_join(v, ", ", [](const auto& e) { return dump_impl(e); }), "]");
  }
  template<typename T, size_t N>
  static std::string auto_dump(const absl::InlinedVector<T, N>& v) {
    return absl::StrCat("[", str_join(v, ", ", [](const auto& e) { return dump_impl(e); }), "]");
  }
  template<typename T>
  static std::string auto_dump(absl::Span<const T> v) {
    return absl::StrCat("[", str_join(v, ", ", [](const auto& e) { return dump_impl(e); }), "]");
  }

  template<typename T, typename Comp>
  static std::string auto_dump(const std::set<T, Comp>& v) {
    return absl::StrCat("{", str_join(set_to_vector(v), ", ", [](const auto& e) { return dump_impl(e); }), "}");
  }
  template<typename T>
  static std::string auto_dump(const absl::flat_hash_set<T>& v) {
    return absl::StrCat("{", str_join(set_to_vector(v), ", ", [](const auto& e) { return dump_impl(e); }), "}");
  }

  template<typename K, typename V, typename Comp>
  static std::string auto_dump(const std::map<K, V, Comp>& v) {
    return absl::StrCat("{", str_join(map_to_vector(v), ", ", [](const auto& e) {
      return absl::StrCat(dump_impl(e.first), " => ", dump_impl(e.second));
    }), "}");
  }
  template<typename K, typename V>
  static std::string auto_dump(const absl::flat_hash_map<K, V>& v) {
    return absl::StrCat("{", str_join(map_to_vector(v), ", ", [](const auto& e) {
      return absl::StrCat(dump_impl(e.first), " => ", dump_impl(e.second));
    }), "}");
  }

public:
  template<typename T>
  static typename std::enable_if_t<HasDumpToStringImpl<T>::value, std::string>
  dump_impl(T& v) {
    return dump_to_string_impl(v);
  }
  template<typename T>
  static typename std::enable_if_t<!HasDumpToStringImpl<T>::value && HasToString<T>::value, std::string>
  dump_impl(T& v) {
    return to_string(v);
  }
  template<typename T>
  static typename std::enable_if_t<!HasDumpToStringImpl<T>::value && !HasToString<T>::value, std::string>
  dump_impl(T& v) {
    return auto_dump(v);
  }
};
}  // namespace internal

template<typename T>
std::string dump_to_string(const T& v) {
  return internal::DumpToStringHelper::dump_impl(v);
}
