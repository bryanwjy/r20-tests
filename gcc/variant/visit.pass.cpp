// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite
// Copyright (C) 2019-2026 Free Software Foundation, Inc.
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

// { dg-do run { target c++17 } }

#include "rxx/variant.h"

#include <cassert>
#include <functional> // reference_wrapper

// N.B. there are more __RXX visit tests in ./compile.cc and ./run.cc

void test01() {
    // Verify that visitation uses INVOKE and supports arbitrary callables.

    struct X {
        int sum(int i) const { return i + n; }
        int product(int i) const { return i * n; }
        int n;
    };

    __RXX variant<X, X*, std::reference_wrapper<X>> vobj{X{1}};
    int res = __RXX visit(&X::n, vobj);
    assert(res == 1);

    __RXX variant<int, short> varg{2};
    res = __RXX visit(&X::sum, vobj, varg);
    assert(res == 3);

    X x{4};
    vobj = &x;
    res = __RXX visit(&X::n, vobj);
    assert(res == 4);

    varg.emplace<short>(5);
    res = __RXX visit(&X::sum, vobj, varg);
    assert(res == 9);

    x.n = 6;
    res = __RXX visit(&X::product, vobj, varg);
    assert(res == 30);

    vobj = std::ref(x);
    x.n = 7;
    res = __RXX visit(&X::n, vobj);
    assert(res == 7);

    res = __RXX visit(&X::product, vobj, varg);
    assert(res == 35);
}

void test02() {
    struct NoCopy {
        NoCopy() {}
        NoCopy(NoCopy const&) = delete;
        NoCopy(NoCopy&&) = delete;
        ~NoCopy() {}

        int operator()(int i) { return i; }
        int operator()(NoCopy const&) { return 0; }
    };

    __RXX variant<NoCopy, int> v{1};
    NoCopy f;
    // Visit should not need arguments to be copyable:
    int res = __RXX visit(f, v);
    assert(res == 1);
    v.emplace<NoCopy>();
    res = __RXX visit(f, v);
    assert(res == 0);
}

int main() {
    test01();
    test02();
}
