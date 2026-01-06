// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// REQUIRES: std-at-least-c++26

// <optional>

// constexpr iterator optional::end() noexcept;
// constexpr const_iterator optional::end() const noexcept;

#include "rxx/optional.h"

#include <cassert>
#include <type_traits>
#include <utility>

template <typename T>
constexpr bool test() {
    __RXX optional<T> disengaged{__RXX nullopt};

    { // end() is marked noexcept
        static_assert(noexcept(disengaged.end()));
        static_assert(noexcept(std::as_const(disengaged).end()));
    }

    { // end() == begin() and end() == end() if the optional is disengaged
        auto it = disengaged.end();
        auto it2 = std::as_const(disengaged).end();

        assert(it == disengaged.begin());
        assert(disengaged.begin() == it);
        assert(it == disengaged.end());

        assert(it2 == std::as_const(disengaged).begin());
        assert(std::as_const(disengaged).begin() == it2);
        assert(it2 == std::as_const(disengaged).end());
    }

    std::remove_reference_t<T> t = std::remove_reference_t<T>{};
    __RXX optional<T> engaged{t};

    { // end() != begin() if the optional is engaged
        auto it = engaged.end();
        auto it2 = std::as_const(engaged).end();

        assert(it != engaged.begin());
        assert(engaged.begin() != it);

        assert(it2 != std::as_const(engaged).begin());
        assert(std::as_const(engaged).begin() != it2);
    }

    return true;
}

constexpr bool tests() {
    assert(test<int>());
    assert(test<char>());
    assert(test<int const>());
    assert(test<char const>());
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    assert(test<int&>());
    assert(test<char&>());
    assert(test<int const&>());
    assert(test<char const&>());
#endif

    return true;
}

int main(int, char**) {
    assert(tests());
    static_assert(tests());

    return 0;
}
