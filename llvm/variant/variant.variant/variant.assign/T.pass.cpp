// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <variant>

// template <class ...Types> class variant;

// template <class T>
// variant& operator=(T&&) noexcept(see below);

#include "../../../variant_test_helpers.h"
#include "rxx/variant.h"

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace MetaHelpers {

struct Dummy {
    Dummy() = default;
};

struct ThrowsCtorT {
    ThrowsCtorT(int) noexcept(false) {}
    ThrowsCtorT& operator=(int) noexcept { return *this; }
};

struct ThrowsAssignT {
    ThrowsAssignT(int) noexcept {}
    ThrowsAssignT& operator=(int) noexcept(false) { return *this; }
};

struct NoThrowT {
    NoThrowT(int) noexcept {}
    NoThrowT& operator=(int) noexcept { return *this; }
};

} // namespace MetaHelpers

namespace RuntimeHelpers {
#if RXX_WITH_EXCEPTIONS

struct ThrowsCtorT {
    int value;
    ThrowsCtorT() : value(0) {}
    ThrowsCtorT(int) noexcept(false) { throw 42; }
    ThrowsCtorT& operator=(int v) noexcept {
        value = v;
        return *this;
    }
};

struct MoveCrashes {
    int value;
    MoveCrashes(int v = 0) noexcept : value{v} {}
    MoveCrashes(MoveCrashes&&) noexcept { assert(false); }
    MoveCrashes& operator=(MoveCrashes&&) noexcept {
        assert(false);
        return *this;
    }
    MoveCrashes& operator=(int v) noexcept {
        value = v;
        return *this;
    }
};

struct ThrowsCtorTandMove {
    int value;
    ThrowsCtorTandMove() : value(0) {}
    ThrowsCtorTandMove(int) noexcept(false) { throw 42; }
    ThrowsCtorTandMove(ThrowsCtorTandMove&&) noexcept(false) { assert(false); }
    ThrowsCtorTandMove& operator=(int v) noexcept {
        value = v;
        return *this;
    }
};

struct ThrowsAssignT {
    int value;
    ThrowsAssignT() : value(0) {}
    ThrowsAssignT(int v) noexcept : value(v) {}
    ThrowsAssignT& operator=(int) noexcept(false) { throw 42; }
};

struct NoThrowT {
    int value;
    NoThrowT() : value(0) {}
    NoThrowT(int v) noexcept : value(v) {}
    NoThrowT& operator=(int v) noexcept {
        value = v;
        return *this;
    }
};

#endif
} // namespace RuntimeHelpers

constexpr void test_T_assignment_noexcept() {
    using namespace MetaHelpers;
    {
        using V = __RXX variant<Dummy, NoThrowT>;
        static_assert(std::is_nothrow_assignable<V, int>::value, "");
    }
    {
        using V = __RXX variant<Dummy, ThrowsCtorT>;
        static_assert(!std::is_nothrow_assignable<V, int>::value, "");
    }
    {
        using V = __RXX variant<Dummy, ThrowsAssignT>;
        static_assert(!std::is_nothrow_assignable<V, int>::value, "");
    }
}

constexpr void test_T_assignment_sfinae() {
    {
        using V = __RXX variant<long, long long>;
        static_assert(!std::is_assignable<V, int>::value, "ambiguous");
    }
    {
        using V = __RXX variant<std::string, std::string>;
        static_assert(!std::is_assignable<V, char const*>::value, "ambiguous");
    }
    {
        using V = __RXX variant<std::string, void*>;
        static_assert(
            !std::is_assignable<V, int>::value, "no matching operator=");
    }
    {
        using V = __RXX variant<std::string, float>;
        static_assert(
            !std::is_assignable<V, int>::value, "no matching operator=");
    }
    {
        using V = __RXX variant<std::unique_ptr<int>, bool>;
        static_assert(!std::is_assignable<V, std::unique_ptr<char>>::value,
            "no explicit bool in operator=");
        struct X {
            operator void*();
        };
        static_assert(!std::is_assignable<V, X>::value,
            "no boolean conversion in operator=");
        static_assert(std::is_assignable<V, std::false_type>::value,
            "converted to bool in operator=");
    }
    {
        struct X {};
        struct Y {
            operator X();
        };
        using V = __RXX variant<X>;
        static_assert(std::is_assignable<V, Y>::value,
            "regression on user-defined conversions in operator=");
    }
}

constexpr void test_T_assignment_basic() {
    {
        __RXX variant<int> v(43);
        v = 42;
        assert(v.index() == 0);
        assert(__RXX get<0>(v) == 42);
    }
    {
        __RXX variant<int, long> v(43l);
        v = 42;
        assert(v.index() == 0);
        assert(__RXX get<0>(v) == 42);
        v = 43l;
        assert(v.index() == 1);
        assert(__RXX get<1>(v) == 43);
    }
    {
        __RXX variant<unsigned, long> v;
        v = 42;
        assert(v.index() == 1);
        assert(__RXX get<1>(v) == 42);
        v = 43u;
        assert(v.index() == 0);
        assert(__RXX get<0>(v) == 43);
    }
    {
        __RXX variant<std::string, bool> v = true;
        v = "bar";
        assert(v.index() == 0);
        assert(__RXX get<0>(v) == "bar");
    }
}

void test_T_assignment_basic_no_constexpr() {
    __RXX variant<bool, std::unique_ptr<int>> v;
    v = nullptr;
    assert(v.index() == 1);
    assert(__RXX get<1>(v) == nullptr);
}

struct TraceStat {
    int construct = 0;
    int copy_construct = 0;
    int copy_assign = 0;
    int move_construct = 0;
    int move_assign = 0;
    int T_copy_assign = 0;
    int T_move_assign = 0;
    int destroy = 0;
};

template <bool CtorNoexcept, bool MoveCtorNoexcept>
struct Trace {
    struct T {};

    constexpr Trace(TraceStat* s) noexcept(CtorNoexcept) : stat(s) {
        ++s->construct;
    }
    constexpr Trace(T) noexcept(CtorNoexcept) : stat(nullptr) {}
    constexpr Trace(Trace const& o) : stat(o.stat) { ++stat->copy_construct; }
    constexpr Trace(Trace&& o) noexcept(MoveCtorNoexcept) : stat(o.stat) {
        ++stat->move_construct;
    }
    constexpr Trace& operator=(Trace const&) {
        ++stat->copy_assign;
        return *this;
    }
    constexpr Trace& operator=(Trace&&) noexcept {
        ++stat->move_assign;
        return *this;
    }

    constexpr Trace& operator=(T const&) {
        ++stat->T_copy_assign;
        return *this;
    }
    constexpr Trace& operator=(T&&) noexcept {
        ++stat->T_move_assign;
        return *this;
    }
    constexpr ~Trace() { ++stat->destroy; }

    TraceStat* stat;
};

constexpr void test_T_assignment_performs_construction() {
    {
        using V = __RXX variant<int, Trace<false, false>>;
        TraceStat stat;
        V v{1};
        v = &stat;
        assert(stat.construct == 1);
        assert(stat.copy_construct == 0);
        assert(stat.move_construct == 0);
        assert(stat.copy_assign == 0);
        assert(stat.move_assign == 0);
        assert(stat.destroy == 0);
    }
    {
        using V = __RXX variant<int, Trace<false, true>>;
        TraceStat stat;
        V v{1};
        v = &stat;
        assert(stat.construct == 1);
        assert(stat.copy_construct == 0);
        assert(stat.move_construct == 1);
        assert(stat.copy_assign == 0);
        assert(stat.move_assign == 0);
        assert(stat.destroy == 1);
    }

    {
        using V = __RXX variant<int, Trace<true, false>>;
        TraceStat stat;
        V v{1};
        v = &stat;
        assert(stat.construct == 1);
        assert(stat.copy_construct == 0);
        assert(stat.move_construct == 0);
        assert(stat.copy_assign == 0);
        assert(stat.move_assign == 0);
        assert(stat.destroy == 0);
    }

    {
        using V = __RXX variant<int, Trace<true, true>>;
        TraceStat stat;
        V v{1};
        v = &stat;
        assert(stat.construct == 1);
        assert(stat.copy_construct == 0);
        assert(stat.move_construct == 0);
        assert(stat.copy_assign == 0);
        assert(stat.move_assign == 0);
        assert(stat.destroy == 0);
    }
}

constexpr void test_T_assignment_performs_assignment() {
    {
        using V = __RXX variant<int, Trace<false, false>>;
        TraceStat stat;
        V v{&stat};
        v = Trace<false, false>::T{};
        assert(stat.construct == 1);
        assert(stat.copy_construct == 0);
        assert(stat.move_construct == 0);
        assert(stat.copy_assign == 0);
        assert(stat.move_assign == 0);
        assert(stat.T_copy_assign == 0);
        assert(stat.T_move_assign == 1);
        assert(stat.destroy == 0);
    }
    {
        using V = __RXX variant<int, Trace<false, false>>;
        TraceStat stat;
        V v{&stat};
        Trace<false, false>::T t;
        v = t;
        assert(stat.construct == 1);
        assert(stat.copy_construct == 0);
        assert(stat.move_construct == 0);
        assert(stat.copy_assign == 0);
        assert(stat.move_assign == 0);
        assert(stat.T_copy_assign == 1);
        assert(stat.T_move_assign == 0);
        assert(stat.destroy == 0);
    }
}

void test_T_assignment_performs_construction_throw() {
    using namespace RuntimeHelpers;
#if RXX_WITH_EXCEPTIONS
    {
        using V = __RXX variant<std::string, ThrowsCtorT>;
        V v(std::in_place_type<std::string>, "hello");
        try {
            v = 42;
            assert(false);
        } catch (...) { /* ... */
        }
        assert(v.index() == 0);
        assert(__RXX get<0>(v) == "hello");
    }
    {
        using V = __RXX variant<ThrowsAssignT, std::string>;
        V v(std::in_place_type<std::string>, "hello");
        v = 42;
        assert(v.index() == 0);
        assert(__RXX get<0>(v).value == 42);
    }
#endif // TEST_HAS_NO_EXCEPTIONS
}

void test_T_assignment_performs_assignment_throw() {
    using namespace RuntimeHelpers;
#if RXX_WITH_EXCEPTIONS
    {
        using V = __RXX variant<ThrowsCtorT>;
        V v;
        v = 42;
        assert(v.index() == 0);
        assert(__RXX get<0>(v).value == 42);
    }
    {
        using V = __RXX variant<ThrowsCtorT, std::string>;
        V v;
        v = 42;
        assert(v.index() == 0);
        assert(__RXX get<0>(v).value == 42);
    }
    {
        using V = __RXX variant<ThrowsAssignT>;
        V v(100);
        try {
            v = 42;
            assert(false);
        } catch (...) { /* ... */
        }
        assert(v.index() == 0);
        assert(__RXX get<0>(v).value == 100);
    }
    {
        using V = __RXX variant<std::string, ThrowsAssignT>;
        V v(100);
        try {
            v = 42;
            assert(false);
        } catch (...) { /* ... */
        }
        assert(v.index() == 1);
        assert(__RXX get<1>(v).value == 100);
    }
#endif // TEST_HAS_NO_EXCEPTIONS
}

constexpr void test_T_assignment_vector_bool() {
    std::vector<bool> vec = {true};
    __RXX variant<bool, int> v;
    v = vec[0];
    assert(v.index() == 0);
    assert(__RXX get<0>(v) == true);
}

void non_constexpr_test() {
    test_T_assignment_basic_no_constexpr();
    test_T_assignment_performs_construction_throw();
    test_T_assignment_performs_assignment_throw();
}

constexpr bool test() {
    test_T_assignment_basic();
    test_T_assignment_performs_construction();
    test_T_assignment_performs_assignment();
    test_T_assignment_noexcept();
    test_T_assignment_sfinae();
    test_T_assignment_vector_bool();

    return true;
}

int main(int, char**) {
    test();
    non_constexpr_test();

    static_assert(test());
    return 0;
}
