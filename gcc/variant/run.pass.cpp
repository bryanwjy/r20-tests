// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2016-2026 Free Software Foundation, Inc.
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

#include "../throw_allocator.h"
#include "rxx/variant.h"

#include <cassert>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

struct AlwaysThrow {
    AlwaysThrow() = default;

    AlwaysThrow(AlwaysThrow const&) { throw nullptr; }

    AlwaysThrow(AlwaysThrow&&) { throw nullptr; }

    AlwaysThrow& operator=(AlwaysThrow const&) {
        throw nullptr;
        return *this;
    }

    AlwaysThrow& operator=(AlwaysThrow&&) {
        throw nullptr;
        return *this;
    }

    bool operator<(AlwaysThrow const&) const { assert(false); }
    bool operator<=(AlwaysThrow const&) const { assert(false); }
    bool operator==(AlwaysThrow const&) const { assert(false); }
    bool operator!=(AlwaysThrow const&) const { assert(false); }
    bool operator>=(AlwaysThrow const&) const { assert(false); }
    bool operator>(AlwaysThrow const&) const { assert(false); }
};

struct DeletedMoves {
    DeletedMoves() = default;
    DeletedMoves(DeletedMoves const&) = default;
    DeletedMoves(DeletedMoves&&) = delete;
    DeletedMoves& operator=(DeletedMoves const&) = default;
    DeletedMoves& operator=(DeletedMoves&&) = delete;
};

void default_ctor() {
    __RXX variant<__RXX monostate, string> v;
    assert(holds_alternative<__RXX monostate>(v));
}

void copy_ctor() {
    __RXX variant<__RXX monostate, string> v("a");
    assert(holds_alternative<string>(v));
    __RXX variant<__RXX monostate, string> u(v);
    assert(holds_alternative<string>(u));
    assert(get<string>(u) == "a");
}

void move_ctor() {
    __RXX variant<__RXX monostate, string> v("a");
    assert(holds_alternative<string>(v));
    __RXX variant<__RXX monostate, string> u(std::move(v));
    assert(holds_alternative<string>(u));
    assert(get<string>(u) == "a");
    assert(holds_alternative<string>(v));

    __RXX variant<vector<int>, DeletedMoves> d{
        std::in_place_index<0>, {1, 2, 3, 4}
};
    // DeletedMoves is not move constructible, so this uses copy ctor:
    __RXX variant<vector<int>, DeletedMoves> e(std::move(d));
    assert(__RXX get<0>(d).size() == 4);
    assert(__RXX get<0>(e).size() == 4);
}

void arbitrary_ctor() {
    __RXX variant<int, string> v("a");
    assert(holds_alternative<string>(v));
    assert(get<1>(v) == "a");

    {
        // P0608R3
        __RXX variant<string, bool> x = "abc";
        assert(x.index() == 0);
    }

    {
        // P0608R3
        struct U {
            U(char16_t c) : c(c) {}
            char16_t c;
        };
        __RXX variant<char, U> x = u'\u2043';
        assert(x.index() == 1);
        assert(__RXX get<1>(x).c == u'\u2043');

        struct Double {
            Double(double& d) : d(d) {}
            double& d;
        };
        double d = 3.14;
        __RXX variant<int, Double> y = d;
        assert(y.index() == 1);
        assert(__RXX get<1>(y).d == d);
    }

    {
        // P0608R3
        __RXX variant<float, int> v1 = 'a';
        assert(__RXX get<1>(v1) == int('a'));
        __RXX variant<float, long> v2 = 0;
        assert(__RXX get<1>(v2) == 0L);
        struct big_int {
            big_int(int) {}
        };
        __RXX variant<float, big_int> v3 = 0;
        assert(v3.index() == 1);
    }

    {
        // P1957R2 Converting from T* to bool should be considered narrowing
        struct ConvertibleToBool {
            operator bool() const { return true; }
        };
        __RXX variant<bool> v1 = ConvertibleToBool();
        assert(__RXX get<0>(v1) == true);
        __RXX variant<bool, int> v2 = ConvertibleToBool();
        assert(__RXX get<0>(v2) == true);
        __RXX variant<int, bool> v3 = ConvertibleToBool();
        assert(__RXX get<1>(v3) == true);
    }
}

struct ThrowingMoveCtorThrowsCopyCtor {
    ThrowingMoveCtorThrowsCopyCtor() noexcept = default;
    ThrowingMoveCtorThrowsCopyCtor(ThrowingMoveCtorThrowsCopyCtor&&) {}
    ThrowingMoveCtorThrowsCopyCtor(ThrowingMoveCtorThrowsCopyCtor const&) {
        throw 0;
    }

    ThrowingMoveCtorThrowsCopyCtor& operator=(
        ThrowingMoveCtorThrowsCopyCtor&&) noexcept = default;
    ThrowingMoveCtorThrowsCopyCtor& operator=(
        ThrowingMoveCtorThrowsCopyCtor const&) noexcept = default;
};

void copy_assign() {
    __RXX variant<__RXX monostate, string> v("a");
    assert(holds_alternative<string>(v));
    __RXX variant<__RXX monostate, string> u;
    u = v;
    assert(holds_alternative<string>(u));
    assert(get<string>(u) == "a");
    {
        __RXX variant<int, ThrowingMoveCtorThrowsCopyCtor> v1,
            v2 = ThrowingMoveCtorThrowsCopyCtor();
        bool should_throw = false;
        try {
            v1 = v2;
        } catch (int) {
            should_throw = true;
        }
        assert(should_throw);
    }
}

void move_assign() {
    __RXX variant<__RXX monostate, string> v("a");
    assert(holds_alternative<string>(v));
    __RXX variant<__RXX monostate, string> u;
    u = std::move(v);
    assert(holds_alternative<string>(u));
    assert(get<string>(u) == "a");
    assert(holds_alternative<string>(v));

    __RXX variant<vector<int>, DeletedMoves> d{
        std::in_place_index<0>, {1, 2, 3, 4}
};
    __RXX variant<vector<int>, DeletedMoves> e;
    // DeletedMoves is not move assignable, so this uses copy assignment:
    e = std::move(d);
    assert(__RXX get<0>(d).size() == 4);
    assert(__RXX get<0>(e).size() == 4);
}

void arbitrary_assign() {
    __RXX variant<int, string> v;
    v = "a";

    assert(holds_alternative<string>(__RXX variant<int, string>("a")));
    assert(get<1>(v) == "a");

    {
        // P0608R3
        using T1 = __RXX variant<float, int>;
        T1 v1;
        v1 = 0;
        assert(v1.index() == 1);

        using T2 = __RXX variant<float, long>;
        T2 v2;
        v2 = 0;
        assert(v2.index() == 1);

        struct big_int {
            big_int(int) {}
        };
        using T3 = __RXX variant<float, big_int>;
        T3 v3;
        v3 = 0;
        assert(v3.index() == 1);
    }

    {
        // P1957R2 Converting from T* to bool should be considered narrowing
        struct ConvertibleToBool {
            operator bool() const { return true; }
        };
        __RXX variant<bool> v1;
        v1 = ConvertibleToBool();
        assert(__RXX get<0>(v1) == true);
        __RXX variant<bool, int> v2;
        v2 = ConvertibleToBool();
        assert(__RXX get<0>(v2) == true);
        __RXX variant<int, bool> v3;
        v3 = ConvertibleToBool();
        assert(__RXX get<1>(v3) == true);
    }
}

void dtor() {
    struct A {
        A(int& called) : called(called) {}
        ~A() { called++; }
        int& called;
    };
    {
        int called = 0;
        { __RXX variant<string, A> a(in_place_index<1>, called); }
        assert(called == 1);
    }
    {
        int called = 0;
        { __RXX variant<string, A> a(in_place_index<0>); }
        assert(called == 0);
    }
}

void in_place_index_ctor() {
    {
        __RXX variant<int, string> v(in_place_index<1>, "a");
        assert(holds_alternative<string>(v));
        assert(get<1>(v) == "a");
    }
    {
        __RXX variant<int, string> v(in_place_index<1>, {'a', 'b'});
        assert(holds_alternative<string>(v));
        assert(get<1>(v) == "ab");
    }
}

void in_place_type_ctor() {
    {
        __RXX variant<int, string> v(in_place_type<string>, "a");
        assert(holds_alternative<string>(v));
        assert(get<1>(v) == "a");
    }
    {
        __RXX variant<int, string> v(in_place_type<string>, {'a', 'b'});
        assert(holds_alternative<string>(v));
        assert(get<1>(v) == "ab");
    }
}

void emplace() {
    __RXX variant<int, string> v;
    v.emplace<0>(1);
    assert(get<0>(v) == 1);
    v.emplace<string>("a");
    assert(get<string>(v) == "a");
    v.emplace<1>({'a', 'b'});
    assert(get<1>(v) == "ab");
    v.emplace<string>({'a', 'c'});
    assert(get<string>(v) == "ac");
    {
        __RXX variant<int, AlwaysThrow> v;
        AlwaysThrow a;
        try {
            v.emplace<1>(a);
        } catch (nullptr_t) {}
        assert(v.valueless_by_exception());
        v.emplace<0>(42);
        assert(!v.valueless_by_exception());
    }
    {
        __RXX variant<int, AlwaysThrow> v;
        try {
            v.emplace<1>(AlwaysThrow{});
        } catch (nullptr_t) {}
        assert(v.valueless_by_exception());
        v.emplace<0>(42);
        assert(!v.valueless_by_exception());
    }
    assert(&v.emplace<0>(1) == &__RXX get<0>(v));
    assert(&v.emplace<int>(1) == &__RXX get<int>(v));
    assert(&v.emplace<1>("a") == &__RXX get<1>(v));
    assert(&v.emplace<string>("a") == &__RXX get<string>(v));
    {
        __RXX variant<vector<int>> v;
        assert(&v.emplace<0>({1, 2, 3}) == &__RXX get<0>(v));
        assert(
            &v.emplace<vector<int>>({1, 2, 3}) == &__RXX get<vector<int>>(v));
    }

    {
        // Ensure no copies of the vector are made, only moves.
        // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=87431#c21

        // static_assert(__detail::____RXX
        // variant::_Never_valueless_alt<vector<AlwaysThrow>>::value);
        __RXX variant<int, DeletedMoves, vector<AlwaysThrow>> v;
        v.emplace<2>(1);
        v.emplace<vector<AlwaysThrow>>(1);
        v.emplace<0>(0);

        // To test the emplace(initializer_list<U>, Args&&...) members we
        // can't use AlwaysThrow because elements in an initialier_list
        // are always copied. Use throw_allocator instead.
        using Vector = vector<int, throw_allocator_limit<int>>;
        __RXX variant<int, DeletedMoves, Vector> vv;
        Vector::allocator_type::set_limit(1);
        vv.emplace<2>(1, 1);
        Vector::allocator_type::set_limit(1);
        vv.emplace<Vector>(1, 1);
        Vector::allocator_type::set_limit(1);
        vv.emplace<0>(0);
        Vector::allocator_type::set_limit(1);
        vv.emplace<2>({1, 2, 3});
        Vector::allocator_type::set_limit(1);
        vv.emplace<Vector>({1, 2, 3, 4});
        try {
            Vector::allocator_type::set_limit(0);
            vv.emplace<2>(1, 1);
            assert(false);
        } catch (forced_error const&) {}
        assert(vv.valueless_by_exception());
    }
}

void test_get() {
    assert(get<1>(__RXX variant<int, string>("a")) == "a");
    assert(get<string>(__RXX variant<int, string>("a")) == "a");
    {
        bool caught = false;

        try {
            (void)get<0>(__RXX variant<int, string>("a"));
        } catch (__RXX bad_variant_access const&) {
            caught = true;
        }
        assert(caught);
    }
    {
        bool caught = false;

        try {
            (void)get<int>(__RXX variant<int, string>("a"));
        } catch (__RXX bad_variant_access const&) {
            caught = true;
        }
        assert(caught);
    }
}

void test_relational() {
    assert((__RXX variant<int, string>(2) < __RXX variant<int, string>(3)));
    assert((__RXX variant<int, string>(3) == __RXX variant<int, string>(3)));
    assert((__RXX variant<int, string>(3) > __RXX variant<int, string>(2)));
    assert((__RXX variant<int, string>(3) <= __RXX variant<int, string>(3)));
    assert((__RXX variant<int, string>(2) <= __RXX variant<int, string>(3)));
    assert((__RXX variant<int, string>(3) >= __RXX variant<int, string>(3)));
    assert((__RXX variant<int, string>(3) >= __RXX variant<int, string>(2)));
    assert((__RXX variant<int, string>(2) != __RXX variant<int, string>(3)));

    assert(
        (__RXX variant<int, string>(2) < __RXX variant<int, string>("a")));
    assert(
        (__RXX variant<string, int>(2) > __RXX variant<string, int>("a")));

    {
        __RXX variant<int, AlwaysThrow> v, w;
        try {
            AlwaysThrow a;
            v = a;
        } catch (nullptr_t) {}
        assert(v.valueless_by_exception());
        assert(v < w);
        assert(v <= w);
        assert(!(v == w));
        assert(v == v);
        assert(v != w);
        assert(w > v);
        assert(w >= v);
    }
}

void test_swap() {
    __RXX variant<int, string> a("a"), b("b");
    a.swap(b);
    assert(get<1>(a) == "b");
    assert(get<1>(b) == "a");
    swap(a, b);
    assert(get<1>(a) == "a");
    assert(get<1>(b) == "b");
}

void test_visit() {
    {
        struct Visitor {
            int operator()(int, float) { return 0; }
            int operator()(int, double) { return 1; }
            int operator()(char, float) { return 2; }
            int operator()(char, double) { return 3; }
            int operator()(int, float) const { return 5; }
            int operator()(int, double) const { return 6; }
            int operator()(char, float) const { return 7; }
            int operator()(char, double) const { return 8; }
        } visitor1;
        assert(visit(visitor1, __RXX variant<int, char>(1),
                   __RXX variant<float, double>(1.0f)) == 0);
        assert(visit(visitor1, __RXX variant<int, char>(1),
                   __RXX variant<float, double>(1.0)) == 1);
        assert(visit(visitor1, __RXX variant<int, char>('a'),
                   __RXX variant<float, double>(1.0f)) == 2);
        assert(visit(visitor1, __RXX variant<int, char>('a'),
                   __RXX variant<float, double>(1.0)) == 3);

        auto const& visitor2 = visitor1;
        assert(visit(visitor2, __RXX variant<int, char>(1),
                   __RXX variant<float, double>(1.0f)) == 5);
        assert(visit(visitor2, __RXX variant<int, char>(1),
                   __RXX variant<float, double>(1.0)) == 6);
        assert(visit(visitor2, __RXX variant<int, char>('a'),
                   __RXX variant<float, double>(1.0f)) == 7);
        assert(visit(visitor2, __RXX variant<int, char>('a'),
                   __RXX variant<float, double>(1.0)) == 8);
    }

    {
        struct Visitor {
            int operator()(int, float) && { return 0; }
            int operator()(int, double) && { return 1; }
            int operator()(char, float) && { return 2; }
            int operator()(char, double) && { return 3; }
        };
        assert(visit(Visitor{}, __RXX variant<int, char>(1),
                   __RXX variant<float, double>(1.0f)) == 0);
        assert(visit(Visitor{}, __RXX variant<int, char>(1),
                   __RXX variant<float, double>(1.0)) == 1);
        assert(visit(Visitor{}, __RXX variant<int, char>('a'),
                   __RXX variant<float, double>(1.0f)) == 2);
        assert(visit(Visitor{}, __RXX variant<int, char>('a'),
                   __RXX variant<float, double>(1.0)) == 3);
    }
}

struct Hashable {
    Hashable(char const* s) : s(s) {}
    // Non-trivial special member functions:
    Hashable(Hashable const&) {}
    Hashable(Hashable&&) noexcept {}
    ~Hashable() {}

    string s;

    bool operator==(Hashable const& rhs) const noexcept { return s == rhs.s; }
};

namespace std {
template <>
struct hash<Hashable> {
    size_t operator()(Hashable const& h) const noexcept {
        return hash<std::string>()(h.s);
    }
};
} // namespace std

void test_hash() {
    unordered_set<__RXX variant<int, Hashable>> s;
    assert(s.emplace(3).second);
    assert(s.emplace("asdf").second);
    assert(s.emplace().second);
    assert(s.size() == 3);
    assert(!s.emplace(3).second);
    assert(!s.emplace("asdf").second);
    assert(!s.emplace().second);
    assert(s.size() == 3);
    {
        struct A {
            operator Hashable() { throw nullptr; }
        };
        __RXX variant<int, Hashable> v;
        try {
            v.emplace<1>(A{});
        } catch (nullptr_t) {}
        assert(v.valueless_by_exception());
        assert(s.insert(v).second);
        assert(s.size() == 4);
        assert(!s.insert(v).second);
    }
}

void test_valueless_by_exception() {
    {
        AlwaysThrow a;
        bool caught = false;
        try {
            __RXX variant<int, AlwaysThrow> v(a);
        } catch (nullptr_t) {
            caught = true;
        }
        assert(caught);
    }
    {
        AlwaysThrow a;
        bool caught = false;
        try {
            __RXX variant<int, AlwaysThrow> v(a);
        } catch (nullptr_t) {
            caught = true;
        }
        assert(caught);
    }
    {
        __RXX variant<int, AlwaysThrow> v;
        bool caught = false;
        try {
            AlwaysThrow a;
            v = a;
        } catch (nullptr_t) {
            caught = true;
        }
        assert(caught);
        assert(v.valueless_by_exception());
    }
    {
        __RXX variant<int, AlwaysThrow> v;
        bool caught = false;
        try {
            v = AlwaysThrow{};
        } catch (nullptr_t) {
            caught = true;
        }
        assert(caught);
        assert(v.valueless_by_exception());
    }
}

int main() {
    default_ctor();
    copy_ctor();
    move_ctor();
    arbitrary_ctor();
    in_place_index_ctor();
    in_place_type_ctor();
    copy_assign();
    move_assign();
    arbitrary_assign();
    dtor();
    emplace();
    test_get();
    test_relational();
    test_swap();
    test_visit();
    test_hash();
    test_valueless_by_exception();
}
