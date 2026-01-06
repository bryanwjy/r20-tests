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

// [optional.nullops], comparison with nullopt

// template<class T>
//   constexpr strong_ordering operator<=>(const optional<T>&, nullopt_t)
//   noexcept;

#include "../../test_comparisons.h"
#include "rxx/optional.h"

#include <cassert>
#include <compare>

constexpr bool test() {
    {
        __RXX optional<int> op;
        assert((__RXX nullopt <=> op) == std::strong_ordering::equal);
        assert(testOrder(__RXX nullopt, op, std::strong_ordering::equal));
        assert((op <=> __RXX nullopt) == std::strong_ordering::equal);
        assert(testOrder(op, __RXX nullopt, std::strong_ordering::equal));
    }
    {
        __RXX optional<int> op{1};
        assert((__RXX nullopt <=> op) == std::strong_ordering::less);
        assert(testOrder(__RXX nullopt, op, std::strong_ordering::less));
    }
    {
        __RXX optional<int> op{1};
        assert((op <=> __RXX nullopt) == std::strong_ordering::greater);
        assert(testOrder(op, __RXX nullopt, std::strong_ordering::greater));
    }

    return true;
}

int main(int, char**) {
    assert(test());
    static_assert(test());
    return 0;
}
