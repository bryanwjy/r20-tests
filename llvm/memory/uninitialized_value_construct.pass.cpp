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
// nothrow-sentinel-for<ForwardIterator> Sentinel>
//   requires default_initializable<iter_value_t<ForwardIterator>>
// ForwardIterator ranges::uninitialized_value_construct(ForwardIterator first,
// Sentinel last);
//
// template <nothrow-forward-range ForwardRange>
//   requires default_initializable<range_value_t<ForwardRange>>
// borrowed_iterator_t<ForwardRange>
// ranges::uninitialized_value_construct(ForwardRange&& range);

#include "../test_iterators.h"
#include "buffer.h"
#include "counted.h"
#include "rxx/iterator.h"
#include "rxx/memory.h"
#include "rxx/ranges.h"

#include <cassert>
#include <type_traits>

static_assert(
    std::is_class_v<decltype(xranges::uninitialized_value_construct)>);

struct NotDefaultCtrable {
    NotDefaultCtrable() = delete;
};
static_assert(
    !std::is_invocable_v<decltype(xranges::uninitialized_value_construct),
        NotDefaultCtrable*, NotDefaultCtrable*>);

int main(int, char**) {
    // An empty range -- no default constructors should be invoked.
    {
        Buffer<Counted, 1> buf;

        xranges::uninitialized_value_construct(buf.begin(), buf.begin());
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 0);

        xranges::uninitialized_value_construct(xranges::empty_view<Counted>());
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 0);

        forward_iterator<Counted*> it(buf.begin());
        auto range = xranges::subrange(
            it, sentinel_wrapper<forward_iterator<Counted*>>(it));
        xranges::uninitialized_value_construct(range.begin(), range.end());
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 0);

        xranges::uninitialized_value_construct(range);
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 0);
    }

    // A range containing several objects, (iter, sentinel) overload.
    {
        constexpr int N = 5;
        Buffer<Counted, N> buf;

        xranges::uninitialized_value_construct(buf.begin(), buf.end());
        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);

        std::destroy(buf.begin(), buf.end());
        Counted::reset();
    }

    // A range containing several objects, (range) overload.
    {
        constexpr int N = 5;
        Buffer<Counted, N> buf;

        auto range = xranges::subrange(buf.begin(), buf.end());
        xranges::uninitialized_value_construct(range);
        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);

        std::destroy(buf.begin(), buf.end());
        Counted::reset();
    }

    // Using `counted_iterator`.
    {
        constexpr int N = 3;
        Buffer<Counted, 5> buf;

        xranges::uninitialized_value_construct(
            std::counted_iterator(buf.begin(), N), std::default_sentinel);
        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);

        std::destroy(buf.begin(), buf.begin() + N);
        Counted::reset();
    }

    // Using `views::counted`.
    {
        constexpr int N = 3;
        Buffer<Counted, 5> buf;

        xranges::uninitialized_value_construct(
            std::views::counted(buf.begin(), N));
        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);

        std::destroy(buf.begin(), buf.begin() + N);
        Counted::reset();
    }

    // Using `reverse_view`.
    {
        constexpr int N = 3;
        Buffer<Counted, 5> buf;

        auto range = xranges::subrange(buf.begin(), buf.begin() + N);
        xranges::uninitialized_value_construct(xranges::reverse_view(range));
        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);

        std::destroy(buf.begin(), buf.begin() + N);
        Counted::reset();
    }

    // Any existing values should be overwritten by value constructors.
    {
        constexpr int N = 5;
        int buffer[N] = {42, 42, 42, 42, 42};

        xranges::uninitialized_value_construct(buffer, buffer + 1);
        assert(buffer[0] == 0);
        assert(buffer[1] == 42);

        xranges::uninitialized_value_construct(buffer, buffer + N);
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
        Buffer<Counted, 5> buf;

        Counted::throw_on =
            3; // When constructing the fourth object (counting from one).
        try {
            xranges::uninitialized_value_construct(buf.begin(), buf.end());
        } catch (...) {}
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 3);
        xranges::destroy(buf.begin(), buf.begin() + Counted::total_objects);
        Counted::reset();
    }

    // An exception is thrown while objects are being created -- the existing
    // objects should stay valid. (range) overload.
    {
        Buffer<Counted, 5> buf;

        Counted::throw_on = 3; // When constructing the fourth object.
        try {
            xranges::uninitialized_value_construct(buf);
        } catch (...) {}
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 3);
        xranges::destroy(buf.begin(), buf.begin() + Counted::total_objects);
        Counted::reset();
    }
#endif // RXX_WITH_EXCEPTIONS

    return 0;
}
