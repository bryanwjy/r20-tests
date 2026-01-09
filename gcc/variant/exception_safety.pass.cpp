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

#include <any>
#include <cassert>
#include <functional>
#include <memory>
#include <memory_resource>
#include <optional>
#include <string>
#include <vector>

void test01() {
    __RXX variant<int, std::pmr::string, std::pmr::vector<int>> v(1);
    assert(v.index() == 0);

    try {
        std::pmr::string s = "how long is a piece of SSO string?";
        v.emplace<1>(s, std::pmr::null_memory_resource());
        assert(false);
    } catch (std::bad_alloc const&) {
        assert(v.valueless_by_exception());
    }

    v.emplace<0>(2);
    assert(v.index() == 0);

    try {
        v.emplace<2>({1, 2, 3}, std::pmr::null_memory_resource());
        assert(false);
    } catch (std::bad_alloc const&) {
        assert(v.valueless_by_exception());
    }
}

void test02() {
    struct X {
        X(int i) : i(1) {
            if (i > 2)
                throw 3;
        }
        X(std::initializer_list<int> l) : i(2) {
            if (l.size() > 2)
                throw 3;
        }
        int i;
    };
    static_assert(std::is_trivially_copyable_v<X>);

    __RXX variant<__RXX monostate, int, X> v(111);
    assert(v.index() == 1);

    try {
        v.emplace<X>(3);
        assert(false);
    } catch (int) {
        assert(!v.valueless_by_exception());
        assert(v.index() == 1);
        assert(__RXX get<int>(v) == 111);
    }

    v.emplace<X>(1);
    assert(v.index() == 2);
    assert(__RXX get<X>(v).i == 1);

    try {
        v.emplace<X>(3);
        assert(false);
    } catch (int) {
        assert(!v.valueless_by_exception());
        assert(v.index() == 2);
        assert(__RXX get<X>(v).i == 1);
    }

    try {
        v.emplace<X>({1, 2, 3});
        assert(false);
    } catch (int) {
        assert(!v.valueless_by_exception());
        assert(v.index() == 2);
        assert(__RXX get<X>(v).i == 1);
    }
}

template <typename T, typename V>
bool bad_emplace(V& v) {
    struct X {
        operator T() const { throw 1; }
    };

    auto const index = v.index();

    try {
        if constexpr (std::is_same_v<T, std::any>) {
            // Need to test std::any differently, because emplace<std::any>(X{})
            // would create a std::any with a contained X, instead of using
            // X::operator any() to convert to std::any.
            struct ThrowOnCopy {
                ThrowOnCopy() {}
                ThrowOnCopy(ThrowOnCopy const&) { throw 1; }
            } t;
            v.template emplace<std::any>(t);
        } else
            v.template emplace<T>(X{});
    } catch (int) {
        return v.index() == index;
    }
    return false;
}

void test03() {
    struct TriviallyCopyable {
        int i = 0;
    };

    __RXX variant<__RXX monostate, int, TriviallyCopyable,
        std::optional<int>, std::string, std::vector<int>,
        std::function<void()>, std::any, std::shared_ptr<int>,
        std::weak_ptr<int>, std::unique_ptr<int>>
        v(1);
    assert(v.index() == 1);

    assert(bad_emplace<int>(v));
    assert(bad_emplace<TriviallyCopyable>(v));
    assert(bad_emplace<std::optional<int>>(v));

    // These are libstdc++ specific behaviours

    // assert(bad_emplace<std::string>(v));
    // assert(bad_emplace<std::vector<int>>(v));
    // assert(bad_emplace<std::function<void()>>(v));
    // assert(bad_emplace<std::any>(v));
    // assert(bad_emplace<std::shared_ptr<int>>(v));
    // assert(bad_emplace<std::weak_ptr<int>>(v));
    // assert(bad_emplace<std::unique_ptr<int>>(v));
}

void test04() {
    // LWG 2904. Make variant move-assignment more exception safe

    struct ThrowOnCopy {
        ThrowOnCopy() {}
        ThrowOnCopy(ThrowOnCopy const&) { throw 1; }
        ThrowOnCopy& operator=(ThrowOnCopy const&) { throw "shouldn't happen"; }
        ThrowOnCopy(ThrowOnCopy&&) noexcept {}
    };

    __RXX variant<int, ThrowOnCopy> v1(std::in_place_type<ThrowOnCopy>),
        v2(2);
    try {
        v2 = v1; // uses variant<Types...>::operator=(const variant&)
        assert(false);
    } catch (int) {
        assert(!v2.valueless_by_exception());
        assert(v2.index() == 0);
        assert(__RXX get<0>(v2) == 2);
    }

    try {
        ThrowOnCopy toc;
        v2 = toc; // uses variant<Types...>::operator=(T&&)
        assert(false);
    } catch (int) {
        assert(!v2.valueless_by_exception());
        assert(v2.index() == 0);
        assert(__RXX get<0>(v2) == 2);
    }
}

int main() {
    test01();
    test02();
    test03();
    test04();
}
