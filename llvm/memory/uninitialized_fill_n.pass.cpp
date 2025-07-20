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

// <memory>

// template <nothrow-forward-iterator ForwardIterator, class T>
//   requires constructible_from<iter_value_t<ForwardIterator>, const T&>
// ForwardIterator ranges::uninitialized_fill_n(ForwardIterator first,
// iter_difference_t<ForwardIterator> n);

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "buffer.h"
#include "counted.h"
#include "overload_compare_iterator.h"
#include "rxx/algorithm.h"
#include "rxx/iterator.h"
#include "rxx/memory.h"
#include "rxx/ranges.h"

#include <cassert>
#include <type_traits>

static_assert(std::is_class_v<decltype(xranges::uninitialized_fill_n)>);

struct NotConvertibleFromInt {};
static_assert(!std::is_invocable_v<decltype(xranges::uninitialized_fill_n),
              NotConvertibleFromInt*, std::size_t, int>);

int main(int, char**) {
    constexpr int value = 42;
    Counted x(value);
    Counted::reset();
    auto pred = [](Counted const& e) { return e.value == value; };

    // An empty range -- no default constructors should be invoked.
    {
        Buffer<Counted, 1> buf;

        xranges::uninitialized_fill_n(buf.begin(), 0, x);
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 0);
    }

    // A range containing several objects.
    {
        constexpr int N = 5;
        Buffer<Counted, N> buf;

        xranges::uninitialized_fill_n(buf.begin(), N, x);
        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);
        assert(std::all_of(buf.begin(), buf.end(), pred));

        xranges::destroy(buf.begin(), buf.end());
        Counted::reset();
    }

    // Any existing values should be overwritten by value constructors.
    {
        constexpr int N = 5;
        int buffer[N] = {value, value, value, value, value};

        xranges::uninitialized_fill_n(buffer, 1, 0);
        assert(buffer[0] == 0);
        assert(buffer[1] == value);

        xranges::uninitialized_fill_n(buffer, N, 0);
        assert(buffer[0] == 0);
        assert(buffer[1] == 0);
        assert(buffer[2] == 0);
        assert(buffer[3] == 0);
        assert(buffer[4] == 0);
    }

    // An exception is thrown while objects are being created -- the existing
    // objects should stay valid. (iterator, sentinel) overload.
#if RXX_WITH_EXCEPTIONS
    {
        constexpr int N = 5;
        Buffer<Counted, N> buf;

        Counted::throw_on = 3; // When constructing the fourth object.
        try {
            xranges::uninitialized_fill_n(buf.begin(), N, x);
        } catch (...) {}
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 3);

        xranges::destroy(buf.begin(), buf.begin() + 3);
        Counted::reset();
    }
#endif // RXX_WITH_EXCEPTIONS

    return 0;
}
