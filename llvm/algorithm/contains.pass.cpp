// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <algorithm>

// UNSUPPORTED: c++03, c++11, c++14, c++17

// template<input_iterator I, sentinel_for<I> S, class T, class Proj = identity>
//     requires indirect_binary_predicate<ranges::equal_to, projected<I, Proj>,
//     const T*> constexpr bool ranges::contains(I first, S last, const T&
//     value, Proj proj = {});       // since C++23

// template<input_range R, class T, class Proj = identity>
//     requires indirect_binary_predicate<ranges::equal_to,
//     projected<iterator_t<R>, Proj>, const T*> constexpr bool
//     ranges::contains(R&& r, const T& value, Proj proj = {}); // since C++23

#include "rxx/algorithm/contains.h"

#include "../almost_satisfies_types.h"
#include "../boolean_testable.h"
#include "../test_iterators.h"
#include "rxx/ranges.h"

#include <array>
#include <cassert>
#include <list>
#include <string>
#include <vector>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct NotEqualityComparable {};

template <class Iter, class Sent = Iter>
concept HasContainsIt =
    requires(Iter iter, Sent sent) { xranges::contains(iter, sent, *iter); };

static_assert(HasContainsIt<int*>);
static_assert(HasContainsIt<int*, int*>);
static_assert(!HasContainsIt<NotEqualityComparable*>);
static_assert(!HasContainsIt<InputIteratorNotDerivedFrom>);
static_assert(!HasContainsIt<InputIteratorNotIndirectlyReadable>);
static_assert(!HasContainsIt<InputIteratorNotInputOrOutputIterator>);
static_assert(
    !HasContainsIt<cpp20_input_iterator<int*>, SentinelForNotSemiregular>);
static_assert(!HasContainsIt<cpp20_input_iterator<int*>,
              InputRangeNotSentinelEqualityComparableWith>);
static_assert(!HasContainsIt<cpp20_input_iterator<int*>,
              sentinel_wrapper<cpp20_input_iterator<int*>>>);

static_assert(!HasContainsIt<int*, int>);
static_assert(!HasContainsIt<int, int*>);

template <class Range, class ValT>
concept HasContainsR = requires(
    Range&& range) { xranges::contains(std::forward<Range>(range), ValT{}); };

static_assert(!HasContainsR<int, int>);
static_assert(HasContainsR<int[1], int>);
static_assert(!HasContainsR<NotEqualityComparable[1], NotEqualityComparable>);
static_assert(!HasContainsR<InputRangeNotDerivedFrom, int>);
static_assert(!HasContainsR<InputRangeNotIndirectlyReadable, int>);
static_assert(!HasContainsR<InputRangeNotInputOrOutputIterator, int>);
static_assert(!HasContainsR<InputRangeNotSentinelSemiregular, int>);
static_assert(!HasContainsR<InputRangeNotSentinelEqualityComparableWith, int>);

template <class Iter, class Sent = Iter>
constexpr void test_iterators() {
    using ValueT = std::iter_value_t<Iter>;
    { // simple tests
        ValueT a[] = {1, 2, 3, 4, 5, 6};
        {
            std::same_as<bool> decltype(auto) ret =
                xranges::contains(Iter(a), Sent(Iter(a + 6)), 3);
            assert(ret);
        }
        {
            auto whole = xranges::subrange(Iter(a), Sent(Iter(a + 6)));
            std::same_as<bool> decltype(auto) ret = xranges::contains(whole, 3);
            assert(ret);
        }
    }

    { // check that a range with a single element works
        ValueT a[] = {32};
        {
            bool ret = xranges::contains(Iter(a), Sent(Iter(a + 1)), 32);
            assert(ret);
        }
        {
            auto whole = xranges::subrange(Iter(a), Sent(Iter(a + 1)));
            bool ret = xranges::contains(whole, 32);
            assert(ret);
        }
    }

    { // check that an empty range works
        std::array<ValueT, 0> a = {};
        {
            bool ret =
                xranges::contains(Iter(a.data()), Sent(Iter(a.data())), 1);
            assert(!ret);
        }
        {
            auto whole =
                xranges::subrange(Iter(a.data()), Sent(Iter(a.data())));
            bool ret = xranges::contains(whole, 1);
            assert(!ret);
        }
    }

    { // check that the first element matches
        ValueT a[] = {32, 3, 2, 1, 0, 23, 21, 9, 40, 100};
        {
            bool ret = xranges::contains(Iter(a), Sent(Iter(a + 10)), 32);
            assert(ret);
        }
        {
            auto whole = xranges::subrange(Iter(a), Sent(Iter(a + 10)));
            bool ret = xranges::contains(whole, 32);
            assert(ret);
        }
    }

    { // check that the last element matches
        ValueT a[] = {3, 22, 1, 43, 99, 0, 56, 100, 32};
        {
            bool ret = xranges::contains(Iter(a), Sent(Iter(a + 9)), 32);
            assert(ret);
        }
        {
            auto whole = xranges::subrange(Iter(a), Sent(Iter(a + 9)));
            bool ret = xranges::contains(whole, 32);
            assert(ret);
        }
    }

    { // no match
        ValueT a[] = {13, 1, 21, 4, 5};
        {
            bool ret = xranges::contains(Iter(a), Sent(Iter(a + 5)), 10);
            assert(!ret);
        }
        {
            auto whole = xranges::subrange(Iter(a), Sent(Iter(a + 5)));
            bool ret = xranges::contains(whole, 10);
            assert(!ret);
        }
    }

    { // check that the projections are used
        int a[] = {1, 9, 0, 13, 25};
        {
            bool ret =
                xranges::contains(a, a + 5, -13, [&](int i) { return i * -1; });
            assert(ret);
        }
        {
            auto range = xranges::subrange(a, a + 5);
            bool ret =
                xranges::contains(range, -13, [&](int i) { return i * -1; });
            assert(ret);
        }
    }
}

constexpr bool test() {
    types::for_each(types::type_list<char, long long>{}, []<class T> {
        types::for_each(types::cpp20_input_iterator_list<T*>{}, []<class Iter> {
            if constexpr (std::forward_iterator<Iter>)
                test_iterators<Iter>();
            test_iterators<Iter, sentinel_wrapper<Iter>>();
            test_iterators<Iter, sized_sentinel<Iter>>();
        });
    });

    { // count invocations of the projection for contiguous iterators
        int a[] = {1, 9, 0, 13, 25};
        int projection_count = 0;
        {
            bool ret = xranges::contains(a, a + 5, 0, [&](int i) {
                ++projection_count;
                return i;
            });
            assert(ret);
            assert(projection_count == 3);
            projection_count = 0;
        }
        {
            bool ret = xranges::contains(a, 0, [&](int i) {
                ++projection_count;
                return i;
            });
            assert(ret);
            assert(projection_count == 3);
        }
    }

// std::string did not work at constexpr before
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AFTER(2023, 11, 08)
    if (!std::is_constant_evaluated())
#endif
    { // check invocations of the projection for std::string
        std::string const str{"hello world"};
        std::string const str1{"hi world"};
        int projection_count = 0;
        {
            std::string a[] = {str1, str1, str, str1, str1};
            auto whole =
                xranges::subrange(forward_iterator(std::move_iterator(a)),
                    forward_iterator(std::move_iterator(a + 5)));
            bool ret = xranges::contains(whole.begin(), whole.end(),
                +"hello world", [&](std::string const i) {
                    ++projection_count;
                    return i;
                });
            assert(ret);
            assert(projection_count == 3);
            projection_count = 0;
        }
        {
            std::string a[] = {str1, str1, str, str1, str1};
            auto whole =
                xranges::subrange(forward_iterator(std::move_iterator(a)),
                    forward_iterator(std::move_iterator(a + 5)));
            bool ret = xranges::contains(
                whole, +"hello world", [&](std::string const i) {
                    ++projection_count;
                    return i;
                });
            assert(ret);
            assert(projection_count == 3);
        }
    }

#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AFTER(2024, 06, 11)
    if (!std::is_constant_evaluated())
#endif
    { // check invocations of the projection for non-contiguous iterators
        std::vector<bool> whole{false, false, true, false};
        int projection_count = 0;
        {
            bool ret = xranges::contains(
                whole.begin(), whole.end(), true, [&](bool b) {
                    ++projection_count;
                    return b;
                });
            assert(ret);
            assert(projection_count == 3);
            projection_count = 0;
        }
        {
            bool ret = xranges::contains(whole, true, [&](bool b) {
                ++projection_count;
                return b;
            });
            assert(ret);
            assert(projection_count == 3);
        }
    }

    { // check invocations of the projection for views::transform
        int a[] = {1, 2, 3, 4, 5};
        int projection_count = 0;
        auto square_number = a | xviews::transform([](int x) { return x * x; });
        {
            bool ret = xranges::contains(
                square_number.begin(), square_number.end(), 16, [&](int i) {
                    ++projection_count;
                    return i;
                });
            assert(ret);
            assert(projection_count == 4);
            projection_count = 0;
        }
        {
            bool ret = xranges::contains(square_number, 16, [&](int i) {
                ++projection_count;
                return i;
            });
            assert(ret);
            assert(projection_count == 4);
        }
    }

    return true;
}

// test for non-contiguous containers
bool test_nonconstexpr() {
    std::list<int> a = {7, 5, 0, 16, 8};
    int projection_count = 0;
    {
        bool ret = xranges::contains(a.begin(), a.end(), 0, [&](int i) {
            ++projection_count;
            return i;
        });
        assert(ret);
        assert(projection_count == 3);
        projection_count = 0;
    }
    {
        bool ret = xranges::contains(a, 0, [&](int i) {
            ++projection_count;
            return i;
        });
        assert(ret);
        assert(projection_count == 3);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    assert(test_nonconstexpr());

    return 0;
}
