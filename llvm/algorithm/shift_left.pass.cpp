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

// <algorithm>

// template<class ForwardIterator>
// constexpr ForwardIterator
//   shift_left(ForwardIterator first, ForwardIterator last,
//              typename iterator_traits<ForwardIterator>::difference_type n);

#include "../MoveOnly.h"
#include "../test_iterators.h"
#include "rxx/algorithm/copy.h"
#include "rxx/algorithm/equal.h"
#include "rxx/algorithm/shift.h"
#include "rxx/ranges.h"

#include <cassert>

namespace xranges = __RXX ranges;
namespace xviews = __RXX views;

template <class T, class Iter>
constexpr bool test() {
    int orig[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9};
    T work[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9};

    for (int n = 0; n <= 15; ++n) {
        for (int k = 0; k <= n + 2; ++k) {
            xranges::copy(orig, orig + n, work);
            Iter it = xranges::end(
                xranges::shift_left(Iter(work), Iter(work + n), k));
            if (0 <= k && k < n) {
                assert(it == Iter(work + n - k));
                assert(xranges::equal(orig + k, orig + n, work, work + n - k,
                    [](auto& left, auto& right) { return left == right; }));
            } else {
                assert(it == Iter(work));
                assert(xranges::equal(orig, orig + n, work, work + n,
                    [](auto& left, auto& right) { return left == right; }));
            }
        }
    }

    // n == 0
    {
        T input[] = {0, 1, 2};
        T const expected[] = {0, 1, 2};
        auto output = xranges::shift_left(input, 0);
        assert(xranges::equal(expected, output));
        assert(xranges::end(output) == xranges::end(input));
    }

    // n > 0 && n < len
    {
        T input[] = {0, 1, 2};
        T const expected[] = {1, 2};
        auto output = xranges::shift_left(input, 1);
        assert(xranges::equal(expected, output));
    }
    {
        T input[] = {1, 2, 3, 4, 5, 6, 7, 8};
        T const expected[] = {3, 4, 5, 6, 7, 8};
        auto output = xranges::shift_left(input, 2);
        assert(xranges::equal(expected, output));
    }
    {
        T input[] = {1, 2, 3, 4, 5, 6, 7, 8};
        T const expected[] = {7, 8};
        auto output = xranges::shift_left(input, 6);
        assert(xranges::equal(expected, output));
    }

    // n == len
    {
        T input[] = {0, 1, 2};
        T const expected[] = {0, 1, 2};
        auto output = xranges::shift_left(input, xranges::size(input));
        assert(xranges::equal(expected, input));
        assert(xranges::empty(output) &&
            xranges::end(output) == xranges::begin(input));
    }

    // n > len
    {
        T input[] = {0, 1, 2};
        T const expected[] = {0, 1, 2};
        auto output = xranges::shift_left(input, xranges::size(input) + 1);
        assert(xranges::equal(expected, input));
        assert(xranges::empty(output) &&
            xranges::end(output) == xranges::begin(input));
    }

    return true;
}

int main(int, char**) {
    test<int, forward_iterator<int*>>();
    test<int, bidirectional_iterator<int*>>();
    test<int, random_access_iterator<int*>>();
    test<int, int*>();
    test<MoveOnly, forward_iterator<MoveOnly*>>();
    test<MoveOnly, bidirectional_iterator<MoveOnly*>>();
    test<MoveOnly, random_access_iterator<MoveOnly*>>();
    test<MoveOnly, MoveOnly*>();

    static_assert(test<int, forward_iterator<int*>>());
    static_assert(test<int, bidirectional_iterator<int*>>());
    static_assert(test<int, random_access_iterator<int*>>());
    static_assert(test<int, int*>());
    static_assert(test<MoveOnly, forward_iterator<MoveOnly*>>());
    static_assert(test<MoveOnly, bidirectional_iterator<MoveOnly*>>());
    static_assert(test<MoveOnly, random_access_iterator<MoveOnly*>>());
    static_assert(test<MoveOnly, MoveOnly*>());

    return 0;
}
