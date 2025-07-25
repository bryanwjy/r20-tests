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
// namespace ranges {
//   template<nothrow-input-iterator InputIterator,
//   nothrow-sentinel-for<InputIterator> Sentinel>
//     requires destructible<iter_value_t<InputIterator>>
//     constexpr InputIterator destroy(InputIterator first, Sentinel last)
//     noexcept; // since C++20
//   template<nothrow-input-range InputRange>
//     requires destructible<range_value_t<InputRange>>
//     constexpr borrowed_iterator_t<InputRange> destroy(InputRange&& range)
//     noexcept; // since C++20
// }

#include "../test_iterators.h"
#include "rxx/memory.h"
#include "rxx/ranges.h"

#include <cassert>
#include <type_traits>

static_assert(std::is_class_v<decltype(xranges::destroy)>);

struct NotNothrowDtrable {
    ~NotNothrowDtrable() noexcept(false) {}
};
static_assert(!std::is_invocable_v<decltype(xranges::destroy),
              NotNothrowDtrable*, NotNothrowDtrable*>);

struct Counted {
    int& count;

    constexpr Counted(int& count_ref) : count(count_ref) { ++count; }
    constexpr Counted(Counted const& rhs) : count(rhs.count) { ++count; }
    constexpr ~Counted() { --count; }

    friend void operator&(Counted) = delete;
};

template <class Iterator>
constexpr void test() {
    // (iterator + sentinel) overload.
    {
        constexpr int N = 5;
        std::allocator<Counted> alloc;
        using Traits = std::allocator_traits<decltype(alloc)>;
        int counter = 0;

        Counted* out = Traits::allocate(alloc, N);
        for (int i = 0; i != N; ++i) {
            Traits::construct(alloc, out + i, counter);
        }
        assert(counter == N);

        xranges::destroy(Iterator(out), Iterator(out + N));
        assert(counter == 0);

        Traits::deallocate(alloc, out, N);
    }

    // (range) overload.
    {
        constexpr int N = 5;
        std::allocator<Counted> alloc;
        using Traits = std::allocator_traits<decltype(alloc)>;
        int counter = 0;

        Counted* out = Traits::allocate(alloc, N);
        for (int i = 0; i != N; ++i) {
            Traits::construct(alloc, out + i, counter);
        }
        assert(counter == N);

        auto range = xranges::subrange(Iterator(out), Iterator(out + N));
        xranges::destroy(range);
        assert(counter == 0);

        Traits::deallocate(alloc, out, N);
    }
}

constexpr bool tests() {
    test<Counted*>();
    test<forward_iterator<Counted*>>();

    return true;
}

constexpr bool test_arrays() {
    // One-dimensional array, (iterator + sentinel) overload.
    {
        constexpr int N = 5;
        constexpr int M = 3;

        using Array = Counted[M];
        std::allocator<Array> alloc;
        using Traits = std::allocator_traits<decltype(alloc)>;
        int counter = 0;

        Array* buffer = Traits::allocate(alloc, N);
        for (int i = 0; i != N; ++i) {
            Array& array_ref = *(buffer + i);
            for (int j = 0; j != M; ++j) {
                Traits::construct(alloc, std::addressof(array_ref[j]), counter);
            }
        }
        assert(counter == N * M);

        xranges::destroy(buffer, buffer + N);
        assert(counter == 0);

        Traits::deallocate(alloc, buffer, N);
    }

    // One-dimensional array, (range) overload.
    {
        constexpr int N = 5;
        constexpr int A = 3;

        using Array = Counted[A];
        std::allocator<Array> alloc;
        using Traits = std::allocator_traits<decltype(alloc)>;
        int counter = 0;

        Array* buffer = Traits::allocate(alloc, N);
        for (int i = 0; i != N; ++i) {
            Array& array_ref = *(buffer + i);
            for (int j = 0; j != A; ++j) {
                Traits::construct(alloc, std::addressof(array_ref[j]), counter);
            }
        }
        assert(counter == N * A);

        auto range = xranges::subrange(buffer, buffer + N);
        xranges::destroy(range);
        assert(counter == 0);

        Traits::deallocate(alloc, buffer, N);
    }

    // Multidimensional array, (iterator + sentinel ) overload.
    {
        constexpr int N = 5;
        constexpr int A = 3;
        constexpr int B = 3;

        using Array = Counted[A][B];
        std::allocator<Array> alloc;
        using Traits = std::allocator_traits<decltype(alloc)>;
        int counter = 0;

        Array* buffer = Traits::allocate(alloc, N);
        for (int i = 0; i != N; ++i) {
            Array& array_ref = *(buffer + i);
            for (int j = 0; j != A; ++j) {
                for (int k = 0; k != B; ++k) {
                    Traits::construct(
                        alloc, std::addressof(array_ref[j][k]), counter);
                }
            }
        }
        assert(counter == N * A * B);

        xranges::destroy(buffer, buffer + N);
        assert(counter == 0);

        Traits::deallocate(alloc, buffer, N);
    }

    // Multidimensional array, (range) overload.
    {
        constexpr int N = 5;
        constexpr int A = 3;
        constexpr int B = 3;

        using Array = Counted[A][B];
        std::allocator<Array> alloc;
        using Traits = std::allocator_traits<decltype(alloc)>;
        int counter = 0;

        Array* buffer = Traits::allocate(alloc, N);
        for (int i = 0; i != N; ++i) {
            Array& array_ref = *(buffer + i);
            for (int j = 0; j != A; ++j) {
                for (int k = 0; k != B; ++k) {
                    Traits::construct(
                        alloc, std::addressof(array_ref[j][k]), counter);
                }
            }
        }
        assert(counter == N * A * B);

        xranges::destroy(buffer, buffer + N);
        assert(counter == 0);

        Traits::deallocate(alloc, buffer, N);
    }

    return true;
}

int main(int, char**) {
    tests();
    test_arrays();

    static_assert(tests());
    // requires constexpr construct_at for arrays
    // static_assert(test_arrays());

    return 0;
}
