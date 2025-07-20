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

// Iterator traits and member typedefs in join_view::<iterator>.

#include "../../test_iterators.h"
#include "../types.h"
#include "rxx/ranges/join_view.h"

#include <ranges>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
template <class T>
struct ForwardView : std::ranges::view_base {
    forward_iterator<T*> begin() const;
    sentinel_wrapper<forward_iterator<T*>> end() const;
};

template <class T>
struct InputView : std::ranges::view_base {
    cpp17_input_iterator<T*> begin() const;
    sentinel_wrapper<cpp17_input_iterator<T*>> end() const;
};

template <class T, class V>
struct diff_type_iter {
    using iterator_category = std::input_iterator_tag;
    using value_type = V;
    using difference_type = T;

    V& operator*() const;
    diff_type_iter& operator++();
    void operator++(int);
    friend constexpr bool operator==(diff_type_iter, diff_type_iter) = default;
};

template <class T, class V = int>
struct DiffTypeRange : std::ranges::view_base {
    diff_type_iter<T, V> begin() const;
    diff_type_iter<T, V> end() const;
};

template <class T>
concept HasIterCategory = requires { typename T::iterator_category; };

void test() {
    {
        int buffer[4][4];
        xranges::join_view jv(buffer);
        using Iter = xranges::iterator_t<decltype(jv)>;

        static_assert(std::is_same_v<Iter::iterator_concept,
            std::bidirectional_iterator_tag>);
        static_assert(std::is_same_v<Iter::iterator_category,
            std::bidirectional_iterator_tag>);
        static_assert(std::is_same_v<Iter::difference_type, std::ptrdiff_t>);
        static_assert(std::is_same_v<Iter::value_type, int>);
        static_assert(HasIterCategory<Iter>);
    }

    {
        using Iter = xranges::iterator_t<
            xranges::join_view<ForwardView<ForwardView<int>>>>;

        static_assert(
            std::is_same_v<Iter::iterator_concept, std::forward_iterator_tag>);
        static_assert(
            std::is_same_v<Iter::iterator_category, std::forward_iterator_tag>);
        static_assert(std::is_same_v<Iter::difference_type, std::ptrdiff_t>);
        static_assert(std::is_same_v<Iter::value_type, int>);
        static_assert(HasIterCategory<Iter>);
    }

    {
        using Iter =
            xranges::iterator_t<xranges::join_view<InputView<InputView<int>>>>;

        static_assert(
            std::is_same_v<Iter::iterator_concept, std::input_iterator_tag>);
        static_assert(!HasIterCategory<Iter>);
        static_assert(std::is_same_v<Iter::difference_type, std::ptrdiff_t>);
        static_assert(std::is_same_v<Iter::value_type, int>);
    }

    {
        // LWG3535 `join_view::iterator::iterator_category` and
        // `::iterator_concept` lie Bidi non common inner range should not have
        // bidirectional_iterator_tag
        using Base = BidiCommonOuter<BidiNonCommonInner>;
        using Iter = xranges::iterator_t<xranges::join_view<Base>>;
        static_assert(
            std::is_same_v<Iter::iterator_concept, std::forward_iterator_tag>);
        static_assert(
            std::is_same_v<Iter::iterator_category, std::forward_iterator_tag>);
        static_assert(HasIterCategory<Iter>);
        static_assert(std::is_same_v<Iter::difference_type, std::ptrdiff_t>);
        static_assert(std::is_same_v<Iter::value_type, int>);
    }

    {
        // !ref-is-glvalue
        using Outer = InnerRValue<BidiCommonOuter<BidiCommonInner>>;
        using Iter = xranges::iterator_t<xranges::join_view<Outer>>;
        static_assert(!HasIterCategory<Iter>);
        static_assert(
            std::is_same_v<Iter::iterator_concept, std::input_iterator_tag>);
    }

    {
        // value_type == inner's value_type
        using Inner = IterMoveSwapAwareView;
        using InnerValue = xranges::range_value_t<Inner>;
        using InnerReference = xranges::range_reference_t<Inner>;
        static_assert(
            !std::is_same_v<InnerValue, std::remove_cvref<InnerReference>>);

        using Outer = BidiCommonOuter<Inner>;
        using Iter = xranges::iterator_t<xranges::join_view<Outer>>;
        static_assert(std::is_same_v<InnerValue, std::pair<int, int>>);
        static_assert(std::is_same_v<Iter::value_type, std::pair<int, int>>);
    }

    {
        // difference_type
        using Inner = DiffTypeRange<std::intptr_t>;
        using Outer = DiffTypeRange<std::ptrdiff_t, Inner>;
        using Iter = xranges::iterator_t<xranges::join_view<Outer>>;
        static_assert(std::is_same_v<Iter::difference_type,
            std::common_type_t<std::intptr_t, std::ptrdiff_t>>);
    }
}
