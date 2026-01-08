// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/variant.h"

// P2231R1 Missing constexpr in std::optional and __RXX variant

#if __cpp_lib_constexpr_exceptions >= 202502L && RXX_CXX26
#  include <string>
#endif
#include <cassert>

constexpr bool test_assign() {
    __RXX variant<int, double> v1(1);
    v1 = 2.5;
    assert(__RXX get<double>(v1) == 2.5);

    v1 = 99;
    assert(__RXX get<int>(v1) == 99);
    v1 = 999;
    assert(__RXX get<int>(v1) == 999);

    struct S // non-trivial special members
    {
        constexpr S(int i) : i(i) {}
        constexpr ~S() {}
        constexpr S(S const& s) : i(s.i) {}

        int i;
    };

    __RXX variant<int, S> v;
    v = S(123);
    assert(__RXX get<1>(v).i == 123);

    S const s(456);
    v = s;
    assert(__RXX get<1>(v).i == 456);

    v = 789;
    assert(__RXX get<0>(v) == 789);

    return true;
}

static_assert(test_assign());

#if __cpp_lib_constexpr_exceptions >= 202502L && RXX_CXX26
constexpr bool test_get() {
    assert(__RXX get<1>(__RXX variant<int, std::string>("a")) == "a");
    assert( __RXX get<std::string>(__RXX variant<int, std::string>("a")) ==
        "a");
    {
        try {
            __RXX get<0>(__RXX variant<int, std::string>("a"));
        } catch (__RXX bad_variant_access const& x) {
            long c = x.what()[0];
            assert(c == x.what()[0]);
        }
    }
    {
        try {
            __RXX get<int>(__RXX variant<int, std::string>("a"));
        } catch (__RXX bad_variant_access const& x) {
            long c = x.what()[0];
            assert(c == x.what()[0]);
        }
    }
    return true;
}

static_assert(test_get());
#endif

constexpr bool test_emplace() {
    struct S // non-trivial special members
    {
        constexpr S(std::initializer_list<int> l) : i(l.begin()[0]) {}
        constexpr S(std::initializer_list<int> l, int n) : i(l.begin()[n]) {}
        constexpr ~S() {}
        constexpr S(S const& s) : i(s.i) {}

        int i;
    };

    __RXX variant<int, double, S> v(1);

    // template<class T, class... Args> constexpr T& emplace(Args&&... args);
    v.emplace<double>(2.0);
    assert(__RXX get<1>(v) == 2.0);
    v.emplace<double>(2.5);
    assert(__RXX get<1>(v) == 2.5);
    v.emplace<int>(2.5);
    assert(__RXX get<0>(v) == 2);

    // template<class T, class U, class... Args>
    // constexpr T& emplace(initializer_list<U>, Args&&... args);
    v.emplace<S>({3, 2, 1});
    assert(__RXX get<2>(v).i == 3);
    v.emplace<S>({3, 2, 1}, 1);
    assert(__RXX get<2>(v).i == 2);

    // template<size_t I, class... Args>
    // constexpr variant_alternative_t<I, ...>& emplace(Args&&... args);
    v.emplace<1>(3.0);
    assert(__RXX get<1>(v) == 3.0);
    v.emplace<1>(0.5);
    assert(__RXX get<1>(v) == 0.5);
    v.emplace<0>(1.5);
    assert(__RXX get<0>(v) == 1);

    // template<size_t I, class U, class... Args>
    // constexpr variant_alternative_t<I, ...>&
    // emplace(initializer_list<U>, Args&&... args);
    v.emplace<2>({7, 8, 9});
    assert(__RXX get<2>(v).i == 7);
    v.emplace<2>({13, 12, 11}, 1);
    assert(__RXX get<2>(v).i == 12);

    return true;
}

static_assert(test_emplace());

constexpr bool test_swap() {
    __RXX variant<int, double> v1(1), v2(2.5);
    v1.swap(v2);
    assert(__RXX get<double>(v1) == 2.5);
    assert(__RXX get<int>(v2) == 1);

    swap(v1, v2);
    assert(__RXX get<int>(v1) == 1);
    assert(__RXX get<double>(v2) == 2.5);

    struct S {
        constexpr S(int i) : i(i) {}
        constexpr S(S&& s) : i(s.i) {}
        constexpr S& operator=(S&& s) {
            i = s.i;
            s.i = -1;
            return *this;
        }

        int i;
    };

    __RXX variant<int, S> v3(3), v4(S(4));
    v3.swap(v4);
    assert(__RXX get<S>(v3).i == 4);
    assert(__RXX get<int>(v4) == 3);
    v3.swap(v4);
    assert(__RXX get<int>(v3) == 3);
    assert(__RXX get<S>(v4).i == 4);

    return true;
}

static_assert(test_swap());
