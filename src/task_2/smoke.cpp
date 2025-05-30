#include "curry.hpp"
#include "curry.hpp"

#include <memory>
#include <string>

#include "../simple_test.h"

// #define task_2_functions
// #define task_2_functors
// #define task_2_movable_only

[[maybe_unused]]
static int constant() {
    return 32;
}

[[maybe_unused]]
static int id(int i) {
    return i;
}

[[maybe_unused]]
static int add(int lhs, int rhs) {
    return lhs + rhs;
}

[[maybe_unused]]
int sum_sptrs(std::shared_ptr<int> lhs, std::shared_ptr<int> rhs)
{
    return *lhs + *rhs;
}

[[maybe_unused]]
int add_five_args(int i1, int i2, int i3, int i4, int i5)
{
    return i1 + i2 + i3 + i4 + i5;
}

[[maybe_unused]]
static void fun(int, std::string, double) {}

struct Data {
    int i;
    explicit Data(int i): i(i) {}
};

[[maybe_unused]]
static int fun_data(Data d, Data b) {
    return d.i + b.i;
}

#ifdef task_2_functions

TEST(curry, simple_regular_functions) {
    {
        EXPECT_EQ(32, control_03::curry(constant)());
    }

    {
        EXPECT_EQ(10, control_03::curry(id)(10));
        EXPECT_EQ(20, control_03::curry(id)(20));
    }

    {
        EXPECT_EQ(3, control_03::curry(add)(1)(2));
        EXPECT_EQ(7, control_03::curry(add)(3)(4));
    }

    {
        auto lhs = std::make_unique<int>(10);
        auto rhs = std::make_unique<int>(32);

        auto res = control_03::curry(sum_sptrs)(std::move(lhs))(std::move(rhs));
        EXPECT_EQ(42, res);
    }

    {
        EXPECT_EQ(15, control_03::curry(add_five_args)(1)(2)(3)(4)(5));
        EXPECT_EQ(15, control_03::curry(add_five_args)(1, 2)(3, 4)(5));
        EXPECT_EQ(15, control_03::curry(add_five_args)(1)(2, 3, 4, 5));
    }

    {
        (void)control_03::curry(fun)(10)("hello")(12.);
    }

    {
        (void)control_03::curry(fun_data)(Data { 10 })(Data { 20 });
    }
}

#endif

struct MoveOnlyIdFunctor {
    MoveOnlyIdFunctor() = default;
    MoveOnlyIdFunctor(const MoveOnlyIdFunctor&) = delete;
    MoveOnlyIdFunctor(MoveOnlyIdFunctor&&) = default;
    MoveOnlyIdFunctor& operator=(const MoveOnlyIdFunctor&) = delete;
    MoveOnlyIdFunctor& operator=(MoveOnlyIdFunctor&&) = default;
    int operator()(int x) const { return x; }
};

struct MoveOnlySumFunctor {
    MoveOnlySumFunctor() = default;
    MoveOnlySumFunctor(const MoveOnlySumFunctor&) = delete;
    MoveOnlySumFunctor(MoveOnlySumFunctor&&) = default;
    MoveOnlySumFunctor& operator=(const MoveOnlySumFunctor&) = delete;
    MoveOnlySumFunctor& operator=(MoveOnlySumFunctor&&) = default;
    int operator()(int x, int y) const { return x + y; }
};

struct Idter {
    int operator()(int x) { return x; }
};

#ifdef task_2_functors

TEST(curry, simple_functors) {
    {
        auto constant = []() { return 32; };
        EXPECT_EQ(32, control_03::curry(constant)());
    }

    {
        EXPECT_EQ(32, control_03::curry(Idter{})(32));
        EXPECT_EQ(24, control_03::curry(Idter{})(24));
    }

    {
        auto id = [](int i) { return i; };
        EXPECT_EQ(10, control_03::curry(id)(10));
        EXPECT_EQ(20, control_03::curry(id)(20));
    }

    {
        auto add_local = [](int lhs, int rhs) { return lhs + rhs; };

        EXPECT_EQ(3, control_03::curry(add_local)(1)(2));
        EXPECT_EQ(7, control_03::curry(add_local)(3)(4));
    }

    {
        int x = 42;
        auto add42 = [x](int y) { return x + y; };
        EXPECT_EQ(43, control_03::curry(add42)(1));

        auto curryObj = control_03::curry(add42);
        EXPECT_EQ(43, curryObj(1));
    }

    {
        MoveOnlyIdFunctor functor;
        EXPECT_EQ(10, control_03::curry(std::move(functor))(10));
    }

    {
        MoveOnlyIdFunctor functor;
        auto curryObject = control_03::curry(std::move(functor));
        EXPECT_EQ(10, curryObject(10));
        EXPECT_EQ(11, curryObject(11));
        EXPECT_EQ(12, curryObject(12));
    }

    {
        MoveOnlySumFunctor functor;
        auto curryObject1 = control_03::curry(std::move(functor));
        auto curryObject2 = std::move(curryObject1)(100);

        EXPECT_EQ(110, curryObject2(10));
        EXPECT_EQ(111, curryObject2(11));
        EXPECT_EQ(112, curryObject2(12));
    }

    {
        auto sumPlus10 = [ptr = std::make_unique<int>(10)]
            (std::unique_ptr<int> p1, int x)
        {
            return *ptr + *p1 + x;
        };

        EXPECT_EQ(16, control_03::curry(std::move(sumPlus10))(std::make_unique<int>(3))(3));
    }

    {
        auto sumPlus10 = [ptr = std::make_unique<int>(10)]
            (std::unique_ptr<int> p1, int x)
        {
            return *ptr + *p1 + x;
        };

        auto curryObject1 = control_03::curry(std::move(sumPlus10));
        auto curryObject2 = std::move(curryObject1)(std::make_unique<int>(3));
        EXPECT_EQ(16, std::move(curryObject2)(3));
    }
}
#endif

#ifdef task_2_movable_only

TEST(curry, movable_only) {
    {
        {
            auto unwrap = [](std::unique_ptr<int> p1) {
                return *p1;
            };

            EXPECT_EQ(10, control_03::curry(unwrap)(std::make_unique<int>(10)));
        }

        {
            auto unwrap = [](std::unique_ptr<int> p1) {
                return *p1;
            };

            auto curried_unwrap = control_03::curry(unwrap);
            EXPECT_EQ(10, curried_unwrap(std::make_unique<int>(10)));
        }
    }

    {
        auto sum_pointers = [](std::unique_ptr<int> p1, std::unique_ptr<int> p2) {
            return *p1 + *p2;
        };

        EXPECT_EQ(30, control_03::curry(sum_pointers)(std::make_unique<int>(10))(std::make_unique<int>(20)));
    }

    {
        auto sum_pointers = [](std::unique_ptr<int> p1, std::unique_ptr<int> p2) {
            return *p1 + *p2;
        };

        auto curried_sum_pointers = control_03::curry(sum_pointers);

        EXPECT_EQ(30, curried_sum_pointers(std::make_unique<int>(10))(std::make_unique<int>(20)));
    }
}
#endif

TESTING_MAIN()
