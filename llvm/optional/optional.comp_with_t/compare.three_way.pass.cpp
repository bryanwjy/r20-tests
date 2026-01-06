// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// <optional>

// [optional.comp.with.t], comparison with T

// template<class T, class U>
//     requires (!is-derived-from-optional<U>) && three_way_comparable_with<T,
//     U>
//   constexpr compare_three_way_result_t<T, U>
//     operator<=>(const optional<T>&, const U&);

#include "../../test_comparisons.h"
#include "rxx/optional.h"

#include <cassert>
#include <compare>

struct SomeInt {
    int value_;

    constexpr explicit SomeInt(int value = 0) : value_(value) {}

    auto operator<=>(SomeInt const&) const = default;
};

template <class T, class U>
concept HasSpaceship = requires(T t, U u) { t <=> u; };

// SFINAE tests.

static_assert(std::three_way_comparable_with<__RXX optional<int>,
    __RXX optional<int>>);
static_assert(HasSpaceship<__RXX optional<int>, __RXX optional<int>>);

static_assert(std::three_way_comparable_with<__RXX optional<SomeInt>,
    __RXX optional<SomeInt>>);
static_assert(
    HasSpaceship<__RXX optional<SomeInt>, __RXX optional<SomeInt>>);

static_assert(!HasSpaceship<__RXX optional<int>, __RXX optional<SomeInt>>);

// Runtime and static tests.

constexpr void test_custom_integral() {
    {
        SomeInt t{3};
        __RXX optional<SomeInt> op{3};
        assert((t <=> op) == std::strong_ordering::equal);
        assert(testOrder(t, op, std::strong_ordering::equal));
    }
    {
        SomeInt t{2};
        __RXX optional<SomeInt> op{3};
        assert((t <=> op) == std::strong_ordering::less);
        assert(testOrder(t, op, std::strong_ordering::less));
    }
    {
        SomeInt t{3};
        __RXX optional<SomeInt> op{2};
        assert((t <=> op) == std::strong_ordering::greater);
        assert(testOrder(t, op, std::strong_ordering::greater));
    }
}

constexpr void test_int() {
    {
        int t{3};
        __RXX optional<int> op{3};
        assert((t <=> op) == std::strong_ordering::equal);
        assert(testOrder(t, op, std::strong_ordering::equal));
    }
    {
        int t{2};
        __RXX optional<int> op{3};
        assert((t <=> op) == std::strong_ordering::less);
        assert(testOrder(t, op, std::strong_ordering::less));
    }
    {
        int t{3};
        __RXX optional<int> op{2};
        assert((t <=> op) == std::strong_ordering::greater);
        assert(testOrder(t, op, std::strong_ordering::greater));
    }
}

constexpr bool test() {
    test_custom_integral();
    test_int();

    return true;
}

int main(int, char**) {
    assert(test());
    static_assert(test());
    return 0;
}
