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

// xviews::join

#include "../static_asserts.h"
#include "../test_range.h"
#include "rxx/ranges.h"
#include "types.h"

#include <cassert>
#include <type_traits>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
struct MoveOnlyOuter : SimpleForwardCommonOuter<ForwardCommonInner> {
    using SimpleForwardCommonOuter<
        ForwardCommonInner>::SimpleForwardCommonOuter;

    constexpr MoveOnlyOuter(MoveOnlyOuter&&) = default;
    constexpr MoveOnlyOuter(MoveOnlyOuter const&) = delete;

    constexpr MoveOnlyOuter& operator=(MoveOnlyOuter&&) = default;
    constexpr MoveOnlyOuter& operator=(MoveOnlyOuter const&) = delete;
};

struct Foo {
    int i;
    constexpr Foo(int ii) : i(ii) {}
};

constexpr bool test() {
    int buffer1[3] = {1, 2, 3};
    int buffer2[2] = {4, 5};
    int buffer3[4] = {6, 7, 8, 9};
    Foo nested[2][3][3] = {
        {   {1, 2, 3},    {4, 5, 6},    {7, 8, 9}},
        {{10, 11, 12}, {13, 14, 15}, {16, 17, 18}}
    };

    {
        // Test `views::join(v)`
        ForwardCommonInner inners[3] = {buffer1, buffer2, buffer3};
        using Result =
            xranges::join_view<xranges::ref_view<ForwardCommonInner[3]>>;
        std::same_as<Result> decltype(auto) v = xviews::join(inners);
        assert(xranges::next(v.begin(), 9) == v.end());
        assert(&(*v.begin()) == buffer1);
    }

    {
        // Test `views::join(move-only-view)`
        ForwardCommonInner inners[3] = {buffer1, buffer2, buffer3};
        using Result = xranges::join_view<MoveOnlyOuter>;
        std::same_as<Result> decltype(auto) v =
            xviews::join(MoveOnlyOuter{inners});
        assert(xranges::next(v.begin(), 9) == v.end());
        assert(&(*v.begin()) == buffer1);

        static_assert(std::invocable<decltype(xviews::join), MoveOnlyOuter>);
        static_assert(!std::invocable<decltype(xviews::join), MoveOnlyOuter&>);
    }

    {
        // LWG3474 Nesting `join_views` is broken because of CTAD
        // views::join(join_view) should join the view instead of calling copy
        // constructor
        auto jv = xviews::join(nested);
        ASSERT_SAME_TYPE(xranges::range_reference_t<decltype(jv)>, Foo(&)[3]);

        auto jv2 = xviews::join(jv);
        ASSERT_SAME_TYPE(xranges::range_reference_t<decltype(jv2)>, Foo&);

        assert(&(*jv2.begin()) == &nested[0][0][0]);
    }

    {
        // Test `v | views::join`
        ForwardCommonInner inners[3] = {buffer1, buffer2, buffer3};

        using Result =
            xranges::join_view<xranges::ref_view<ForwardCommonInner[3]>>;
        std::same_as<Result> decltype(auto) v = inners | xviews::join;
        assert(xranges::next(v.begin(), 9) == v.end());
        assert(&(*v.begin()) == buffer1);
        static_assert(CanBePiped<decltype((inners)), decltype((xviews::join))>);
    }

    {
        // Test `move-only-view | views::join`
        ForwardCommonInner inners[3] = {buffer1, buffer2, buffer3};
        using Result = xranges::join_view<MoveOnlyOuter>;
        std::same_as<Result> decltype(auto) v =
            MoveOnlyOuter{inners} | xviews::join;
        assert(xranges::next(v.begin(), 9) == v.end());
        assert(&(*v.begin()) == buffer1);

        static_assert(CanBePiped<MoveOnlyOuter, decltype((xviews::join))>);
        static_assert(!CanBePiped<MoveOnlyOuter&, decltype((xviews::join))>);
    }

    {
        // LWG3474 Nesting `join_views` is broken because of CTAD
        // join_view | views::join should join the view instead of calling copy
        // constructor
        auto jv = nested | xviews::join | xviews::join;
        ASSERT_SAME_TYPE(xranges::range_reference_t<decltype(jv)>, Foo&);

        assert(&(*jv.begin()) == &nested[0][0][0]);
        static_assert(CanBePiped<decltype((nested)), decltype((xviews::join))>);
    }

    {
        // Test `adaptor | views::join`
        auto join_twice = xviews::join | xviews::join;
        auto jv = nested | join_twice;
        ASSERT_SAME_TYPE(xranges::range_reference_t<decltype(jv)>, Foo&);

        assert(&(*jv.begin()) == &nested[0][0][0]);
        static_assert(CanBePiped<decltype((nested)), decltype((join_twice))>);
    }

    {
        static_assert(!CanBePiped<int, decltype((xviews::join))>);
        static_assert(!CanBePiped<Foo, decltype((xviews::join))>);
        static_assert(!CanBePiped<int(&)[2], decltype((xviews::join))>);
        static_assert(CanBePiped<int(&)[2][2], decltype((xviews::join))>);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
