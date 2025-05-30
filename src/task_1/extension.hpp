#pragma once
#include <tuple>
#include <utility>

namespace control_03 {

namespace details {
template <typename F, typename... Ts> class pipeable_extension {
  F &func;
  std::tuple<Ts...> values;

public:
  template <typename... Xs>
  pipeable_extension(F &f, Xs &&...ts)
      : func(f), values(std::forward<Xs>(ts)...) {}

  template <typename A> decltype(auto) call(A &&a) & {
    return std::apply(
        [this, &a](auto &&...xs) mutable -> decltype(auto) {
          return func(std::forward<A>(a), std::forward<decltype(xs)>(xs)...);
        },
        values);
  }

  template <typename A> decltype(auto) call(A &&a) && {
    return std::apply(
        // does not work because x=... creates a copy for lvalue ref
        // [this, x = std::forward<A>(a)](auto &&...xs) mutable ->
        // decltype(auto) {
        [this, &a](auto &&...xs) mutable -> decltype(auto) {
          return std::move(func)(std::forward<A>(a), std::move(xs)...);
        },
        std::move(values));
  }
};

template <typename F> class extension {
  F func;

public:
  constexpr extension(F &&f) : func(std::move(f)) {}
  constexpr extension(const F &f) : func(f) {}
  // template<typename T> extension(T &&t) : func(std::forward<T>(t)) {}

  template <typename... Ts> decltype(auto) operator()(Ts &&...xs) const {
    // std::forward_as_tuple?
    return pipeable_extension<const F, std::decay_t<Ts>...>(
        func, std::forward<Ts>(xs)...);
  }
};

template <typename A, typename F, typename... Ts>
decltype(auto) operator|(A &&a, const pipeable_extension<F, Ts...> &p) {
  return p.call(std::forward<A>(a));
}

template <typename A, typename F, typename... Ts>
decltype(auto) operator|(A &&a, pipeable_extension<F, Ts...> &&p) {
  return std::move(p).call(std::forward<A>(a));
}
} // namespace details

template <typename F> constexpr decltype(auto) make_ext(F &&func) {
  return details::extension<std::decay_t<F>>(std::forward<F>(func));
}

} // namespace control_03
