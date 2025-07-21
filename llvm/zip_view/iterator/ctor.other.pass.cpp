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

// constexpr iterator(iterator<!Const> i)
//       requires Const && (convertible_to<iterator_t<Views>,
//                                         iterator_t<maybe-const<Const,
//                                         Views>>> && ...);

#include "../types.h"
#include "rxx/ranges.h"

#include <cassert>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

using ConstIterIncompatibleView =
    BasicView<forward_iterator<int*>, forward_iterator<int*>,
        random_access_iterator<int const*>, random_access_iterator<int const*>>;
static_assert(
    !std::convertible_to<xranges::iterator_t<ConstIterIncompatibleView>,
        xranges::iterator_t<ConstIterIncompatibleView const>>);

constexpr bool test() {
    int buffer[3] = {1, 2, 3};

    {
        xranges::zip_view v(NonSimpleCommon{buffer});
        auto iter1 = v.begin();
        xranges::iterator_t<const decltype(v)> iter2 = iter1;
        assert(iter1 == iter2);

        static_assert(!std::is_same_v<decltype(iter1), decltype(iter2)>);

        // We cannot create a non-const iterator from a const iterator.
        static_assert(
            !std::constructible_from<decltype(iter1), decltype(iter2)>);
    }

    {
        // underlying non-const to const not convertible
        xranges::zip_view v(ConstIterIncompatibleView{buffer});
        auto iter1 = v.begin();
        auto iter2 = std::as_const(v).begin();

        static_assert(!std::is_same_v<decltype(iter1), decltype(iter2)>);

        static_assert(
            !std::constructible_from<decltype(iter1), decltype(iter2)>);
        static_assert(
            !std::constructible_from<decltype(iter2), decltype(iter1)>);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
