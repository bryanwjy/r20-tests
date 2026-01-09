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

// constexpr bool valueless_by_exception() const noexcept;

#include "../../../variant_test_helpers.h"
#include "rxx/variant.h"

#include <cassert>
#include <string>

int main(int, char**) {
    {
        using V = __RXX variant<int, long>;
        constexpr V v;
        static_assert(!v.valueless_by_exception(), "");
    }
    {
        using V = __RXX variant<int, long>;
        V v;
        assert(!v.valueless_by_exception());
    }
    {
        using V = __RXX variant<int, long, std::string>;
        V const v("abc");
        assert(!v.valueless_by_exception());
    }
#if RXX_WITH_EXCEPTIONS
    {
        using V = __RXX variant<int, MakeEmptyT>;
        V v;
        assert(!v.valueless_by_exception());
        makeEmpty(v);
        assert(v.valueless_by_exception());
    }
#endif

    return 0;
}
