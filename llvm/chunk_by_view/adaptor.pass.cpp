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

// <ranges>

// xviews::chunk_by

#include "../test_iterators.h"
#include "../test_range.h"
#include "rxx/ranges/chunk_by_view.h"
#include "rxx/ranges/filter_view.h"
#include "rxx/ranges/view_base.h"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct Pred {
    constexpr bool operator()(int x, int y) const { return x != -y; }
};

struct NonCopyablePredicate : Pred {
    NonCopyablePredicate(NonCopyablePredicate const&) = delete;
};

struct Range : xranges::view_base {
    using Iterator = forward_iterator<int*>;
    using Sentinel = sentinel_wrapper<Iterator>;
    constexpr explicit Range(int* b, int* e) : begin_(b), end_(e) {}
    constexpr Iterator begin() const { return Iterator(begin_); }
    constexpr Sentinel end() const { return Sentinel(Iterator(end_)); }

private:
    int* begin_;
    int* end_;
};

template <typename View>
constexpr void compareViews(
    View v, std::initializer_list<std::initializer_list<int>> list) {
    auto b1 = v.begin();
    auto e1 = v.end();
    auto b2 = list.begin();
    auto e2 = list.end();
    for (; b1 != e1 && b2 != e2; ++b1, ++b2) {
        bool eq = xranges::equal(*b1, *b2, [](int x, int y) {
            assert(x == y);
            return true;
        });
        assert(eq);
    }
    assert(b1 == e1);
    assert(b2 == e2);
}

constexpr int absoluteValue(int x) {
    return x < 0 ? -x : x;
}

template <class T>
constexpr T const&& asConstRvalue(T&& t) {
    return static_cast<T const&&>(t);
}

constexpr bool test() {
    int buff[] = {-4, -3, -2, -1, 1, 2, 3, 4};

    // Test range adaptor object
    {
        using RangeAdaptorObject = decltype(xviews::chunk_by);
        static_assert(std::is_const_v<RangeAdaptorObject>);

        // The type of a customization point object, ignoring cv-qualifiers,
        // shall model semiregular
        static_assert(std::semiregular<std::remove_const<RangeAdaptorObject>>);
    }

    // Test `views::chunk_by(pred)(v)`
    {
        using Result = xranges::chunk_by_view<Range, Pred>;
        Range const range(buff, buff + 8);
        Pred pred;

        {
            // 'views::chunk_by(pred)' - &&
            std::same_as<Result> decltype(auto) result =
                xviews::chunk_by(pred)(range);
            compareViews(result,
                {
                    {-4, -3, -2, -1},
                    { 1,  2,  3,  4}
            });
        }
        {
            // 'views::chunk_by(pred)' - const&&
            std::same_as<Result> decltype(auto) result =
                asConstRvalue(xviews::chunk_by(pred))(range);
            compareViews(result,
                {
                    {-4, -3, -2, -1},
                    { 1,  2,  3,  4}
            });
        }
        {
            // 'views::chunk_by(pred)' - &
            auto partial = xviews::chunk_by(pred);
            std::same_as<Result> decltype(auto) result = partial(range);
            compareViews(result,
                {
                    {-4, -3, -2, -1},
                    { 1,  2,  3,  4}
            });
        }
        {
            // 'views::chunk_by(pred)' - const&
            auto const partial = xviews::chunk_by(pred);
            std::same_as<Result> decltype(auto) result = partial(range);
            compareViews(result,
                {
                    {-4, -3, -2, -1},
                    { 1,  2,  3,  4}
            });
        }
    }

    // Test `v | views::chunk_by(pred)`
    {
        using Result = xranges::chunk_by_view<Range, Pred>;
        Range const range(buff, buff + 8);
        Pred pred;

        {
            // 'views::chunk_by(pred)' - &&
            std::same_as<Result> decltype(auto) result =
                range | xviews::chunk_by(pred);
            compareViews(result,
                {
                    {-4, -3, -2, -1},
                    { 1,  2,  3,  4}
            });
        }
        {
            // 'views::chunk_by(pred)' - const&&
            std::same_as<Result> decltype(auto) result =
                range | asConstRvalue(xviews::chunk_by(pred));
            compareViews(result,
                {
                    {-4, -3, -2, -1},
                    { 1,  2,  3,  4}
            });
        }
        {
            // 'views::chunk_by(pred)' - &
            auto partial = xviews::chunk_by(pred);
            std::same_as<Result> decltype(auto) result = range | partial;
            compareViews(result,
                {
                    {-4, -3, -2, -1},
                    { 1,  2,  3,  4}
            });
        }
        {
            // 'views::chunk_by(pred)' - const&
            auto const partial = xviews::chunk_by(pred);
            std::same_as<Result> decltype(auto) result = range | partial;
            compareViews(result,
                {
                    {-4, -3, -2, -1},
                    { 1,  2,  3,  4}
            });
        }
    }

    // Test `views::chunk_by(v, pred)` range adaptor object
    {
        using Result = xranges::chunk_by_view<Range, Pred>;
        Range const range(buff, buff + 8);
        Pred pred;

        {
            // 'views::chunk_by' - &&
            auto range_adaptor = xviews::chunk_by;
            std::same_as<Result> decltype(auto) result =
                std::move(range_adaptor)(range, pred);
            compareViews(result,
                {
                    {-4, -3, -2, -1},
                    { 1,  2,  3,  4}
            });
        }
        {
            // 'views::chunk_by' - const&&
            auto const range_adaptor = xviews::chunk_by;
            std::same_as<Result> decltype(auto) result =
                std::move(range_adaptor)(range, pred);
            compareViews(result,
                {
                    {-4, -3, -2, -1},
                    { 1,  2,  3,  4}
            });
        }
        {
            // 'views::chunk_by' - &
            auto range_adaptor = xviews::chunk_by;
            std::same_as<Result> decltype(auto) result =
                range_adaptor(range, pred);
            compareViews(result,
                {
                    {-4, -3, -2, -1},
                    { 1,  2,  3,  4}
            });
        }
        {
            // 'views::chunk_by' - const&
            auto const range_adaptor = xviews::chunk_by;
            std::same_as<Result> decltype(auto) result =
                range_adaptor(range, pred);
            compareViews(result,
                {
                    {-4, -3, -2, -1},
                    { 1,  2,  3,  4}
            });
        }
    }

    // Test that one can call xviews::chunk_by with arbitrary stuff, as long
    // as we don't try to actually complete the call by passing it a range.
    //
    // That makes no sense and we can't do anything with the result, but it's
    // valid.
    {
        int array[3] = {1, 2, 3};
        [[maybe_unused]] auto partial = xviews::chunk_by(std::move(array));
    }

    // Test `adaptor | views::chunk_by(pred)`
    {
        Range const range(buff, buff + 8);

        {
            auto pred1 = [](int i) { return absoluteValue(i) < 3; };
            Pred pred2;
            using Result = xranges::chunk_by_view<
                xranges::filter_view<Range, decltype(pred1)>, Pred>;
            std::same_as<Result> decltype(auto) result =
                range | xviews::filter(pred1) | xviews::chunk_by(pred2);
            compareViews(result,
                {
                    {-2, -1},
                    { 1,  2}
            });
        }
        {
            auto pred1 = [](int i) { return absoluteValue(i) < 3; };
            Pred pred2;
            using Result = xranges::chunk_by_view<
                xranges::filter_view<Range, decltype(pred1)>, Pred>;
            auto const partial =
                xviews::filter(pred1) | xviews::chunk_by(pred2);
            std::same_as<Result> decltype(auto) result = range | partial;
            compareViews(result,
                {
                    {-2, -1},
                    { 1,  2}
            });
        }
    }

    // Test SFINAE friendliness
    {
        struct NotAView {};
        struct NotInvocable {};

        static_assert(!CanBePiped<Range, decltype(xviews::chunk_by)>);
        static_assert(CanBePiped<Range, decltype(xviews::chunk_by(Pred{}))>);
        static_assert(
            !CanBePiped<NotAView, decltype(xviews::chunk_by(Pred{}))>);
        static_assert(!CanBePiped<std::initializer_list<int>,
                      decltype(xviews::chunk_by(Pred{}))>);
        static_assert(
            !CanBePiped<Range, decltype(xviews::chunk_by(NotInvocable{}))>);

        static_assert(!std::is_invocable_v<decltype(xviews::chunk_by)>);
        static_assert(
            !std::is_invocable_v<decltype(xviews::chunk_by), Pred, Range>);
        static_assert(
            std::is_invocable_v<decltype(xviews::chunk_by), Range, Pred>);
        static_assert(!std::is_invocable_v<decltype(xviews::chunk_by), Range,
                      Pred, Pred>);
        static_assert(!std::is_invocable_v<decltype(xviews::chunk_by),
                      NonCopyablePredicate>);
    }

    {
        static_assert(std::is_same_v<decltype(xviews::chunk_by),
            decltype(xviews::chunk_by)>);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
