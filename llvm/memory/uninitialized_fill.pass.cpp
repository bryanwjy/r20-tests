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

// template <nothrow-forward-iterator ForwardIterator,
// nothrow-sentinel-for<ForwardIterator> Sentinel, class T>
//   requires constructible_from<iter_value_t<ForwardIterator>, const T&>
// ForwardIterator ranges::uninitialized_fill(ForwardIterator first, Sentinel
// last, const T& x);
//
// template <nothrow-forward-range ForwardRange, class T>
//   requires constructible_from<range_value_t<ForwardRange>, const T&>
// borrowed_iterator_t<ForwardRange> ranges::uninitialized_fill(ForwardRange&&
// range, const T& x);

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

static_assert(std::is_class_v<decltype(xranges::uninitialized_fill)>);

struct NotConvertibleFromInt {};
static_assert(!std::is_invocable_v<decltype(xranges::uninitialized_fill),
              NotConvertibleFromInt*, NotConvertibleFromInt*, int>);

int main(int, char**) {
    constexpr int value = 42;
    Counted x(value);
    Counted::reset();
    auto pred = [](Counted const& e) { return e.value == value; };

    // An empty range -- no default constructors should be invoked.
    {
        Buffer<Counted, 1> buf;

        xranges::uninitialized_fill(buf.begin(), buf.begin(), x);
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 0);

        xranges::uninitialized_fill(xranges::empty_view<Counted>(), x);
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 0);

        forward_iterator<Counted*> it(buf.begin());
        auto range = xranges::subrange(
            it, sentinel_wrapper<forward_iterator<Counted*>>(it));
        xranges::uninitialized_fill(range.begin(), range.end(), x);
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 0);
        Counted::reset();

        xranges::uninitialized_fill(range, x);
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 0);
        Counted::reset();
    }

    // A range containing several objects, (iter, sentinel) overload.
    {
        constexpr int N = 5;
        Buffer<Counted, N> buf;

        xranges::uninitialized_fill(buf.begin(), buf.end(), x);
        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);
        assert(std::all_of(buf.begin(), buf.end(), pred));

        std::destroy(buf.begin(), buf.end());
        Counted::reset();
    }

    // A range containing several objects, (range) overload.
    {
        constexpr int N = 5;
        Buffer<Counted, N> buf;

        auto range = xranges::subrange(buf.begin(), buf.end());
        xranges::uninitialized_fill(range, x);
        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);
        assert(std::all_of(buf.begin(), buf.end(), pred));

        std::destroy(buf.begin(), buf.end());
        Counted::reset();
    }

    // Using `counted_iterator`.
    {
        constexpr int N = 3;
        Buffer<Counted, 5> buf;

        xranges::uninitialized_fill(
            std::counted_iterator(buf.begin(), N), std::default_sentinel, x);
        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);
        assert(std::all_of(buf.begin(), buf.begin() + N, pred));

        std::destroy(buf.begin(), buf.begin() + N);
        Counted::reset();
    }

    // Using `views::counted`.
    {
        constexpr int N = 3;
        Buffer<Counted, 5> buf;

        xranges::uninitialized_fill(std::views::counted(buf.begin(), N), x);
        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);
        assert(std::all_of(buf.begin(), buf.begin() + N, pred));

        std::destroy(buf.begin(), buf.begin() + N);
        Counted::reset();
    }

    // Using `reverse_view`.
    {
        constexpr int N = 3;
        Buffer<Counted, 5> buf;

        auto range = xranges::subrange(buf.begin(), buf.begin() + N);
        xranges::uninitialized_fill(xranges::reverse_view(range), x);
        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);
        assert(std::all_of(buf.begin(), buf.begin() + N, pred));

        std::destroy(buf.begin(), buf.begin() + N);
        Counted::reset();
    }

    // Any existing values should be overwritten by value constructors.
    {
        constexpr int N = 5;
        int buffer[N] = {value, value, value, value, value};

        xranges::uninitialized_fill(buffer, buffer + 1, 0);
        assert(buffer[0] == 0);
        assert(buffer[1] == value);

        xranges::uninitialized_fill(buffer, buffer + N, 0);
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
        constexpr int N = 3;
        Buffer<Counted, 5> buf;

        Counted::throw_on = N; // When constructing the fourth object.
        try {
            xranges::uninitialized_fill(buf.begin(), buf.end(), x);
        } catch (...) {}
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == N);

        std::destroy(buf.begin(), buf.begin() + N);
        Counted::reset();
    }

    // An exception is thrown while objects are being created -- the existing
    // objects should stay valid. (range) overload.
    {
        constexpr int N = 3;
        Buffer<Counted, 5> buf;

        Counted::throw_on = N; // When constructing the fourth object.
        try {
            xranges::uninitialized_fill(buf, x);
        } catch (...) {}
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == N);

        std::destroy(buf.begin(), buf.begin() + N);
        Counted::reset();
    }
#endif // RXX_WITH_EXCEPTIONS

    return 0;
}
