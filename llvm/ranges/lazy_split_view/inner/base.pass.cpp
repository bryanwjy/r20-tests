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

// constexpr const iterator_t<Base>& inner-iterator::base() const& noexcept;
//
// constexpr iterator_t<Base> inner-iterator::base() &&
//   requires forward_range<View>;

#include "../../../static_asserts.h"
#include "../types.h"
#include "rxx/ranges.h"

#include <concepts>
#include <utility>

namespace xranges = __RXX ranges;

static_assert(noexcept(std::declval<InnerIterForward&>().base()));
static_assert(noexcept(std::declval<InnerIterForward const&>().base()));
static_assert(noexcept(std::declval<InnerIterForward const&&>().base()));
static_assert(noexcept(std::declval<InnerIterInput&>().base()));
static_assert(noexcept(std::declval<InnerIterInput const&>().base()));
static_assert(noexcept(std::declval<InnerIterInput const&&>().base()));

constexpr bool test() {
    // `base` works with a forward view (two different overloads based on
    // ref-qualification of the `inner-iterator`).
    {
        using BaseIter = xranges::iterator_t<CopyableView>;
        CopyableView input("abc def");
        xranges::lazy_split_view<CopyableView, ForwardView> v(input, " ");
        auto i = (*v.begin()).begin();
        auto const ci = i;

        // Note: some macOS platforms seem to have trouble deducing the type
        // when using `std::same_as` -- use the equivalent `ASSERT_SAME_TYPE`
        // instead.
        {
            decltype(auto) b = i.base();
            ASSERT_SAME_TYPE(decltype(b), BaseIter const&);
            assert(b == input.begin());
        }

        {
            decltype(auto) b = ci.base();
            ASSERT_SAME_TYPE(decltype(b), BaseIter const&);
            assert(b == input.begin());
        }

        {
            decltype(auto) b = std::move(i).base();
            ASSERT_SAME_TYPE(decltype(b), BaseIter);
            assert(b == input.begin());
        }

        {
            decltype(auto) b = std::move(ci).base();
            ASSERT_SAME_TYPE(decltype(b), BaseIter const&);
            assert(b == input.begin());
        }
    }

    // `base` works with an input view (no overloads).
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AT_LEAST(15)
    if (!std::is_constant_evaluated())
#endif
    {
        using BaseIter = xranges::iterator_t<InputView>;
        InputView input("abc def");
        xranges::lazy_split_view<InputView, ForwardTinyView> v(input, ' ');
        auto i = (*v.begin()).begin();
        auto const ci = i;

        {
            decltype(auto) b = i.base();
            ASSERT_SAME_TYPE(decltype(b), BaseIter const&);
        }

        {
            decltype(auto) b = ci.base();
            ASSERT_SAME_TYPE(decltype(b), BaseIter const&);
        }

        {
            decltype(auto) b = std::move(i).base();
            ASSERT_SAME_TYPE(decltype(b), BaseIter const&);
        }

        {
            decltype(auto) b = std::move(ci).base();
            ASSERT_SAME_TYPE(decltype(b), BaseIter const&);
        }
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
