// Lazily evaluated expressions. The idea is to use these for Python bindings.

#pragma once

#include <functional>
#include <memory>
#include <type_traits>

#include "lib/macros.h"
#include "lib/format.h"


template<typename ResultT>
class LazyExpr {
public:
  LazyExpr(std::function<ResultT()> function, std::string description)
    : function_(std::move(function)), description_(std::move(description)) {}

  ResultT evaluate() const { return function_(); }
  const std::string& description() const { return description_; }

private:
  std::function<ResultT()> function_;
  std::string description_;
};


namespace internal {
static constexpr char default_lazy_name[] = "<?>";

template<typename T>
T evaluate_lazy(T value) { return value; }
template<typename ResultT>
auto evaluate_lazy(const LazyExpr<ResultT>& subexpr) { return subexpr.evaluate(); }
// TODO: Add negative overloads for absl::Span, SpanX, std::string_view and other non-owning wrappers.

// TODO: Use non-debug print here.
template<typename T>
std::string describe_lazy(const T& value) { return dump_to_string(value); }
template<typename ResultT>
std::string describe_lazy(const LazyExpr<ResultT>& subexpr) { return subexpr.description(); }
}  // namespace internal


template<typename ExprT>
auto as_lazy(std::string description, ExprT expr) {
  // Wrap expression in a shared pointer to make copies cheap.
  auto as_ptr = std::make_shared<const ExprT>(std::move(expr));
  return LazyExpr<ExprT>([as_ptr]() { return *as_ptr; }, std::move(description));
};

template<typename ExprT>
auto as_lazy(ExprT&& expr) {
  return as_lazy(internal::default_lazy_name, std::forward<ExprT>(expr));
};


template<
    typename FunctionT, typename... Args,
    typename = std::enable_if_t<std::is_invocable_v<FunctionT, decltype(internal::evaluate_lazy(std::declval<Args>()))...>>
>
auto make_lazy(const std::string& name, FunctionT function, Args... args) {
  using ResultT = std::invoke_result_t<FunctionT, decltype(internal::evaluate_lazy(args))...>;
  return LazyExpr<ResultT>(
    [=]() { return function(internal::evaluate_lazy(args)...); },
    name.empty()
      ? internal::default_lazy_name
      : fmt::function(name, {internal::describe_lazy(args)...}, HSpacing::sparse)
  );
};

template<
    typename FunctionT, typename... Args,
    typename = std::enable_if_t<std::is_invocable_v<FunctionT, decltype(internal::evaluate_lazy(std::declval<Args>()))...>>
>
auto make_lazy(FunctionT&& function, Args&&... args) {
  return make_lazy(std::string{}, std::forward<FunctionT>(function), std::forward<Args>(args)...);
};

#define MAKE_LAZY(func, ...)  make_lazy(STRINGIFY(func), DISAMBIGUATE(func), __VA_ARGS__)
