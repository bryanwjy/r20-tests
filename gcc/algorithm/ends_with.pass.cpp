// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2020-2025 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include "rxx/algorithm/ends_with.h"

#include "../test_iterators.h"
#include "rxx/ranges.h"

#include <cassert>
#include <cmath>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

template <typename Range1, typename Range2>
void test01() {
    int n[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    Range1 haystack(n, n + 10);
    Range2 needle(n + 7, n + 10);
    assert(xranges::ends_with(haystack, needle));

    haystack = Range1(n);
    needle = Range2(n, n + 10);
    assert(xranges::ends_with(haystack, needle));

    haystack = Range1(n);
    needle = Range2(n + 6, n + 9);
    assert(!xranges::ends_with(haystack, needle));

    haystack = Range1(n);
    needle = Range2(n + 6, n + 9);
    assert(xranges::ends_with(
        haystack, needle, [](int n, int m) { return std::abs(n - m) <= 1; }));

    haystack = Range1(n);
    needle = Range2(n + 6, n + 9);
    assert(xranges::ends_with(
        haystack, needle, xranges::equal_to{}, [](int n) { return n - 1; }));

    haystack = Range1(n);
    needle = Range2(n + 6, n + 9);
    assert(xranges::ends_with(haystack, needle, xranges::equal_to{},
        std::identity{}, [](int n) { return n + 1; }));

    haystack = Range1(n, n + 5);
    needle = Range2(n, n + 10);
    assert(!xranges::ends_with(haystack, needle));
}

template <typename Range1, typename Range2>
void test02() {
    int n[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    Range1 haystack(n, n + 10);
    Range2 needle(n + 7, n + 10);
    assert(xranges::ends_with(
        haystack.begin(), haystack.end(), needle.begin(), needle.end()));

    haystack = Range1(n);
    needle = Range2(n, n + 10);
    assert(xranges::ends_with(
        haystack.begin(), haystack.end(), needle.begin(), needle.end()));

    haystack = Range1(n);
    needle = Range2(n + 6, n + 9);
    assert(!xranges::ends_with(
        haystack.begin(), haystack.end(), needle.begin(), needle.end()));

    haystack = Range1(n);
    needle = Range2(n + 6, n + 9);
    assert(xranges::ends_with(haystack.begin(), haystack.end(), needle.begin(),
        needle.end(), [](int n, int m) { return std::abs(n - m) <= 1; }));

    haystack = Range1(n);
    needle = Range2(n + 6, n + 9);
    assert(xranges::ends_with(haystack.begin(), haystack.end(), needle.begin(),
        needle.end(), xranges::equal_to{}, [](int n) { return n - 1; }));

    haystack = Range1(n);
    needle = Range2(n + 6, n + 9);
    assert(xranges::ends_with(haystack.begin(), haystack.end(), needle.begin(),
        needle.end(), xranges::equal_to{}, std::identity{},
        [](int n) { return n + 1; }));

    haystack = Range1(n, n + 5);
    needle = Range2(n, n + 10);
    assert(!xranges::ends_with(
        haystack.begin(), haystack.end(), needle.begin(), needle.end()));

    haystack = Range1(n, n + 5);
    needle = Range2(n + 10, n + 10);
    assert(xranges::ends_with(
        haystack.begin(), haystack.end(), needle.begin(), needle.end()));
}

void test03() {
    auto haystack = xviews::iota(0, 10);
    auto needle = xviews::iota(5, 10);

#if RXX_SUPPORTS_INT128 && RXX_LIBSTDCXX && \
    (!__STRICT_ANSI__ || RXX_LIBSTDCXX_AT_LEAST(16))
    auto haystack_ict = xviews::iota(__int128(0), __int128(10));
    auto needle_ict = xviews::iota(__int128(5), __int128(10));
#else
    auto haystack_ict = xviews::iota(0ll, 10ll);
    auto needle_ict = xviews::iota(5ll, 10ll);
#endif

    assert(xranges::ends_with(haystack, needle_ict));
    assert(xranges::ends_with(haystack.begin(), haystack.end(),
        needle_ict.begin(), needle_ict.end()));

    assert(xranges::ends_with(haystack_ict, needle));
    assert(xranges::ends_with(haystack_ict.begin(), haystack_ict.end(),
        needle.begin(), needle.end()));

    assert(xranges::ends_with(haystack_ict, needle_ict));
    assert(xranges::ends_with(haystack_ict.begin(), haystack_ict.end(),
        needle_ict.begin(), needle_ict.end()));
}

int main() {
    using namespace rxx::tests;
    using forward = test_forward_range<int>;
    using bidirectional_common = bidirectional_container<int>;
    using input_sized = test_input_sized_range<int>;
    using input_sized_sent =
        test_sized_range_sized_sent<int, input_iterator_wrapper>;
    using random_access = test_random_access_range<int>;
    using random_access_sized = test_random_access_sized_range<int>;
    using random_access_sized_sent =
        test_sized_range_sized_sent<int, random_access_iterator_wrapper>;

    test01<forward, forward>();
    test01<random_access, random_access>();
    test02<forward, forward>();
    test02<random_access, random_access>();

    test01<bidirectional_common, bidirectional_common>();
    test02<bidirectional_common, bidirectional_common>();
    test01<bidirectional_common, forward>();
    test02<bidirectional_common, forward>();

    test01<input_sized, input_sized>();
    test01<random_access_sized, random_access_sized>();
    // test02<input_sized, input_sized>(); constraint violation
    test02<random_access_sized, random_access_sized>();

    test01<input_sized_sent, input_sized_sent>();
    test01<random_access_sized_sent, random_access_sized_sent>();
    test02<input_sized_sent, input_sized_sent>();
    test02<random_access_sized_sent, random_access_sized_sent>();

    test03();
}
