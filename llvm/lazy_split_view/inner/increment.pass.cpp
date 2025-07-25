// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// constexpr inner-iterator& inner-iterator::operator++();
//
// constexpr decltype(auto) inner-iterator::operator++(int);

#include "../types.h"
#include "rxx/ranges.h"

#include <cassert>
#include <type_traits>

struct EmptyView : xranges::view_base {
    constexpr int* begin() const { return nullptr; }
    constexpr int* end() const { return nullptr; }
    constexpr static std::size_t size() { return 0; }
};
static_assert(xranges::forward_range<EmptyView>);
static_assert(xranges::view<EmptyView>);
static_assert(xranges::details::tiny_range<EmptyView>);

constexpr bool test() {
    // Can call `inner-iterator::operator++`; `View` is a forward range.
    {
        SplitViewForward v("abc def", " ");
        auto val = *v.begin();

        // ++i
        {
            auto i = val.begin();
            assert(*i == 'a');

            decltype(auto) i2 = ++i;
            static_assert(std::is_lvalue_reference_v<decltype(i2)>);
            assert(&i2 == &i);
            assert(*i2 == 'b');
        }

        // i++
        {
            auto i = val.begin();
            assert(*i == 'a');

            decltype(auto) i2 = i++;
            static_assert(!std::is_reference_v<decltype(i2)>);
            assert(*i2 == 'a');
            assert(*i == 'b');
        }
    }

    // Can call `inner-iterator::operator++`; `View` is an input range.
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AFTER(2023, 11, 08)
    if (!std::is_constant_evaluated())
#endif
    {
        // ++i
        {
            SplitViewInput v("abc def", ' ');
            auto val = *v.begin();

            auto i = val.begin();
            assert(*i == 'a');

            decltype(auto) i2 = ++i;
            static_assert(std::is_lvalue_reference_v<decltype(i2)>);
            assert(&i2 == &i);
            assert(*i2 == 'b');
        }

        // i++
        {
            SplitViewInput v("abc def", ' ');
            auto val = *v.begin();

            auto i = val.begin();
            assert(*i == 'a');

            static_assert(std::is_void_v<decltype(i++)>);
            i++;
            assert(*i == 'b');
        }
    }

    // Can call `inner-iterator::operator++`; `View` is an input range and
    // `Pattern` is an "empty" range.
    {
        // ++i
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AFTER(2023, 11, 08)
        if (!std::is_constant_evaluated())
#endif
        {
            xranges::lazy_split_view<InputView, EmptyView> v("a", EmptyView());
            auto val = *v.begin();

            auto i = val.begin();
            assert(*i.base() == 'a');
            assert(i != std::default_sentinel);

            // The iterator doesn't move to the next character but is considered
            // to point to the end.
            decltype(auto) i2 = ++i;
            assert(&i2 == &i);
            assert(*i2.base() == 'a');
            assert(i2 == std::default_sentinel);
        }

        // i++
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AFTER(2023, 11, 08)
        if (!std::is_constant_evaluated())
#endif
        {
            xranges::lazy_split_view<InputView, EmptyView> v("a", EmptyView());
            auto val = *v.begin();

            auto i = val.begin();
            assert(*i.base() == 'a');
            assert(i != std::default_sentinel);

            // The iterator doesn't move to the next character but is considered
            // to point to the end.
            i++;
            assert(*i.base() == 'a');
            assert(i == std::default_sentinel);
        }
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
