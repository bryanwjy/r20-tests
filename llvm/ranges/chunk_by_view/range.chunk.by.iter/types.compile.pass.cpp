// Copyright 2025-2026 Bryan Wong
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

// xranges::chunk_by_view::<iterator>::value_type
// xranges::chunk_by_view::<iterator>::difference_type
// xranges::chunk_by_view::<iterator>::iterator_category
// xranges::chunk_by_view::<iterator>::iterator_concept

#include "../types.h"

#include "../../../test_iterators.h"
#include "rxx/functional.h"
#include "rxx/ranges/chunk_by_view.h"

#include <functional>
#include <type_traits>

namespace xranges = __RXX ranges;
namespace xviews = __RXX views;

struct TestValueTypeAndDifferenceType {
    template <class Iter>
    constexpr void operator()() const {
        using Underlying = View<Iter>;
        using ChunkByView =
            xranges::chunk_by_view<Underlying, xranges::less_equal>;
        using ChunkByIterator = xranges::iterator_t<ChunkByView>;
        static_assert(std::same_as<typename ChunkByIterator::value_type,
            xranges::range_value_t<ChunkByView>>);
        static_assert(std::same_as<typename ChunkByIterator::value_type,
            xranges::subrange<Iter>>);
        static_assert(std::same_as<typename ChunkByIterator::difference_type,
            xranges::range_difference_t<ChunkByView>>);
        static_assert(std::same_as<typename ChunkByIterator::difference_type,
            xranges::range_difference_t<Underlying>>);
    }
};

template <class Iter>
using ChunkByIteratorFor = xranges::iterator_t<
    xranges::chunk_by_view<View<Iter>, xranges::less_equal>>;

constexpr void test() {
    // Check that value_type is range_value_t and difference_type is
    // range_difference_t
    types::for_each(
        types::forward_iterator_list<int*>{}, TestValueTypeAndDifferenceType{});

    // Check iterator_concept for various categories of ranges
    {
        static_assert(std::same_as<
            ChunkByIteratorFor<forward_iterator<int*>>::iterator_concept,
            std::forward_iterator_tag>);
        static_assert(std::same_as<
            ChunkByIteratorFor<bidirectional_iterator<int*>>::iterator_concept,
            std::bidirectional_iterator_tag>);
        static_assert(std::same_as<
            ChunkByIteratorFor<random_access_iterator<int*>>::iterator_concept,
            std::bidirectional_iterator_tag>);
        static_assert(std::same_as<
            ChunkByIteratorFor<contiguous_iterator<int*>>::iterator_concept,
            std::bidirectional_iterator_tag>);
        static_assert(std::same_as<ChunkByIteratorFor<int*>::iterator_concept,
            std::bidirectional_iterator_tag>);
    }

    // Check iterator_category for various categories of ranges
    {
        static_assert(std::same_as<
            ChunkByIteratorFor<forward_iterator<int*>>::iterator_category,
            std::input_iterator_tag>);
        static_assert(std::same_as<
            ChunkByIteratorFor<bidirectional_iterator<int*>>::iterator_category,
            std::input_iterator_tag>);
        static_assert(std::same_as<
            ChunkByIteratorFor<random_access_iterator<int*>>::iterator_category,
            std::input_iterator_tag>);
        static_assert(std::same_as<
            ChunkByIteratorFor<contiguous_iterator<int*>>::iterator_category,
            std::input_iterator_tag>);
        static_assert(std::same_as<ChunkByIteratorFor<int*>::iterator_category,
            std::input_iterator_tag>);
    }
}
