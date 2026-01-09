// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <optional>
// UNSUPPORTED: c++03, c++11, c++14

// template<class T>
//   optional(T) -> optional<T>;

#include "../../../static_asserts.h"
#include "rxx/optional.h"

#include <cassert>

struct A {};

int main(int, char**) {
    //  Test the explicit deduction guides
    {
        //  optional(T)
        __RXX optional opt(5);
        ASSERT_SAME_TYPE(decltype(opt), __RXX optional<int>);
        assert(static_cast<bool>(opt));
        assert(*opt == 5);
    }

    {
        //  optional(T)
        __RXX optional opt(A{});
        ASSERT_SAME_TYPE(decltype(opt), __RXX optional<A>);
        assert(static_cast<bool>(opt));
    }

    {
        //  optional(const T&);
        int const& source = 5;
        __RXX optional opt(source);
        ASSERT_SAME_TYPE(decltype(opt), __RXX optional<int>);
        assert(static_cast<bool>(opt));
        assert(*opt == 5);
    }

    {
        //  optional(T*);
        int const* source = nullptr;
        __RXX optional opt(source);
        ASSERT_SAME_TYPE(decltype(opt), __RXX optional<int const*>);
        assert(static_cast<bool>(opt));
        assert(*opt == nullptr);
    }

    {
        //  optional(T[]);
        int source[] = {1, 2, 3};
        __RXX optional opt(source);
        ASSERT_SAME_TYPE(decltype(opt), __RXX optional<int*>);
        assert(static_cast<bool>(opt));
        assert((*opt)[0] == 1);
    }

    //  Test the implicit deduction guides
    {
        //  optional(optional);
        __RXX optional<char> source('A');
        __RXX optional opt(source);
        ASSERT_SAME_TYPE(decltype(opt), __RXX optional<char>);
        assert(static_cast<bool>(opt) == static_cast<bool>(source));
        assert(*opt == *source);
    }

    return 0;
}
