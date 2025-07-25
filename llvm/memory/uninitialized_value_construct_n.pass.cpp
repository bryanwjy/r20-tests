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

// template <nothrow-forward-iterator ForwardIterator>
//   requires default_initializable<iter_value_t<ForwardIterator>>
// ForwardIterator ranges::uninitialized_value_construct_n(ForwardIterator
// first,
//     iter_difference_t<ForwardIterator> n);

#include "../test_iterators.h"
#include "buffer.h"
#include "counted.h"
#include "rxx/iterator.h"
#include "rxx/memory.h"
#include "rxx/ranges.h"

#include <cassert>

static_assert(
    std::is_class_v<decltype(xranges::uninitialized_value_construct_n)>);

struct NotDefaultCtrable {
    NotDefaultCtrable() = delete;
};
static_assert(
    !std::is_invocable_v<decltype(xranges::uninitialized_value_construct_n),
        NotDefaultCtrable*, int>);

int main(int, char**) {
    // An empty range -- no default constructors should be invoked.
    {
        Buffer<Counted, 1> buf;

        xranges::uninitialized_value_construct_n(buf.begin(), 0);
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 0);
    }

    // A range containing several objects.
    {
        constexpr int N = 5;
        Buffer<Counted, N> buf;

        xranges::uninitialized_value_construct_n(buf.begin(), N);
        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);

        xranges::destroy(buf.begin(), buf.end());
        Counted::reset();
    }

    // Any existing values should be overwritten by value constructors.
    {
        constexpr int N = 5;
        int buffer[N] = {42, 42, 42, 42, 42};

        xranges::uninitialized_value_construct_n(buffer, 1);
        assert(buffer[0] == 0);
        assert(buffer[1] == 42);

        xranges::uninitialized_value_construct_n(buffer, N);
        assert(buffer[0] == 0);
        assert(buffer[1] == 0);
        assert(buffer[2] == 0);
        assert(buffer[3] == 0);
        assert(buffer[4] == 0);
    }

    // An exception is thrown while objects are being created -- the existing
    // objects should stay valid.
#if RXX_WITH_EXCEPTIONS
    {
        constexpr int N = 5;
        Buffer<Counted, N> buf;

        Counted::throw_on =
            3; // When constructing the fourth object (counting from one).
        try {
            xranges::uninitialized_value_construct_n(buf.begin(), N);
        } catch (...) {}
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 3);
        xranges::destroy(buf.begin(), buf.begin() + Counted::total_objects);
        Counted::reset();
    }
#endif // RXX_WITH_EXCEPTIONS

    return 0;
}
