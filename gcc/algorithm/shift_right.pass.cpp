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

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include "../test_iterators.h"
#include "rxx/algorithm/shift.h"

#include <cassert>

using rxx::tests::bidirectional_iterator_wrapper;
using rxx::tests::forward_iterator_wrapper;
using rxx::tests::random_access_iterator_wrapper;
using rxx::tests::test_range;

namespace xranges = rxx::ranges;

struct X {
    int a = -1;
    bool moved_from = false;

    X() = default;

    X(int a) : a(a) {}

    X(X const&) = delete;
    X& operator=(X const&) = delete;

    X(X&& other) {
        if (this != &other)
            *this = std::move(other);
    }

    X& operator=(X&& other) {
        a = other.a;
        other.moved_from = true;
        moved_from = false;
        return *this;
    }
};

template <int N, template <typename> typename Wrapper>
void test01() {
    for (int n = 0; n < N + 5; n++) {
        X x[N];
        for (int i = 0; i < N; i++)
            x[i] = X(i);
        test_range<X, Wrapper> rx(x);
        auto [out, last] = xranges::shift_right(rx.begin(), rx.end(), n);
        assert(last == rx.end());
        if (n < N) {
            assert(out.ptr == x + n);
            for (int i = n; i < N; i++)
                assert(x[i].a == i - n);
            for (int i = 0; i < std::min(n, N - n); i++)
                assert(x[i].moved_from);
            for (int i = std::min(n, N - n); i < std::max(n, N - n); i++)
                assert(!x[i].moved_from);
        } else {
            assert(out.ptr == x + N);
            for (int i = 0; i < N; i++) {
                assert(x[i].a == i);
                assert(!x[i].moved_from);
            }
        }
    }
}

int main() {
    test01<23, forward_iterator_wrapper>();
    test01<23, bidirectional_iterator_wrapper>();
    test01<23, random_access_iterator_wrapper>();

    test01<24, forward_iterator_wrapper>();
    test01<24, bidirectional_iterator_wrapper>();
    test01<24, random_access_iterator_wrapper>();
}
