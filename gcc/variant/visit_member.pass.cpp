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

// { dg-do run { target c++26 } }

#include "rxx/variant.h"

#include <cassert>
#include <functional> // reference_wrapper

void test01() {
    // Verify that visitation uses INVOKE and supports a pointer-to-member.
    struct X {
        int n;
    };
    using V = __RXX variant<X, X*, std::reference_wrapper<X>>;
    struct Derv : private V {
        using V::V;
        using V::visit;
    };

    Derv v{X{1}};
    static_assert(std::is_same_v<decltype(v.visit(&X::n)), int&>);

    // Verify that constness and value category are correctly forwarded.
    __RXX variant<int> v2{1};
    auto id = []<typename T>(T&& t) -> T&& { return std::forward<T>(t); };
    static_assert(std::is_same_v<decltype(v2.visit(id)), int&>);
    static_assert(std::is_same_v<decltype(std::move(v2).visit(id)), int&&>);
    auto const& vc = v2;
    static_assert(std::is_same_v<decltype(vc.visit(id)), int const&>);
    static_assert(
        std::is_same_v<decltype(std::move(vc).visit(id)), int const&&>);

    static_assert(std::is_same_v<decltype(vc.visit<void>(id)), void>);
}

void test02() {
    struct NoCopy {
        NoCopy() {}
        NoCopy(NoCopy const&) = delete;
        NoCopy(NoCopy&&) = delete;
        ~NoCopy() {}

        int operator()(int i) { return i; }
        int operator()(NoCopy const&) { return 100; }
    };

    __RXX variant<NoCopy, int> v{10};
    NoCopy f;
    // Visit should not need arguments to be copyable:
    int res = v.visit(f);
    assert(res == 10);
    v.emplace<NoCopy>();
    res = v.visit(f);
    assert(res == 100);
    res = v.visit<bool>(f);
    assert(res == 1);
}

void test03() {
    // Verify that member visit can access the variant as a private base class.
    struct Derived : private __RXX variant<int> {
        using variant::visit;
    };
    Derived d;
    int i = d.visit([](int& x) { return --x; });
    assert(i == -1);
    unsigned u = d.visit<unsigned>([](int x) { return x; });
    assert(u == -1u);
}

void test04() {
    struct A {
        char a = 'a';
    };
    struct B {
        char b = 'b';
    };
    struct C {
        char c = 'c';
    };
    auto f = [](auto x) { return B{}; };
    using State = __RXX variant<A, B, C>;
    State state = A{};
    auto res = std::move(state).visit<State>(f);
    // Verify that visit<R> only matches the explicit return type overload.
    static_assert(std::is_same_v<decltype(res), State>);
}

int main() {
    test01();
    test02();
    test03();
    test04();
}
