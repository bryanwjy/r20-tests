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

// constexpr auto end();
// constexpr auto end() const;
//   requires forward_range<const V> &&
//            is_reference_v<range_reference_t<const V>> &&
//            input_range<range_reference_t<const V>>

#include "rxx/ranges.h"
#include "types.h"

#include <cassert>
#include <ranges>
#include <type_traits>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

template <class T>
concept HasConstEnd = requires(T const& t) { t.end(); };

// | ID | outer  | outer   | outer  | inner | inner   | inner  |     end()     |
// end()     | |    | simple | forward | common | l_ref | forward | common | |
// const     |
// |----|--------|---------|--------|-------|---------|--------|---------------|--------------|
// | 1  |   Y    |   Y     |   Y    |   Y   |    Y    |   Y    |iterator<true>
// |iterator<true>| | 2  |   Y    |   Y     |   Y    |   Y   |    Y    |   N
// |sentinel<true> |sentinel<true>| | 3  |   Y    |   Y     |   Y    |   Y   |
// N    |   Y    |sentinel<true> |sentinel<true>| | 4  |   Y    |   Y     |   Y
// |   N   |    Y    |   Y    |sentinel<true> |      -       | | 5  |   Y    |
// Y     |   N    |   Y   |    Y    |   Y    |sentinel<true> |sentinel<true>| |
// 6  |   Y    |   N     |   Y    |   Y   |    Y    |   Y    |sentinel<true> |
// -       | | 7  |   N    |   Y     |   Y    |   Y   |    Y    |   Y
// |iterator<false>|iterator<true>| | 8  |   N    |   Y     |   Y    |   Y   |
// Y    |   N    |sentinel<false>|sentinel<true>| | 9  |   N    |   Y     |   Y
// |   Y   |    N    |   Y    |sentinel<false>|sentinel<true>| | 10 |   N    |
// Y     |   Y    |   N   |    Y    |   Y    |sentinel<false>|      -       | |
// 11 |   N    |   Y     |   N    |   Y   |    Y    |   Y
// |sentinel<false>|sentinel<true>| | 12 |   N    |   N     |   Y    |   Y   |
// Y    |   Y    |sentinel<false>|      -       |
//
//

struct ConstNotRange : xranges::view_base {
    ChildView const* begin();
    ChildView const* end();
};

constexpr bool test() {
    int buffer[4][4] = {
        {1111, 2222, 3333, 4444},
        { 555,  666,  777,  888},
        {  99, 1010, 1111, 1212},
        {  13,   14,   15,   16}
    };

    {
        // test ID 1
        ForwardCommonInner inners[4] = {
            buffer[0], buffer[1], buffer[2], buffer[3]};
        SimpleForwardCommonOuter<ForwardCommonInner> outer{inners};

        xranges::join_view jv(outer);
        assert(jv.end() == xranges::next(jv.begin(), 16));
        assert(std::as_const(jv).end() ==
            xranges::next(std::as_const(jv).begin(), 16));

        static_assert(HasConstEnd<decltype(jv)>);
        static_assert(std::same_as<decltype(jv.end()),
            decltype(std::as_const(jv).end())>);
        static_assert(xranges::common_range<decltype(jv)>);
        static_assert(xranges::common_range<const decltype(jv)>);
    }

    {
        // test ID 2
        ForwardNonCommonInner inners[3] = {buffer[0], buffer[1], buffer[2]};
        SimpleForwardCommonOuter<ForwardNonCommonInner> outer{inners};

        xranges::join_view jv(outer);
        assert(jv.end() == xranges::next(jv.begin(), 12));
        assert(std::as_const(jv).end() ==
            xranges::next(std::as_const(jv).begin(), 12));

        static_assert(HasConstEnd<decltype(jv)>);
        static_assert(std::same_as<decltype(jv.end()),
            decltype(std::as_const(jv).end())>);
        static_assert(!xranges::common_range<decltype(jv)>);
        static_assert(!xranges::common_range<const decltype(jv)>);
    }

    {
        // test ID 3
        InputCommonInner inners[3] = {buffer[0], buffer[1], buffer[2]};
        SimpleForwardCommonOuter<InputCommonInner> outer{inners};

        xranges::join_view jv(outer);
        assert(jv.end() == xranges::next(jv.begin(), 12));
        assert(std::as_const(jv).end() ==
            xranges::next(std::as_const(jv).begin(), 12));

        static_assert(HasConstEnd<decltype(jv)>);
        static_assert(std::same_as<decltype(jv.end()),
            decltype(std::as_const(jv).end())>);
        static_assert(!xranges::common_range<decltype(jv)>);
        static_assert(!xranges::common_range<const decltype(jv)>);
    }

    {
        // test ID 4
        ForwardCommonInner inners[2] = {buffer[0], buffer[1]};
        InnerRValue<SimpleForwardCommonOuter<ForwardCommonInner>> outer{inners};

        xranges::join_view jv(outer);
        assert(jv.end() == xranges::next(jv.begin(), 8));

        static_assert(!HasConstEnd<decltype(jv)>);
        static_assert(!xranges::common_range<decltype(jv)>);
        static_assert(!xranges::common_range<const decltype(jv)>);
    }

    {
        // test ID 5
        ForwardCommonInner inners[4] = {
            buffer[0], buffer[1], buffer[2], buffer[3]};
        SimpleForwardNonCommonOuter<ForwardCommonInner> outer{inners};

        xranges::join_view jv(outer);
        assert(jv.end() == xranges::next(jv.begin(), 16));
        assert(std::as_const(jv).end() ==
            xranges::next(std::as_const(jv).begin(), 16));

        static_assert(HasConstEnd<decltype(jv)>);
        static_assert(std::same_as<decltype(jv.end()),
            decltype(std::as_const(jv).end())>);
        static_assert(!xranges::common_range<decltype(jv)>);
        static_assert(!xranges::common_range<const decltype(jv)>);
    }

    {
        // test ID 6
        ForwardCommonInner inners[4] = {
            buffer[0], buffer[1], buffer[2], buffer[3]};
        SimpleInputCommonOuter<ForwardCommonInner> outer{inners};

        xranges::join_view jv(outer);
        assert(jv.end() == xranges::next(jv.begin(), 16));

        static_assert(!HasConstEnd<decltype(jv)>);
        static_assert(!xranges::common_range<decltype(jv)>);
        static_assert(!xranges::common_range<const decltype(jv)>);
    }

    {
        // test ID 7
        ForwardCommonInner inners[1] = {buffer[0]};
        NonSimpleForwardCommonOuter<ForwardCommonInner> outer{inners};

        xranges::join_view jv(outer);
        assert(jv.end() == xranges::next(jv.begin(), 4));
        assert(std::as_const(jv).end() ==
            xranges::next(std::as_const(jv).begin(), 4));

        static_assert(HasConstEnd<decltype(jv)>);
        static_assert(!std::same_as<decltype(jv.end()),
                      decltype(std::as_const(jv).end())>);
        static_assert(xranges::common_range<decltype(jv)>);
        static_assert(xranges::common_range<const decltype(jv)>);
    }

    {
        // test ID 8
        ForwardNonCommonInner inners[3] = {buffer[0], buffer[1], buffer[2]};
        NonSimpleForwardCommonOuter<ForwardNonCommonInner> outer{inners};

        xranges::join_view jv(outer);
        assert(jv.end() == xranges::next(jv.begin(), 12));
        assert(std::as_const(jv).end() ==
            xranges::next(std::as_const(jv).begin(), 12));

        static_assert(HasConstEnd<decltype(jv)>);
        static_assert(!std::same_as<decltype(jv.end()),
                      decltype(std::as_const(jv).end())>);
        static_assert(!xranges::common_range<decltype(jv)>);
        static_assert(!xranges::common_range<const decltype(jv)>);
    }

    {
        // test ID 9
        InputCommonInner inners[3] = {buffer[0], buffer[1], buffer[2]};
        NonSimpleForwardCommonOuter<InputCommonInner> outer{inners};

        xranges::join_view jv(outer);
        assert(jv.end() == xranges::next(jv.begin(), 12));
        assert(std::as_const(jv).end() ==
            xranges::next(std::as_const(jv).begin(), 12));

        static_assert(HasConstEnd<decltype(jv)>);
        static_assert(!std::same_as<decltype(jv.end()),
                      decltype(std::as_const(jv).end())>);
        static_assert(!xranges::common_range<decltype(jv)>);
        static_assert(!xranges::common_range<const decltype(jv)>);
    }

    {
        // test ID 10
        ForwardCommonInner inners[2] = {buffer[0], buffer[1]};
        InnerRValue<NonSimpleForwardCommonOuter<ForwardCommonInner>> outer{
            inners};

        xranges::join_view jv(outer);
        assert(jv.end() == xranges::next(jv.begin(), 8));

        static_assert(!HasConstEnd<decltype(jv)>);
        static_assert(!xranges::common_range<decltype(jv)>);
        static_assert(!xranges::common_range<const decltype(jv)>);
    }

    {
        // test ID 11
        ForwardCommonInner inners[4] = {
            buffer[0], buffer[1], buffer[2], buffer[3]};
        NonSimpleForwardNonCommonOuter<ForwardCommonInner> outer{inners};

        xranges::join_view jv(outer);
        assert(jv.end() == xranges::next(jv.begin(), 16));
        assert(std::as_const(jv).end() ==
            xranges::next(std::as_const(jv).begin(), 16));

        static_assert(HasConstEnd<decltype(jv)>);
        static_assert(!std::same_as<decltype(jv.end()),
                      decltype(std::as_const(jv).end())>);
        static_assert(!xranges::common_range<decltype(jv)>);
        static_assert(!xranges::common_range<const decltype(jv)>);
    }

    {
        // test ID 12
        ForwardCommonInner inners[4] = {
            buffer[0], buffer[1], buffer[2], buffer[3]};
        NonSimpleInputCommonOuter<ForwardCommonInner> outer{inners};

        xranges::join_view jv(outer);
        assert(jv.end() == xranges::next(jv.begin(), 16));

        static_assert(!HasConstEnd<decltype(jv)>);
        static_assert(!xranges::common_range<decltype(jv)>);
        static_assert(!xranges::common_range<const decltype(jv)>);
    }

    {
        xranges::join_view jv(ConstNotRange{});
        static_assert(!HasConstEnd<decltype(jv)>);
    }

    // Has some empty children.
    {
        CopyableChild children[4] = {CopyableChild(buffer[0], 4),
            CopyableChild(buffer[1], 0), CopyableChild(buffer[2], 1),
            CopyableChild(buffer[3], 0)};
        auto jv = xranges::join_view(ParentView(children));
        assert(jv.end() == xranges::next(jv.begin(), 5));
    }

    // Parent is empty.
    {
        CopyableChild children[4] = {CopyableChild(buffer[0]),
            CopyableChild(buffer[1]), CopyableChild(buffer[2]),
            CopyableChild(buffer[3])};
        xranges::join_view jv(ParentView(children, 0));
        assert(jv.end() == jv.begin());
    }

    // Parent size is one.
    {
        CopyableChild children[1] = {CopyableChild(buffer[0])};
        xranges::join_view jv(ParentView(children, 1));
        assert(jv.end() == xranges::next(jv.begin(), 4));
    }

    // Parent and child size is one.
    {
        CopyableChild children[1] = {CopyableChild(buffer[0], 1)};
        xranges::join_view jv(ParentView(children, 1));
        assert(jv.end() == xranges::next(jv.begin()));
    }

    // Parent size is one child is empty
    {
        CopyableChild children[1] = {CopyableChild(buffer[0], 0)};
        xranges::join_view jv(ParentView(children, 1));
        assert(jv.end() == jv.begin());
    }

    // Has all empty children.
    {
        CopyableChild children[4] = {CopyableChild(buffer[0], 0),
            CopyableChild(buffer[1], 0), CopyableChild(buffer[2], 0),
            CopyableChild(buffer[3], 0)};
        auto jv = xranges::join_view(ParentView(children));
        assert(jv.end() == jv.begin());
    }

    // First child is empty, others are not.
    {
        CopyableChild children[4] = {CopyableChild(buffer[0], 4),
            CopyableChild(buffer[1], 0), CopyableChild(buffer[2], 0),
            CopyableChild(buffer[3], 0)};
        auto jv = xranges::join_view(ParentView(children));
        assert(jv.end() == xranges::next(jv.begin(), 4));
    }

    // Last child is empty, others are not.
    {
        CopyableChild children[4] = {CopyableChild(buffer[0], 4),
            CopyableChild(buffer[1], 4), CopyableChild(buffer[2], 4),
            CopyableChild(buffer[3], 0)};
        auto jv = xranges::join_view(ParentView(children));
        assert(jv.end() == xranges::next(jv.begin(), 12));
    }

    // LWG3700: The `const begin` of the `join_view` family does not require
    // `InnerRng` to be a range
    {
        xranges::join_view<ConstNonJoinableRange> jv;
        static_assert(!HasConstEnd<decltype(jv)>);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
