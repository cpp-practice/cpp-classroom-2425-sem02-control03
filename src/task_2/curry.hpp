#pragma once

#include <tuple>
#include <utility>
#include <cstddef>

namespace control_03 {
    namespace details {
    // template <class F>
    // constexpr int get_args_cnt(const F& f) {

    // }

    template <class F>
    struct get_args;

    template <class F, class ...Args>
    struct get_args<F(*)(Args...)> {
        constexpr static size_t cnt = sizeof...(Args);
    };

    template <class F, class ...Args>
    struct get_args<F(&)(Args...)> {
        constexpr static size_t cnt = sizeof...(Args);
    };
}

template <class F, class ...Args>
struct Curried {
    F func;
    std::tuple<Args...> args;

    Curried(const F& f) : func(f) {}

    // Curried(F&& f) : func(std::move(f)) {}

    Curried(const F& f, std::tuple<Args...>&& args)
        : func(f), args(std::move(args)) {}

    // Curried(F&& f, std::tuple<Args...>&& args)
    //     : func(std::move(f)), args(std::move(args)) {}

    template <class ...NewArgs>
    decltype(auto) operator() (NewArgs&&... new_args) {
        constexpr size_t cnt_args = std::tuple_size_v<decltype(args)>;
        constexpr size_t cnt_new_args = sizeof...(NewArgs);
        if constexpr (details::get_args<F>::cnt <= cnt_args + cnt_new_args) {
            // return std::apply(func, std::tuplpe_cat...);
            return std::apply([this](auto&&... total_args) {
                return func(std::forward<decltype(total_args)>(total_args)...);
            }, std::tuple_cat(std::move(args),
                std::forward_as_tuple(std::forward<NewArgs>(new_args)...)
            ));
        } else {
            return Curried<F, Args..., std::decay_t<NewArgs>...>(func, std::tuple_cat(std::move(args),
                std::forward_as_tuple(std::forward<NewArgs>(new_args)...)
            ));
        }
    }
};

template <class F>
decltype(auto) curry(F&& f) {
    return Curried<F>(std::forward<F>(f));
}

}
