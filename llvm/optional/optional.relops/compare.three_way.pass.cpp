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

// [optional.relops], relational operators

// template<class T, three_way_comparable_with<T> U>
//   constexpr compare_three_way_result_t<T, U>
//     operator<=>(const optional<T>&, const optional<U>&);

#include "../../test_comparisons.h"
#include "rxx/optional.h"

#include <cassert>
#include <compare>

constexpr bool test() {
    {
        __RXX optional<int> op1;
        __RXX optional<int> op2;

        assert((op1 <=> op2) == std::strong_ordering::equal);
        assert(testOrder(op1, op2, std::strong_ordering::equal));
    }
    {
        __RXX optional<int> op1{3};
        __RXX optional<int> op2{3};
        assert((op1 <=> op1) == std::strong_ordering::equal);
        assert(testOrder(op1, op1, std::strong_ordering::equal));
        assert((op1 <=> op2) == std::strong_ordering::equal);
        assert(testOrder(op1, op2, std::strong_ordering::equal));
        assert((op2 <=> op1) == std::strong_ordering::equal);
        assert(testOrder(op2, op1, std::strong_ordering::equal));
    }
    {
        __RXX optional<int> op;
        __RXX optional<int> op1{2};
        __RXX optional<int> op2{3};
        assert((op <=> op2) == std::strong_ordering::less);
        assert(testOrder(op, op2, std::strong_ordering::less));
        assert((op1 <=> op2) == std::strong_ordering::less);
        assert(testOrder(op1, op2, std::strong_ordering::less));
    }
    {
        __RXX optional<int> op;
        __RXX optional<int> op1{3};
        __RXX optional<int> op2{2};
        assert((op1 <=> op) == std::strong_ordering::greater);
        assert(testOrder(op1, op, std::strong_ordering::greater));
        assert((op1 <=> op2) == std::strong_ordering::greater);
        assert(testOrder(op1, op2, std::strong_ordering::greater));
    }

    return true;
}

int main(int, char**) {
    assert(test());
    static_assert(test());
    return 0;
}
