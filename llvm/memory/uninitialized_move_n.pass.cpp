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
//
// template<input_iterator I, nothrow-forward-iterator O,
// nothrow-sentinel-for<O> S>
//   requires constructible_from<iter_value_t<O>, iter_reference_t<I>>
// uninitialized_copy_n_result<I, O> uninitialized_copy_n(I ifirst,
// iter_difference_t<I> n, O ofirst, S olast); // since C++20

#include "../MoveOnly.h"
#include "../static_asserts.h"
#include "../test_iterators.h"
#include "buffer.h"
#include "counted.h"
#include "overload_compare_iterator.h"
#include "rxx/algorithm.h"
#include "rxx/iterator.h"
#include "rxx/memory.h"
#include "rxx/ranges.h"

#include <array>
#include <cassert>
#include <type_traits>

static_assert(std::is_class_v<decltype(xranges::uninitialized_move_n)>);

static_assert(std::is_invocable_v<decltype(xranges::uninitialized_move_n), int*,
    std::size_t, long*, long*>);
struct NotConvertibleFromInt {};
static_assert(
    !std::is_invocable_v<decltype(xranges::uninitialized_move_n), int*,
        std::size_t, NotConvertibleFromInt*, NotConvertibleFromInt*>);

int main(int, char**) {
    // An empty range -- no default constructors should be invoked.
    {
        Counted in[] = {Counted()};
        Buffer<Counted, 1> out;
        Counted::reset();

        auto result =
            xranges::uninitialized_move_n(in, 0, out.begin(), out.end());
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == 0);
        assert(Counted::total_copies == 0);
        assert(result.in == in);
        assert(result.out == out.begin());
    }
    Counted::reset();

    // A range containing several objects.
    {
        constexpr int N = 5;
        Counted in[N] = {
            Counted(1), Counted(2), Counted(3), Counted(4), Counted(5)};
        Buffer<Counted, N> out;
        Counted::reset();

        auto result =
            xranges::uninitialized_move_n(in, N, out.begin(), out.end());
        ASSERT_SAME_TYPE(decltype(result),
            xranges::uninitialized_move_n_result<Counted*, Counted*>);

        assert(Counted::current_objects == N);
        assert(Counted::total_objects == N);
        assert(Counted::total_moves == N);
        assert(Counted::total_copies == 0);

        assert(std::equal(in, in + N, out.begin(), out.end()));
        assert(result.in == in + N);
        assert(result.out == out.end());

        xranges::destroy(out.begin(), out.end());
    }
    Counted::reset();

    // An exception is thrown while objects are being created -- the existing
    // objects should stay valid. (iterator, sentinel) overload.
#if RXX_WITH_EXCEPTIONS
    {
        constexpr int N = 3;
        Counted in[] = {
            Counted(1), Counted(2), Counted(3), Counted(4), Counted(5)};
        Buffer<Counted, 5> out;
        Counted::reset();

        Counted::throw_on = N; // When constructing out[3].
        try {
            xranges::uninitialized_move_n(in, 5, out.begin(), out.end());
            assert(false);
        } catch (...) {}
        assert(Counted::current_objects == 0);
        assert(Counted::total_objects == N);
        assert(Counted::total_moves == N);
        assert(Counted::total_copies == 0);

        xranges::destroy(out.begin(), out.begin() + N);
    }
    Counted::reset();

#endif // RXX_WITH_EXCEPTIONS

    // Conversions.
    {
        constexpr int N = 3;
        int in[N] = {1, 2, 3};
        Buffer<double, N> out;

        xranges::uninitialized_move_n(in, N, out.begin(), out.end());
        assert(std::equal(in, in + N, out.begin(), out.end()));
    }

    // Destination range is shorter than the source range.
    {
        constexpr int M = 3;
        constexpr int N = 5;
        Counted in[N] = {
            Counted(1), Counted(2), Counted(3), Counted(4), Counted(5)};
        Buffer<Counted, M> out;
        Counted::reset();

        auto result =
            xranges::uninitialized_move_n(in, N, out.begin(), out.end());
        assert(Counted::current_objects == M);
        assert(Counted::total_objects == M);
        assert(Counted::total_moves == M);
        assert(Counted::total_copies == 0);

        assert(std::equal(in, in + M, out.begin(), out.end()));
        assert(result.in == in + M);
        assert(result.out == out.end());
    }

    // Ensure the `iter_move` customization point is being used.
    {
        constexpr int N = 3;
        int in[N] = {1, 2, 3};
        Buffer<int, N> out;
        int iter_moves = 0;
        adl::Iterator begin = adl::Iterator::TrackMoves(in, iter_moves);
        adl::Iterator end = adl::Iterator::TrackMoves(in + N, iter_moves);

        xranges::uninitialized_move(begin, end, out.begin(), out.end());
        assert(iter_moves == 3);
        iter_moves = 0;

        xranges::subrange range(begin, end);
        xranges::uninitialized_move(range, out);
        assert(iter_moves == 3);
        iter_moves = 0;
    }

    // Move-only iterators are supported.
    {
        using MoveOnlyIter = cpp20_input_iterator<int const*>;
        static_assert(!std::is_copy_constructible_v<MoveOnlyIter>);

        constexpr int N = 3;
        int buffer[N] = {1, 2, 3};

        MoveOnlyIter in(buffer);
        Buffer<int, N> out;
        xranges::uninitialized_move_n(std::move(in), N, out.begin(), out.end());
    }

    // MoveOnly types are supported
    {
        {
            MoveOnly a[] = {1, 2, 3, 4};
            Buffer<MoveOnly, 4> out;
            xranges::uninitialized_move_n(
                std::begin(a), std::size(a), std::begin(out), std::end(out));
            assert(xranges::equal(out, std::array<MoveOnly, 4>{1, 2, 3, 4}));
        }
    }

    // Test with an iterator that overloads operator== and operator!= as the
    // input and output iterators
    {
        using T = int;
        using Iterator = overload_compare_iterator<T*>;
        int const N = 5;

        // input
        {
            char pool[sizeof(T) * N] = {0};
            T* p = reinterpret_cast<T*>(pool);
            T* p_end = reinterpret_cast<T*>(pool) + N;
            T array[N] = {1, 2, 3, 4, 5};
            xranges::uninitialized_move_n(Iterator(array), N, p, p_end);
            for (int i = 0; i != N; ++i) {
                assert(array[i] == p[i]);
            }
        }

        // output
        {
            char pool[sizeof(T) * N] = {0};
            T* p = reinterpret_cast<T*>(pool);
            T* p_end = reinterpret_cast<T*>(pool) + N;
            T array[N] = {1, 2, 3, 4, 5};
            xranges::uninitialized_move_n(
                array, N, Iterator(p), Iterator(p_end));
            for (int i = 0; i != N; ++i) {
                assert(array[i] == p[i]);
            }
        }
    }

    return 0;
}
