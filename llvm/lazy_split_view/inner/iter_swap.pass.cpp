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

// friend constexpr void iter_swap(const inner-iterator& x, const
// inner-iterator& y)
//   noexcept(noexcept(ranges::iter_swap(x.i_.<current>, y.i_.<current>)))
//   requires indirectly_swappable<iterator_t<Base>>;

#include "../../static_asserts.h"
#include "../types.h"
#include "rxx/ranges.h"

#include <cassert>
#include <type_traits>
#include <utility>

namespace adl {

template <bool IsNoexcept = false>
struct MaybeNoexceptIterator {
    using value_type = int;
    using difference_type = std::ptrdiff_t;

    value_type* ptr_ = nullptr;
    int* iter_swap_invocations_ = nullptr;

    constexpr MaybeNoexceptIterator() = default;
    constexpr explicit MaybeNoexceptIterator(int& iter_swaps)
        : iter_swap_invocations_(&iter_swaps) {}

    value_type& operator*() const { return *ptr_; }

    MaybeNoexceptIterator& operator++() {
        ++ptr_;
        return *this;
    }
    MaybeNoexceptIterator operator++(int) {
        MaybeNoexceptIterator prev = *this;
        ++ptr_;
        return prev;
    }

    MaybeNoexceptIterator& operator--() {
        --ptr_;
        return *this;
    }
    MaybeNoexceptIterator operator--(int) {
        MaybeNoexceptIterator prev = *this;
        --ptr_;
        return prev;
    }

    constexpr friend void iter_swap(
        MaybeNoexceptIterator a, MaybeNoexceptIterator) noexcept(IsNoexcept) {
        if (a.iter_swap_invocations_) {
            ++(*a.iter_swap_invocations_);
        }
    }

    friend bool operator==(
        MaybeNoexceptIterator const& lhs, MaybeNoexceptIterator const& rhs) {
        return lhs.ptr_ == rhs.ptr_;
    }
};

template <bool IsNoexcept = false>
struct View : xranges::view_base {
    int* iter_swaps = nullptr;

    constexpr View() = default;
    constexpr View(int& iter_swap_invocations)
        : iter_swaps(&iter_swap_invocations) {}

    constexpr adl::MaybeNoexceptIterator<IsNoexcept> begin() {
        return adl::MaybeNoexceptIterator<IsNoexcept>(*iter_swaps);
    }
    constexpr adl::MaybeNoexceptIterator<IsNoexcept> end() {
        return adl::MaybeNoexceptIterator<IsNoexcept>(*iter_swaps);
    }
};

} // namespace adl

constexpr bool test() {
    // Can use `iter_swap` with `inner-iterator`; `View` is a forward range.
    {
        // Non-const iterator.
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AFTER(2023, 11, 08)
        if (!std::is_constant_evaluated())
#endif
        {
            SplitViewDiff v("abc def", " ");
            auto segment = *v.begin();

            auto i1 = segment.begin();
            auto i2 = i1++;
            static_assert(std::is_void_v<decltype(iter_swap(i1, i2))>);
            assert(*i1 == 'b');
            assert(*i2 == 'a');

            iter_swap(i1, i2);
            assert(*i1 == 'a');
            assert(*i2 == 'b');
            // Note that `iter_swap` swaps characters in the actual underlying
            // range.
            assert(*v.base().begin() == 'b');
        }

        // Const iterator.
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AFTER(2023, 11, 08)
        if (!std::is_constant_evaluated())
#endif
        {
            SplitViewDiff v("abc def", " ");
            auto segment = *v.begin();

            auto i1 = segment.begin();
            auto const i2 = i1++;
            static_assert(std::is_void_v<decltype(iter_swap(i1, i2))>);
            static_assert(std::is_void_v<decltype(iter_swap(i2, i2))>);
            assert(*i1 == 'b');
            assert(*i2 == 'a');

            iter_swap(i1, i2);
            assert(*i1 == 'a');
            assert(*i2 == 'b');
            assert(*v.base().begin() == 'b');
        }
    }

// Can use `iter_swap` with `inner-iterator`; `View` is an input range.
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AFTER(2023, 11, 08)
    if (!std::is_constant_evaluated())
#endif
    {

        // Non-const iterator.
        {
            // Iterators belong to the same view.
            {
                SplitViewInput v("abc def", ' ');
                auto segment = *v.begin();

                auto i1 = segment.begin();
                auto i2 = i1;
                ++i1;
                static_assert(std::is_void_v<decltype(iter_swap(i1, i2))>);
                assert(*i1 == 'b');
                // For an input view, all inner iterators are essentially thin
                // proxies to the same underlying iterator.
                assert(*i2 == 'b');

                iter_swap(i1, i2);
                assert(*i1 == 'b');
                assert(*i2 == 'b');
            }

            // Iterators belong to different views.
            {
                SplitViewInput v1("abc def", ' ');
                auto val1 = *v1.begin();
                SplitViewInput v2 = v1;
                auto val2 = *v2.begin();

                auto i1 = val1.begin();
                auto i2 = val2.begin();
                ++i1;
                assert(*i1 == 'b');
                assert(*i2 == 'a');

                iter_swap(i1, i2);
                assert(*i1 == 'a');
                assert(*i2 == 'b');
            }
        }

        // Const iterator.
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AFTER(2023, 11, 08)
        if (!std::is_constant_evaluated())
#endif
        {
            SplitViewInput v("abc def", ' ');
            auto segment = *v.begin();

            auto const i1 = segment.begin();
            auto const i2 = i1;
            static_assert(std::is_void_v<decltype(iter_swap(i1, i2))>);
            assert(*i1 == 'a');
            assert(*i2 == 'a');

            iter_swap(i1, i2);
            assert(*i1 == 'a');
            assert(*i2 == 'a');
        }
    }

    // Ensure the `iter_swap` customization point is being used.
    {
        int iter_swap_invocations = 0;
        adl::View<> input(iter_swap_invocations);
        xranges::lazy_split_view<adl::View<>, adl::View<>> v(
            input, adl::View<>());

        auto segment = *v.begin();
        auto i = segment.begin();
        iter_swap(i, i);
        assert(iter_swap_invocations == 1);
    }

    // Check the `noexcept` specification.
    {
        {
            using ThrowingSplitView =
                xranges::lazy_split_view<adl::View<false>, adl::View<false>>;
            using ThrowingValueType =
                xranges::iterator_t<ThrowingSplitView>::value_type;
            using ThrowingIter = xranges::iterator_t<ThrowingValueType>;
            ASSERT_NOT_NOEXCEPT(xranges::iter_swap(
                std::declval<adl::MaybeNoexceptIterator<false>>(),
                std::declval<adl::MaybeNoexceptIterator<false>>()));
            ASSERT_NOT_NOEXCEPT(iter_swap(
                std::declval<ThrowingIter>(), std::declval<ThrowingIter>()));
        }

        {
            using NoexceptSplitView =
                xranges::lazy_split_view<adl::View<true>, adl::View<true>>;
            using NoexceptValueType =
                xranges::iterator_t<NoexceptSplitView>::value_type;
            using NoexceptIter = xranges::iterator_t<NoexceptValueType>;
            ASSERT_NOEXCEPT(xranges::iter_swap(
                std::declval<adl::MaybeNoexceptIterator<true>>(),
                std::declval<adl::MaybeNoexceptIterator<true>>()));
            ASSERT_NOEXCEPT(iter_swap(
                std::declval<NoexceptIter>(), std::declval<NoexceptIter>()));
        }
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
