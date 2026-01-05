// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20
// <optional>

// template<class F> constexpr optional or_else(F&&) &&;
// template<class F> constexpr optional or_else(F&&) const&;

#include "../../MoveOnly.h"
#include "rxx/optional.h"

#include <cassert>

struct NonMovable {
    NonMovable() = default;
    NonMovable(NonMovable&&) = delete;
};

template <class Opt, class F>
concept has_or_else = requires(Opt&& opt, F&& f) {
    { std::forward<Opt>(opt).or_else(std::forward<F>(f)) };
};

template <class T>
__RXX optional<T> return_optional() {}

static_assert(
    has_or_else<__RXX optional<int>&, decltype(return_optional<int>)>);
static_assert(
    has_or_else<__RXX optional<int>&&, decltype(return_optional<int>)>);
static_assert(!has_or_else<__RXX optional<MoveOnly>&,
    decltype(return_optional<MoveOnly>)>);
static_assert(has_or_else<__RXX optional<MoveOnly>&&,
    decltype(return_optional<MoveOnly>)>);
static_assert(!has_or_else<__RXX optional<NonMovable>&,
    decltype(return_optional<NonMovable>)>);
static_assert(!has_or_else<__RXX optional<NonMovable>&&,
    decltype(return_optional<NonMovable>)>);

__RXX optional<int> take_int(int) {
    return 0;
}
void take_int_return_void(int) {}

static_assert(!has_or_else<__RXX optional<int>, decltype(take_int)>);
static_assert(
    !has_or_else<__RXX optional<int>, decltype(take_int_return_void)>);
static_assert(!has_or_else<__RXX optional<int>, int>);

constexpr bool test() {
    {
        __RXX optional<int> opt;
        assert(opt.or_else([] { return __RXX optional<int>{0}; }) == 0);
        opt = 1;
        (void)opt.or_else([] {
            assert(false);
            return __RXX optional<int>{};
        });
    }
    {
        __RXX optional<MoveOnly> opt;
        opt = std::move(opt).or_else(
            [] { return __RXX optional<MoveOnly>{MoveOnly{}}; });
        (void)std::move(opt).or_else([] {
            assert(false);
            return __RXX optional<MoveOnly>{};
        });
    }
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    {
        int i = 2;
        __RXX optional<int&> opt;
        assert(opt.or_else([&] { return __RXX optional<int&>{i}; }) == i);
        int j = 3;
        opt = j;
        (void)opt.or_else([] {
            assert(false);
            return __RXX optional<int&>{};
        });
        assert(opt == j);
    }
    {
        int i = 2;
        __RXX optional<int&> opt;
        assert(std::move(opt).or_else(
                   [&] { return __RXX optional<int&>{i}; }) == i);
        int j = 3;
        opt = j;
        (void)std::move(opt).or_else([] {
            assert(false);
            return __RXX optional<int&>{};
        });
        assert(opt == j);
    }
#endif

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());
    return 0;
}
