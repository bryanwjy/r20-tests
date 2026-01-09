// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <variant>

// template <class ...Types> class variant;

// constexpr size_t index() const noexcept;

#include "../../../variant_test_helpers.h"
#include "rxx/variant.h"

#include <cassert>
#include <string>

int main(int, char**) {
    {
        using V = __RXX variant<int, long>;
        constexpr V v;
        static_assert(v.index() == 0, "");
    }
    {
        using V = __RXX variant<int, long>;
        V v;
        assert(v.index() == 0);
    }
    {
        using V = __RXX variant<int, long>;
        constexpr V v(std::in_place_index<1>);
        static_assert(v.index() == 1, "");
    }
    {
        using V = __RXX variant<int, std::string>;
        V v("abc");
        assert(v.index() == 1);
        v = 42;
        assert(v.index() == 0);
    }
#if RXX_WITH_EXCEPTIONS
    {
        using V = __RXX variant<int, MakeEmptyT>;
        V v;
        assert(v.index() == 0);
        makeEmpty(v);
        assert(v.index() == __RXX variant_npos);
    }
#endif

    return 0;
}
