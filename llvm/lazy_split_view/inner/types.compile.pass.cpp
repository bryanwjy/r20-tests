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

//  using iterator_category = If<
//    derived_from<typename
//    iterator_traits<iterator_t<Base>>::iterator_category,
//    forward_iterator_tag>, forward_iterator_tag, typename
//    iterator_traits<iterator_t<Base>>::iterator_category
//  >;
//  using iterator_concept = typename outer-iterator<Const>::iterator_concept;
//  using value_type = range_value_t<Base>;
//  using difference_type = range_difference_t<Base>;

#include "../types.h"

#include "rxx/ranges.h"

#include <concepts>
#include <iterator>

template <class Range, class Pattern>
using OuterIter = xranges::iterator_t<xranges::lazy_split_view<Range, Pattern>>;
template <class Range, class Pattern>
using InnerIter = xranges::iterator_t<decltype(*OuterIter<Range, Pattern>())>;

// iterator_concept

static_assert(
    std::same_as<typename InnerIter<ForwardView, ForwardView>::iterator_concept,
        typename OuterIter<ForwardView, ForwardView>::iterator_concept>);
static_assert(std::same_as<
    typename InnerIter<InputView, ForwardTinyView>::iterator_concept,
    typename OuterIter<InputView, ForwardTinyView>::iterator_concept>);

// iterator_category

static_assert(std::same_as<
    typename InnerIter<ForwardView, ForwardView>::iterator_category,
    std::forward_iterator_tag>);

template <class Range, class Pattern>
concept NoIteratorCategory =
    !requires { typename InnerIter<Range, Pattern>::iterator_category; };
static_assert(NoIteratorCategory<InputView, ForwardTinyView>);

// value_type

static_assert(
    std::same_as<typename InnerIter<ForwardView, ForwardView>::value_type,
        xranges::range_value_t<ForwardView>>);

// difference_type

static_assert(
    std::same_as<typename InnerIter<ForwardView, ForwardView>::difference_type,
        xranges::range_difference_t<ForwardView>>);
