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

#include "rxx/variant.h"

#include <string>
#include <vector>

using namespace std;

struct AllDeleted {
    AllDeleted() = delete;
    AllDeleted(AllDeleted const&) = delete;
    AllDeleted(AllDeleted&&) = delete;
    AllDeleted& operator=(AllDeleted const&) = delete;
    AllDeleted& operator=(AllDeleted&&) = delete;
};

struct Empty {
    Empty() {};
    Empty(Empty const&) {};
    Empty(Empty&&) {};
    Empty& operator=(Empty const&) { return *this; };
    Empty& operator=(Empty&&) { return *this; };
};

struct DefaultNoexcept {
    DefaultNoexcept() noexcept = default;
    DefaultNoexcept(DefaultNoexcept const&) noexcept = default;
    DefaultNoexcept(DefaultNoexcept&&) noexcept = default;
    DefaultNoexcept& operator=(DefaultNoexcept const&) noexcept = default;
    DefaultNoexcept& operator=(DefaultNoexcept&&) noexcept = default;
};

struct MoveCtorOnly {
    MoveCtorOnly() noexcept = delete;
    MoveCtorOnly(MoveCtorOnly const&) noexcept = delete;
    MoveCtorOnly(MoveCtorOnly&&) noexcept {}
    MoveCtorOnly& operator=(MoveCtorOnly const&) noexcept = delete;
    MoveCtorOnly& operator=(MoveCtorOnly&&) noexcept = delete;
};

struct MoveCtorAndSwapOnly : MoveCtorOnly {};
void swap(MoveCtorAndSwapOnly&, MoveCtorAndSwapOnly&) {}

struct DeletedMoves {
    DeletedMoves() = default;
    DeletedMoves(DeletedMoves const&) = default;
    DeletedMoves(DeletedMoves&&) = delete;
    DeletedMoves& operator=(DeletedMoves const&) = default;
    DeletedMoves& operator=(DeletedMoves&&) = delete;
};

struct nonliteral {
    nonliteral() {}

    bool operator<(nonliteral const&) const;
    bool operator<=(nonliteral const&) const;
    bool operator==(nonliteral const&) const;
    bool operator!=(nonliteral const&) const;
    bool operator>=(nonliteral const&) const;
    bool operator>(nonliteral const&) const;
};

struct virtual_default_dtor {
    virtual ~virtual_default_dtor() = default;
};

void default_ctor() {
    static_assert(is_default_constructible_v<__RXX variant<int, string>>);
    static_assert(is_default_constructible_v<__RXX variant<string, string>>);
    static_assert(
        !is_default_constructible_v<__RXX variant<AllDeleted, string>>);
    static_assert(
        is_default_constructible_v<__RXX variant<string, AllDeleted>>);
    static_assert(is_default_constructible_v<__RXX variant<DeletedMoves>>);

    static_assert(noexcept(__RXX variant<int>()));
    static_assert(!noexcept(__RXX variant<Empty>()));
    static_assert(noexcept(__RXX variant<DefaultNoexcept>()));
    { __RXX variant<virtual_default_dtor> a; }
}

void copy_ctor() {
    static_assert(is_copy_constructible_v<__RXX variant<int, string>>);
    static_assert(
        !is_copy_constructible_v<__RXX variant<AllDeleted, string>>);
    static_assert(is_trivially_copy_constructible_v<__RXX variant<int>>);
    static_assert(
        !is_trivially_copy_constructible_v<__RXX variant<std::string>>);
    static_assert(
        is_trivially_copy_constructible_v<__RXX variant<DeletedMoves>>);

    {
        __RXX variant<int> a;
        static_assert(noexcept(__RXX variant<int>(a)));
    }
    {
        __RXX variant<string> a;
        static_assert(!noexcept(__RXX variant<string>(a)));
    }
    {
        __RXX variant<int, string> a;
        static_assert(!noexcept(__RXX variant<int, string>(a)));
    }
    {
        __RXX variant<int, char> a;
        static_assert(noexcept(__RXX variant<int, char>(a)));
    }
}

void move_ctor() {
    static_assert(is_move_constructible_v<__RXX variant<int, string>>);
    static_assert(
        !is_move_constructible_v<__RXX variant<AllDeleted, string>>);
    static_assert(is_move_constructible_v<
        __RXX variant<int, DeletedMoves>>); // uses copy ctor
    static_assert(is_trivially_move_constructible_v<__RXX variant<int>>);
    static_assert(
        !is_trivially_move_constructible_v<__RXX variant<std::string>>);
    static_assert(!noexcept(
        __RXX variant<int, Empty>(declval<__RXX variant<int, Empty>>())));
    static_assert(noexcept(__RXX variant<int, DefaultNoexcept>(
        declval<__RXX variant<int, DefaultNoexcept>>())));
}

void arbitrary_ctor() {
    static_assert(
        !is_constructible_v<__RXX variant<string, string>, char const*>);
    static_assert(
        is_constructible_v<__RXX variant<int, string>, char const*>);
    static_assert(noexcept(__RXX variant<int, Empty>(int{})));
    static_assert(noexcept(__RXX variant<int, DefaultNoexcept>(int{})));
    static_assert(!noexcept(__RXX variant<int, Empty>(Empty{})));
    static_assert(
        noexcept(__RXX variant<int, DefaultNoexcept>(DefaultNoexcept{})));

    // P0608R3 disallow narrowing conversions and boolean conversions
    static_assert(!is_constructible_v<__RXX variant<float>, int>);
    static_assert(
        !is_constructible_v<__RXX variant<float, vector<int>>, int>);
    static_assert(is_constructible_v<__RXX variant<float, int>, char>);
    static_assert(!is_constructible_v<__RXX variant<float, char>, int>);
    static_assert(is_constructible_v<__RXX variant<float, long>, int>);
    struct big_int {
        big_int(int) {}
    };
    static_assert(is_constructible_v<__RXX variant<float, big_int>, int>);

    static_assert(!is_constructible_v<__RXX variant<int>, unsigned>);
    static_assert(!is_constructible_v<__RXX variant<bool>, int>);
    static_assert(!is_constructible_v<__RXX variant<bool>, void*>);

    // P1957R2 Converting from T* to bool should be considered narrowing
    struct ConvertibleToBool {
        operator bool() const { return true; }
    };
    static_assert(is_constructible_v<__RXX variant<bool>, ConvertibleToBool>);
    static_assert(
        is_constructible_v<__RXX variant<bool, int>, ConvertibleToBool>);
}

struct None {
    None() = delete;
};
struct Any {
    template <typename T>
    Any(T&&) {}
};

void in_place_index_ctor() {
    __RXX variant<string, string> a(in_place_index<0>, "a");
    __RXX variant<string, string> b(in_place_index<1>, {'a'});

    static_assert(!is_constructible_v<__RXX variant<None, Any>,
                      std::in_place_index_t<0>>,
        "PR libstdc++/90165");
}

void in_place_type_ctor() {
    __RXX variant<int, string, int> a(in_place_type<string>, "a");
    __RXX variant<int, string, int> b(in_place_type<string>, {'a'});
    static_assert(!is_constructible_v<__RXX variant<string, string>,
        in_place_type_t<string>, char const*>);
    static_assert(!is_constructible_v<__RXX variant<None, Any>,
                      std::in_place_type_t<None>>,
        "PR libstdc++/90165");
}

void dtor() {
    static_assert(is_destructible_v<__RXX variant<int, string>>);
    static_assert(is_destructible_v<__RXX variant<AllDeleted, string>>);
}

void copy_assign() {
    static_assert(is_copy_assignable_v<__RXX variant<int, string>>);
    static_assert(!is_copy_assignable_v<__RXX variant<AllDeleted, string>>);
    static_assert(is_trivially_copy_assignable_v<__RXX variant<int>>);
    static_assert(!is_trivially_copy_assignable_v<__RXX variant<string>>);
    static_assert(
        is_trivially_copy_assignable_v<__RXX variant<DeletedMoves>>);
    {
        __RXX variant<Empty> a;
        static_assert(!noexcept(a = a));
    }
    {
        __RXX variant<DefaultNoexcept> a;
        static_assert(noexcept(a = a));
    }
}

void move_assign() {
    static_assert(is_move_assignable_v<__RXX variant<int, string>>);
    static_assert(!is_move_assignable_v<__RXX variant<AllDeleted, string>>);
    static_assert(
        is_move_assignable_v<__RXX variant<int, DeletedMoves>>); // uses copy
                                                                   // assignment
    static_assert(is_trivially_move_assignable_v<__RXX variant<int>>);
    static_assert(!is_trivially_move_assignable_v<__RXX variant<string>>);
    {
        __RXX variant<Empty> a;
        static_assert(!noexcept(a = std::move(a)));
    }
    {
        __RXX variant<DefaultNoexcept> a;
        static_assert(noexcept(a = std::move(a)));
    }
}

void arbitrary_assign() {
    static_assert(
        !is_assignable_v<__RXX variant<string, string>, char const*>);
    static_assert(is_assignable_v<__RXX variant<int, string>, char const*>);
    static_assert(noexcept(__RXX variant<int, Empty>() = int{}));
    static_assert(noexcept(__RXX variant<int, DefaultNoexcept>() = int{}));
    static_assert(!noexcept(__RXX variant<int, Empty>() = Empty{}));
    static_assert(
        noexcept(__RXX variant<int, DefaultNoexcept>() = DefaultNoexcept{}));
}

void test_get() {
    static_assert(is_same<decltype(get<0>(__RXX variant<int, string>())),
        int&&>::value);
    static_assert(is_same<decltype(get<1>(__RXX variant<int, string>())),
        string&&>::value);
    static_assert(
        is_same<decltype(get<1>(__RXX variant<int, string const>())),
            string const&&>::value);

    static_assert(is_same<decltype(get<int>(__RXX variant<int, string>())),
        int&&>::value);
    static_assert(is_same<decltype(get<string>(__RXX variant<int, string>())),
        string&&>::value);
    static_assert(is_same<decltype(get<string const>(
                              __RXX variant<int, string const>())),
        string const&&>::value);
}

void test_relational() {
    {
        constexpr __RXX variant<int, nonliteral> a(42), b(43);
        static_assert((a < b));
        static_assert(!(a > b));
        static_assert((a <= b));
        static_assert(!(a == b));
        static_assert((a != b));
        static_assert(!(a >= b));
    }
    {
        constexpr __RXX variant<int, nonliteral> a(42), b(42);
        static_assert(!(a < b));
        static_assert(!(a > b));
        static_assert((a <= b));
        static_assert((a == b));
        static_assert(!(a != b));
        static_assert((a >= b));
    }
    {
        constexpr __RXX variant<int, nonliteral> a(43), b(42);
        static_assert(!(a < b));
        static_assert((a > b));
        static_assert(!(a <= b));
        static_assert(!(a == b));
        static_assert((a != b));
        static_assert((a >= b));
    }
    {
        constexpr __RXX monostate a, b;
        static_assert(!(a < b));
        static_assert(!(a > b));
        static_assert((a <= b));
        static_assert((a == b));
        static_assert(!(a != b));
        static_assert((a >= b));
    }
}

// swappable, and __RXX variant<C> not swappable via the generic std::swap.
struct C {
    C(C&&) = delete;
};
void swap(C&, C&) = delete;

static_assert(!std::is_swappable_v<__RXX variant<C>>);
static_assert(!std::is_swappable_v<__RXX variant<int, C>>);
static_assert(!std::is_swappable_v<__RXX variant<C, int>>);

// Not swappable, and __RXX variant<D> not swappable
struct D {
    D(D&&) = delete;
};

static_assert(!std::is_swappable_v<__RXX variant<D>>);
static_assert(!std::is_swappable_v<__RXX variant<int, D>>);
static_assert(!std::is_swappable_v<__RXX variant<D, int>>);

// swappable, swappable via exchange
struct E {};
static_assert(std::is_swappable_v<__RXX variant<E>>);
static_assert(std::is_swappable_v<__RXX variant<int, E>>);
static_assert(std::is_swappable_v<__RXX variant<E, int>>);

// Swappable, and __RXX variant<E2> not swappable even if E2 is swappable
struct E2 {
    E2(E2&&) = delete;
};
void swap(E2&, E2&);
static_assert(std::is_swappable_v<E2>);
static_assert(!std::is_swappable_v<__RXX variant<E2>>);
static_assert(!std::is_swappable_v<__RXX variant<int, E2>>);
static_assert(!std::is_swappable_v<__RXX variant<E2, int>>);

void test_swap() {
    static_assert(is_swappable_v<__RXX variant<int, string>>);
    static_assert(!is_swappable_v<__RXX variant<MoveCtorOnly>>);
    static_assert(is_swappable_v<__RXX variant<MoveCtorAndSwapOnly>>);
    static_assert(!is_swappable_v<__RXX variant<AllDeleted>>);
}

void test_visit() {
    {
        struct Visitor {
            void operator()(__RXX monostate) {}
            void operator()(int const&) {}
        };
        struct CVisitor {
            void operator()(__RXX monostate) const {}
            void operator()(int const&) const {}
        };
    }
    {
        struct Visitor {
            bool operator()(int, float) { return false; }
            bool operator()(int, double) { return false; }
            bool operator()(char, float) { return false; }
            bool operator()(char, double) { return false; }
        };
        visit(Visitor(), __RXX variant<int, char>(),
            __RXX variant<float, double>());
    }
    {
        struct Visitor {
            constexpr bool operator()(int const&) { return true; }
            constexpr bool operator()(nonliteral const&) { return false; }
        };
        static_assert(visit(Visitor(), __RXX variant<int, nonliteral>(0)));
    }
    {
        struct Visitor {
            constexpr bool operator()(int const&) { return true; }
            constexpr bool operator()(nonliteral const&) { return false; }
        };
        static_assert(visit(Visitor(), __RXX variant<int, nonliteral>(0)));
    }
    // PR libstdc++/79513
    {
        __RXX variant<int> v [[gnu::unused]] (5);
        __RXX visit([](int&) {}, v);
        __RXX visit([](int&&) {}, std::move(v));
    }
}

void test_constexpr() {
    constexpr __RXX variant<int> a;
    static_assert(holds_alternative<int>(a));
    constexpr __RXX variant<int, char> b(in_place_index<0>, int{});
    static_assert(holds_alternative<int>(b));
    constexpr __RXX variant<int, char> c(in_place_type<int>, int{});
    static_assert(holds_alternative<int>(c));
    constexpr __RXX variant<int, char> d(in_place_index<1>, char{});
    static_assert(holds_alternative<char>(d));
    constexpr __RXX variant<int, char> e(in_place_type<char>, char{});
    static_assert(holds_alternative<char>(e));
    constexpr __RXX variant<int, char> f(char{});
    static_assert(holds_alternative<char>(f));

    {
        struct literal {
            constexpr literal() = default;
        };

        constexpr __RXX variant<literal, nonliteral> v{};
        constexpr __RXX variant<literal, nonliteral> v1{
            in_place_type<literal>};
        constexpr __RXX variant<literal, nonliteral> v2{in_place_index<0>};
    }

    {
        constexpr __RXX variant<int> a(42);
        static_assert(get<0>(a) == 42);
    }
    {
        constexpr __RXX variant<int, nonliteral> a(42);
        static_assert(get<0>(a) == 42);
    }
    {
        constexpr __RXX variant<nonliteral, int> a(42);
        static_assert(get<1>(a) == 42);
    }
    {
        constexpr __RXX variant<int> a(42);
        static_assert(get<int>(a) == 42);
    }
    {
        constexpr __RXX variant<int, nonliteral> a(42);
        static_assert(get<int>(a) == 42);
    }
    {
        constexpr __RXX variant<nonliteral, int> a(42);
        static_assert(get<int>(a) == 42);
    }
    {
        constexpr __RXX variant<int> a(42);
        static_assert(get<0>(std::move(a)) == 42);
    }
    {
        constexpr __RXX variant<int, nonliteral> a(42);
        static_assert(get<0>(std::move(a)) == 42);
    }
    {
        constexpr __RXX variant<nonliteral, int> a(42);
        static_assert(get<1>(std::move(a)) == 42);
    }
    {
        constexpr __RXX variant<int> a(42);
        static_assert(get<int>(std::move(a)) == 42);
    }
    {
        constexpr __RXX variant<int, nonliteral> a(42);
        static_assert(get<int>(std::move(a)) == 42);
    }
    {
        constexpr __RXX variant<nonliteral, int> a(42);
        static_assert(get<int>(std::move(a)) == 42);
    }
}

void test_pr77641() {
    struct X {
        constexpr X() {}
    };

    constexpr __RXX variant<X> v1 = X{};
}

namespace adl_trap {
struct X {
    X() = default;
    X(int) {}
    X(std::initializer_list<int>, X const&) {}
};
template <typename T>
void move(T&) {}
template <typename T>
void forward(T&) {}

struct Visitor {
    template <typename T>
    void operator()(T&&) {}
};
} // namespace adl_trap

void test_adl() {
    using adl_trap::X;
    X x;
    std::initializer_list<int> il;
    adl_trap::Visitor vis;

    __RXX variant<X> v0(x);
    v0 = x;
    v0.emplace<0>(x);
    v0.emplace<0>(il, x);
    visit(vis, v0);
    __RXX variant<X> v1{in_place_index<0>, x};
    __RXX variant<X> v2{in_place_type<X>, x};
    __RXX variant<X> v3{in_place_index<0>, il, x};
    __RXX variant<X> v4{in_place_type<X>, il, x};
}

void test_variant_alternative() {
    static_assert(
        is_same_v<__RXX variant_alternative_t<0, __RXX variant<int, string>>,
            int>);
    static_assert(
        is_same_v<__RXX variant_alternative_t<1, __RXX variant<int, string>>,
            string>);

    static_assert(
        is_same_v<__RXX variant_alternative_t<0, __RXX variant<int> const>,
            int const>);
    static_assert(is_same_v<
        __RXX variant_alternative_t<0, __RXX variant<int> volatile>,
        int volatile>);
    static_assert(is_same_v<
        __RXX variant_alternative_t<0, __RXX variant<int> const volatile>,
        int const volatile>);
}

template <typename V, typename T>
constexpr auto has_type_emplace(int)
    -> decltype((declval<V>().template emplace<T>(), true)) {
    return true;
};

template <typename V, typename T>
constexpr bool has_type_emplace(...) {
    return false;
};

template <typename V, size_t N>
constexpr auto has_index_emplace(int)
    -> decltype((declval<V>().template emplace<N>(), true)) {
    return true;
};

template <typename V, size_t T>
constexpr bool has_index_emplace(...) {
    return false;
};

void test_emplace() {
    static_assert(has_type_emplace<__RXX variant<int>, int>(0));
    static_assert(!has_type_emplace<__RXX variant<long>, int>(0));
    static_assert(has_index_emplace<__RXX variant<int>, 0>(0));
    static_assert(
        !has_type_emplace<__RXX variant<AllDeleted>, AllDeleted>(0));
    static_assert(!has_index_emplace<__RXX variant<AllDeleted>, 0>(0));
    static_assert(has_type_emplace<__RXX variant<int, AllDeleted>, int>(0));
    static_assert(has_index_emplace<__RXX variant<int, AllDeleted>, 0>(0));
    static_assert(
        has_type_emplace<__RXX variant<int, vector<int>, AllDeleted>,
            vector<int>>(0));
    static_assert(
        has_index_emplace<__RXX variant<int, vector<int>, AllDeleted>, 1>(0));

    // The above tests only check the emplace members are available for
    // overload resolution. The following odr-uses will instantiate them:
    __RXX variant<int, vector<int>, AllDeleted> v;
    v.emplace<0>(1);
    v.emplace<int>(1);
    v.emplace<1>(1, 1);
    v.emplace<vector<int>>(1, 1);
    v.emplace<1>({1, 2, 3, 4});
    v.emplace<vector<int>>({1, 2, 3, 4});
}

void test_triviality() {
#define TEST_TEMPLATE(DT, CC, MC, CA, MA, CC_VAL, MC_VAL, CA_VAL, MA_VAL)   \
    []() {                                                                  \
        struct A {                                                          \
            ~A() DT;                                                        \
            A(const A&) CC;                                                 \
            A(A&&) MC;                                                      \
            A& operator=(const A&) CA;                                      \
            A& operator=(A&&) MA;                                           \
        };                                                                  \
        static_assert(                                                      \
            CC_VAL == is_trivially_copy_constructible_v<__RXX variant<A>>); \
        static_assert(                                                      \
            MC_VAL == is_trivially_move_constructible_v<__RXX variant<A>>); \
        static_assert(                                                      \
            CA_VAL == is_trivially_copy_assignable_v<__RXX variant<A>>);    \
        static_assert(                                                      \
            MA_VAL == is_trivially_move_assignable_v<__RXX variant<A>>);    \
    }()
    TEST_TEMPLATE(= default, = default, = default, = default, = default, true,
        true, true, true);
    TEST_TEMPLATE(
        = default, = default, = default, = default, , true, true, true, false);
    TEST_TEMPLATE(
        = default, = default, = default, , = default, true, true, false, true);
    TEST_TEMPLATE(
        = default, = default, = default, , , true, true, false, false);
    TEST_TEMPLATE(
        = default, = default, , = default, = default, true, false, true, false);
    TEST_TEMPLATE(
        = default, = default, , = default, , true, false, true, false);
    TEST_TEMPLATE(
        = default, = default, , , = default, true, false, false, false);
    TEST_TEMPLATE(= default, = default, , , , true, false, false, false);
    TEST_TEMPLATE(
        = default, , = default, = default, = default, false, true, false, true);
    TEST_TEMPLATE(
        = default, , = default, = default, , false, true, false, false);
    TEST_TEMPLATE(
        = default, , = default, , = default, false, true, false, true);
    TEST_TEMPLATE(= default, , = default, , , false, true, false, false);
    TEST_TEMPLATE(
        = default, , , = default, = default, false, false, false, false);
    TEST_TEMPLATE(= default, , , = default, , false, false, false, false);
    TEST_TEMPLATE(= default, , , , = default, false, false, false, false);
    TEST_TEMPLATE(= default, , , , , false, false, false, false);
    TEST_TEMPLATE(, = default, = default, = default, = default, false, false,
        false, false);
    TEST_TEMPLATE(
        , = default, = default, = default, , false, false, false, false);
    TEST_TEMPLATE(
        , = default, = default, , = default, false, false, false, false);
    TEST_TEMPLATE(, = default, = default, , , false, false, false, false);
    TEST_TEMPLATE(
        , = default, , = default, = default, false, false, false, false);
    TEST_TEMPLATE(, = default, , = default, , false, false, false, false);
    TEST_TEMPLATE(, = default, , , = default, false, false, false, false);
    TEST_TEMPLATE(, = default, , , , false, false, false, false);
    TEST_TEMPLATE(
        , , = default, = default, = default, false, false, false, false);
    TEST_TEMPLATE(, , = default, = default, , false, false, false, false);
    TEST_TEMPLATE(, , = default, , = default, false, false, false, false);
    TEST_TEMPLATE(, , = default, , , false, false, false, false);
    TEST_TEMPLATE(, , , = default, = default, false, false, false, false);
    TEST_TEMPLATE(, , , = default, , false, false, false, false);
    TEST_TEMPLATE(, , , , = default, false, false, false, false);
    TEST_TEMPLATE(, , , , , false, false, false, false);
#undef TEST_TEMPLATE

#define TEST_TEMPLATE(CC, MC, CA, MA)                                          \
    []() {                                                                     \
        struct A {                                                             \
            A(const A&) CC;                                                    \
            A(A&&) MC;                                                         \
            A& operator=(const A&) CA;                                         \
            A& operator=(A&&) MA;                                              \
        };                                                                     \
        static_assert(                                                         \
            !is_trivially_copy_constructible_v<__RXX variant<AllDeleted, A>>); \
        static_assert(                                                         \
            !is_trivially_move_constructible_v<__RXX variant<AllDeleted, A>>); \
        static_assert(                                                         \
            !is_trivially_copy_assignable_v<__RXX variant<AllDeleted, A>>);    \
        static_assert(                                                         \
            !is_trivially_move_assignable_v<__RXX variant<AllDeleted, A>>);    \
    }()
    TEST_TEMPLATE(= default, = default, = default, = default);
    TEST_TEMPLATE(= default, = default, = default, );
    TEST_TEMPLATE(= default, = default, , = default);
    TEST_TEMPLATE(= default, = default, , );
    TEST_TEMPLATE(= default, , = default, = default);
    TEST_TEMPLATE(= default, , = default, );
    TEST_TEMPLATE(= default, , , = default);
    TEST_TEMPLATE(= default, , , );
    TEST_TEMPLATE(, = default, = default, = default);
    TEST_TEMPLATE(, = default, = default, );
    TEST_TEMPLATE(, = default, , = default);
    TEST_TEMPLATE(, = default, , );
    TEST_TEMPLATE(, , = default, = default);
    TEST_TEMPLATE(, , = default, );
    TEST_TEMPLATE(, , , = default);
    TEST_TEMPLATE(, , , );
#undef TEST_TEMPLATE

    static_assert(is_trivially_copy_constructible_v<
        __RXX variant<DefaultNoexcept, int, char, float, double>>);
    static_assert(is_trivially_move_constructible_v<
        __RXX variant<DefaultNoexcept, int, char, float, double>>);
    static_assert(is_trivially_copy_assignable_v<
        __RXX variant<DefaultNoexcept, int, char, float, double>>);
    static_assert(is_trivially_move_assignable_v<
        __RXX variant<DefaultNoexcept, int, char, float, double>>);
}
