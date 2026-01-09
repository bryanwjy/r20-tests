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
// There is a bug in older versions of Clang that causes trouble with
// constraints in classes like `ContainerWithDirectCtr`. XFAIL: apple-clang-15

// template<template<class...> class C, input_range R, class... Args>
//   constexpr auto to(R&& r, Args&&... args);  // Since C++23

#include "container.h"
#include "rxx/algorithm.h"
#include "rxx/ranges.h"

#include <array>
#include <cassert>

namespace xranges = __RXX ranges;

template <class ElementType>
struct ContainerWithDirectCtr : Container<ElementType, CtrChoice::DirectCtr> {
#if RXX_CXX23
    // This behaviour requires P2582R1 in C++23
    using Container<ElementType, CtrChoice::DirectCtr>::Container;
#else
    template <typename... Args>
    requires std::constructible_from<
        Container<ElementType, CtrChoice::DirectCtr>, Args...>
    constexpr ContainerWithDirectCtr(Args&&... args)
        : Container<ElementType, CtrChoice::DirectCtr>{
              std::forward<Args>(args)...} {}
#endif
};

template <xranges::input_range Range>
ContainerWithDirectCtr(Range&&)
    -> ContainerWithDirectCtr<xranges::range_value_t<Range>>;

template <xranges::input_range Range>
ContainerWithDirectCtr(Range&&, int, char)
    -> ContainerWithDirectCtr<xranges::range_value_t<Range>>;

#if RXX_SUPPORTS_FROM_RANGE
// from_range only available in C++23
template <class ElementType>
struct ContainerWithFromRangeT : Container<ElementType, CtrChoice::FromRangeT> {
    using Container<ElementType, CtrChoice::FromRangeT>::Container;
};

template <xranges::input_range Range>
ContainerWithFromRangeT(std::from_range_t, Range&&)
    -> ContainerWithFromRangeT<xranges::range_value_t<Range>>;

template <xranges::input_range Range>
ContainerWithFromRangeT(std::from_range_t, Range&&, int, char)
    -> ContainerWithFromRangeT<xranges::range_value_t<Range>>;
#endif

template <class ElementType>
struct ContainerWithBeginEndPair :
    Container<ElementType, CtrChoice::BeginEndPair> {
#if RXX_CXX23
    // This behaviour requires P2582R1 in C++23
    using Container<ElementType, CtrChoice::BeginEndPair>::Container;
#else
    template <typename... Args>
    requires std::constructible_from<
        Container<ElementType, CtrChoice::BeginEndPair>, Args...>
    constexpr ContainerWithBeginEndPair(Args&&... args)
        : Container<ElementType, CtrChoice::BeginEndPair>{
              std::forward<Args>(args)...} {}
#endif
};

template <class Iter>
ContainerWithBeginEndPair(Iter, Iter)
    -> ContainerWithBeginEndPair<std::iter_value_t<Iter>>;

template <class Iter>
ContainerWithBeginEndPair(Iter, Iter, int, char)
    -> ContainerWithBeginEndPair<std::iter_value_t<Iter>>;

static_assert(sizeof(xranges::details::template_deducer<ContainerWithDirectCtr,
    std::array<int, 5>&, int, char>::type));

constexpr bool test() {
    std::array in = {1, 2, 3, 4, 5};
    int arg1 = 42;
    char arg2 = 'a';

    { // Case 1 -- can construct directly from the given range.
        {
            std::same_as<ContainerWithDirectCtr<int>> decltype(auto) result =
                xranges::to<ContainerWithDirectCtr>(in);

            assert(result.ctr_choice == CtrChoice::DirectCtr);
            assert(xranges::equal(result, in));
            assert((in | xranges::to<ContainerWithDirectCtr>()) == result);
        }

        { // Extra arguments.
            std::same_as<ContainerWithDirectCtr<int>> decltype(auto) result =
                xranges::to<ContainerWithDirectCtr>(in, arg1, arg2);

            assert(result.ctr_choice == CtrChoice::DirectCtr);
            assert(xranges::equal(result, in));
            assert(result.extra_arg1 == arg1);
            assert(result.extra_arg2 == arg2);
            assert((in | xranges::to<ContainerWithDirectCtr>(arg1, arg2)) ==
                result);
        }
    }

#if RXX_SUPPORTS_FROM_RANGE
    { // Case 2 -- can construct from the given range using the `from_range_t`
      // tagged constructor.
        {
            std::same_as<ContainerWithFromRangeT<int>> decltype(auto) result =
                xranges::to<ContainerWithFromRangeT>(in);

            assert(result.ctr_choice == CtrChoice::FromRangeT);
            assert(xranges::equal(result, in));
            assert((in | xranges::to<ContainerWithFromRangeT>()) == result);
        }

        { // Extra arguments.
            std::same_as<ContainerWithFromRangeT<int>> decltype(auto) result =
                xranges::to<ContainerWithFromRangeT>(in, arg1, arg2);

            assert(result.ctr_choice == CtrChoice::FromRangeT);
            assert(xranges::equal(result, in));
            assert(result.extra_arg1 == arg1);
            assert(result.extra_arg2 == arg2);
            assert((in | xranges::to<ContainerWithFromRangeT>(arg1, arg2)) ==
                result);
        }
    }
#endif

    { // Case 3 -- can construct from a begin-end iterator pair.
        {
            std::same_as<ContainerWithBeginEndPair<int>> decltype(auto) result =
                xranges::to<ContainerWithBeginEndPair>(in);

            assert(result.ctr_choice == CtrChoice::BeginEndPair);
            assert(xranges::equal(result, in));
            assert((in | xranges::to<ContainerWithBeginEndPair>()) == result);
        }

        { // Extra arguments.
            std::same_as<ContainerWithBeginEndPair<int>> decltype(auto) result =
                xranges::to<ContainerWithBeginEndPair>(in, arg1, arg2);

            assert(result.ctr_choice == CtrChoice::BeginEndPair);
            assert(xranges::equal(result, in));
            assert(result.extra_arg1 == arg1);
            assert(result.extra_arg2 == arg2);
            assert((in | xranges::to<ContainerWithBeginEndPair>(arg1, arg2)) ==
                result);
        }
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
