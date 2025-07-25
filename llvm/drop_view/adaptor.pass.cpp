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

// xviews::drop

#include "../test_iterators.h"
#include "../test_range.h"
#include "rxx/ranges.h"

#include <array>
#include <cassert>
#include <concepts>
#include <span>
#include <string_view>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct SizedView : xranges::view_base {
    int* begin_ = nullptr;
    int* end_ = nullptr;
    constexpr SizedView(int* begin, int* end) : begin_(begin), end_(end) {}

    constexpr auto begin() const { return forward_iterator<int*>(begin_); }
    constexpr auto end() const {
        return sized_sentinel<forward_iterator<int*>>(
            forward_iterator<int*>(end_));
    }
};
static_assert(xranges::forward_range<SizedView>);
static_assert(xranges::sized_range<SizedView>);
static_assert(xranges::view<SizedView>);

struct SizedViewWithUnsizedSentinel : xranges::view_base {
    using iterator = random_access_iterator<int*>;
    using sentinel = sentinel_wrapper<random_access_iterator<int*>>;

    int* begin_ = nullptr;
    int* end_ = nullptr;
    constexpr SizedViewWithUnsizedSentinel(int* begin, int* end)
        : begin_(begin)
        , end_(end) {}

    constexpr auto begin() const { return iterator(begin_); }
    constexpr auto end() const { return sentinel(iterator(end_)); }
    constexpr std::size_t size() const { return end_ - begin_; }
};
static_assert(xranges::random_access_range<SizedViewWithUnsizedSentinel>);
static_assert(xranges::sized_range<SizedViewWithUnsizedSentinel>);
static_assert(!std::sized_sentinel_for<SizedViewWithUnsizedSentinel::sentinel,
              SizedViewWithUnsizedSentinel::iterator>);
static_assert(xranges::view<SizedViewWithUnsizedSentinel>);

template <class T>
constexpr void test_small_range(T const& input) {
    constexpr int N = 100;
    auto size = xranges::size(input);
    assert(size < N);

    auto result = input | xviews::drop(N);
    assert(result.empty());
}

constexpr bool test() {
    constexpr int N = 8;
    int buf[N] = {1, 2, 3, 4, 5, 6, 7, 8};

    // Test that `xviews::drop` is a range adaptor.
    {
        using SomeView = SizedView;

        // Test `view | views::drop`
        {
            SomeView view(buf, buf + N);
            std::same_as<xranges::drop_view<SomeView>> decltype(auto) result =
                view | xviews::drop(3);
            assert(result.base().begin_ == buf);
            assert(result.base().end_ == buf + N);
            assert(base(result.begin()) == buf + 3);
            assert(base(base(result.end())) == buf + N);
            assert(result.size() == 5);
        }

        // Test `adaptor | views::drop`
        {
            SomeView view(buf, buf + N);
            auto f = [](int i) { return i; };
            auto const partial = xviews::transform(f) | xviews::drop(3);

            using Result = xranges::drop_view<
                xranges::transform_view<SomeView, decltype(f)>>;
            std::same_as<Result> decltype(auto) result = partial(view);
            assert(result.base().base().begin_ == buf);
            assert(result.base().base().end_ == buf + N);
            assert(base(result.begin().base()) == buf + 3);
            assert(base(base(result.end().base())) == buf + N);
            assert(result.size() == 5);
        }

        // Test `views::drop | adaptor`
        {
            SomeView view(buf, buf + N);
            auto f = [](int i) { return i; };
            auto const partial = xviews::drop(3) | xviews::transform(f);

            using Result = xranges::transform_view<xranges::drop_view<SomeView>,
                decltype(f)>;
            std::same_as<Result> decltype(auto) result = partial(view);
            assert(result.base().base().begin_ == buf);
            assert(result.base().base().end_ == buf + N);
            assert(base(result.begin().base()) == buf + 3);
            assert(base(base(result.end().base())) == buf + N);
            assert(result.size() == 5);
        }

        // Check SFINAE friendliness
        {
            struct NotAView {};
            static_assert(!std::is_invocable_v<decltype(xviews::drop)>);
            static_assert(
                !std::is_invocable_v<decltype(xviews::drop), NotAView, int>);
            static_assert(CanBePiped<SomeView&, decltype(xviews::drop(3))>);
            static_assert(CanBePiped<int(&)[10], decltype(xviews::drop(3))>);
            static_assert(!CanBePiped<int(&&)[10], decltype(xviews::drop(3))>);
            static_assert(!CanBePiped<NotAView, decltype(xviews::drop(3))>);

            static_assert(!CanBePiped<SomeView&,
                          decltype(xviews::drop(/*n=*/NotAView{}))>);
        }
    }

    {
        static_assert(std::same_as<decltype(xviews::drop),
            decltype(xranges::views::drop)>);
    }

    // `views::drop(empty_view, n)` returns an `empty_view`.
    {
        using Result = xranges::empty_view<int>;
        [[maybe_unused]] std::same_as<Result> decltype(auto) result =
            xviews::empty<int> | xviews::drop(3);
    }

    // `views::drop(span, n)` returns a `span`.
    {
        std::span<int> s(buf);
        std::same_as<decltype(s)> decltype(auto) result = s | xviews::drop(5);
        assert(result.size() == 3);
    }

    // `views::drop(span, n)` returns a `span` with a dynamic extent, regardless
    // of the input `span`.
    {
        std::span<int, 8> s(buf);
        std::same_as<std::span<int, std::dynamic_extent>> decltype(auto)
            result = s | xviews::drop(3);
        assert(result.size() == 5);
    }

    // `views::drop(string_view, n)` returns a `string_view`.
    {
        {
            std::string_view sv = "abcdef";
            std::same_as<decltype(sv)> decltype(auto) result =
                sv | xviews::drop(2);
            assert(result.size() == 4);
        }

        {
            std::u32string_view sv = U"abcdef";
            std::same_as<decltype(sv)> decltype(auto) result =
                sv | xviews::drop(2);
            assert(result.size() == 4);
        }
    }

    // `views::drop(iota_view, n)` returns an `iota_view`.
    {
        auto iota = xviews::iota(1, 8);
        // The second template argument of the resulting `iota_view` is
        // different because it has to be able to hold the `range_difference_t`
        // of the input `iota_view`.
        using Result = xranges::iota_view<int, int>;
        std::same_as<Result> decltype(auto) result = iota | xviews::drop(3);
        assert(result.size() == 4);
        assert(*result.begin() == 4);
        assert(*xranges::next(result.begin(), 3) == 7);
    }

    // `views::drop(subrange, n)` returns a `subrange` when `subrange::StoreSize
    // == false`.
    {
        auto subrange = xranges::subrange(buf, buf + N);
#if RXX_LIBCXX
        static_assert(!decltype(subrange)::_StoreSize);
#endif
        using Result = xranges::subrange<int*>;
        std::same_as<Result> decltype(auto) result = subrange | xviews::drop(3);
        assert(result.size() == 5);
    }

    // `views::drop(subrange, n)` returns a `subrange` when `subrange::StoreSize
    // == true`.
    {
        using View = SizedViewWithUnsizedSentinel;
        View view(buf, buf + N);

        using Subrange = xranges::subrange<View::iterator, View::sentinel,
            xranges::subrange_kind::sized>;
        auto subrange = Subrange(view.begin(), view.end(),
            xranges::distance(view.begin(), view.end()));
#if RXX_LIBCXX
        static_assert(decltype(subrange)::_StoreSize);
#endif

        std::same_as<Subrange> decltype(auto) result =
            subrange | xviews::drop(3);
        assert(result.size() == 5);
    }

    // `views::drop(subrange, n)` doesn't return a `subrange` if it's not a
    // random access range.
    {
        SizedView v(buf, buf + N);
        auto subrange = xranges::subrange(v.begin(), v.end());

        using Result =
            xranges::drop_view<xranges::subrange<forward_iterator<int*>,
                sized_sentinel<forward_iterator<int*>>>>;
        std::same_as<Result> decltype(auto) result = subrange | xviews::drop(3);
        assert(result.size() == 5);
    }

    // When the size of the input range `s` is shorter than `n`, an `empty_view`
    // is returned.
    {
        test_small_range(std::span(buf));
        test_small_range(std::string_view("abcdef"));
        test_small_range(xranges::subrange(buf, buf + N));
        test_small_range(xviews::iota(1, 8));
    }

    // `views::drop(repeat_view, n)` returns a `repeat_view` when `repeat_view`
    // models `sized_range`.
    {
        auto repeat = xranges::repeat_view<int, int>(1, 8);
        using Result = xranges::repeat_view<int, int>;
        std::same_as<Result> decltype(auto) result = repeat | xviews::drop(3);
        static_assert(xranges::sized_range<Result>);
        assert(result.size() == 5);
        assert(*result.begin() == 1);
    }

    // `views::drop(repeat_view, n)` returns a `repeat_view` when `repeat_view`
    // doesn't model `sized_range`.
    {
        auto repeat = xranges::repeat_view<int>(1);
        using Result = xranges::repeat_view<int, std::unreachable_sentinel_t>;
        std::same_as<Result> decltype(auto) result = repeat | xviews::drop(3);
        static_assert(!xranges::sized_range<Result>);
        static_assert(
            std::same_as<std::unreachable_sentinel_t, decltype(result.end())>);
    }

    // Test that it's possible to call `xviews::drop` with any single
    // argument as long as the resulting closure is never invoked. There is no
    // good use case for it, but it's valid.
    {
        struct X {};
        [[maybe_unused]] auto partial = xviews::drop(X{});
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
