#include "extension.hpp"
#include "extension.hpp"

#include <memory>
#include <string>

#include "../simple_test.h"

int double_int(int val) {
    return val * 2;
}

auto double_int_ext = control_03::make_ext(double_int);

int multiply(int lhs, int rhs) {
    return lhs * rhs;
}

auto multiply_ext = control_03::make_ext(multiply);

TEST(extensions, simple_functions) {
    {
        EXPECT_EQ(20, (10 | double_int_ext()));
        EXPECT_EQ(40, (20 | double_int_ext()));

        EXPECT_EQ(4, (1 | double_int_ext() | double_int_ext()));
    }

    {
        EXPECT_EQ(0, (0 | multiply_ext(10)));
        EXPECT_EQ(10, (1 | multiply_ext(10)));
        EXPECT_EQ(20, (2 | multiply_ext(10)));

        EXPECT_EQ(2 * 20 * 3, (2 | multiply_ext(20) | multiply_ext(3)));
    }

    {
        EXPECT_EQ(40, (1 | double_int_ext() | multiply_ext(20)));
    }
}

TEST(extensions, simple_functors) {
    {
        auto ext = control_03::make_ext([](int i) { return i; });
        EXPECT_EQ(10, (10 | ext()));
    }

    {
        auto ext = control_03::make_ext([](int lhs, int rhs) { return lhs * rhs; });

        EXPECT_EQ(200, (10 | ext(20)));
    }
}

struct add {
    template <typename L, typename R>
    auto operator()(L left, R right) const {
        return left + right;
    }
};

constexpr auto add_ext = control_03::make_ext(add{});


TEST(extensions, functor_overloads) {
    EXPECT_EQ("hellohi", (std::string("hello") | add_ext("hi")));
    EXPECT_EQ("hihello", ("hi" | add_ext(std::string("hello"))));
    EXPECT_EQ(30, (10 | add_ext(20)));
}

TEST(extensions, mutating_extensions) {
    auto starts_with_ext = control_03::make_ext([](std::string const& s, std::string prefix) {
        return s.substr(0, prefix.size()) == prefix;
    });

    auto trim_prefix = control_03::make_ext([starts_with_ext](std::string& s, std::string prefix) {
        if (s | starts_with_ext(prefix)) {
            s = s.substr(prefix.size());
        }
    });

    std::string value {"hello!"};
    value | trim_prefix("he");

    EXPECT_EQ("llo!", value);
}

int x = 32;

TEST(extensions, lambda_returns_ref) {
    const auto lambda = [](int, int) -> int& { return x; };
    auto ext = control_03::make_ext(lambda);
    auto& r = (10 | ext(10));

    EXPECT_EQ(32, r);
    EXPECT_EQ(&x, &r);
}

TEST(extensions, move_only_args) {
    auto functor = [](int x, std::unique_ptr<int> ptr) {
        return *ptr + x;
    };
    auto ext = control_03::make_ext(functor);
    EXPECT_EQ(8, (5 | ext(std::make_unique<int>(3))));
}

static std::string copy_string(std::string const& ref) {
    return ref;
}

TEST(extensions, on_const_lvalue_ref) {
    auto copy_string_ext = control_03::make_ext(copy_string);

    std::string const s{"hello world i am big string"};

    (void)(s | copy_string_ext());
}

static std::string move_string_away(std::string&& ref) {
    return std::move(ref);
}

TEST(extensions, on_rvalue_ref) {
    auto move_string_away_ext = control_03::make_ext(move_string_away);

    std::string s{"hello world i am big string"};

    (void)(std::move(s) | move_string_away_ext());
}

TESTING_MAIN()
