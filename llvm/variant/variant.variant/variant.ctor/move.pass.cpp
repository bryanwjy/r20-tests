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

// constexpr variant(variant&&) noexcept(see below);

#include "../../../test_workarounds.h"
#include "rxx/variant.h"

#include <cassert>
#include <string>
#include <type_traits>

struct ThrowsMove {
    ThrowsMove(ThrowsMove&&) noexcept(false) {}
};

struct NoCopy {
    NoCopy(NoCopy const&) = delete;
};

struct MoveOnly {
    int value;
    constexpr MoveOnly(int v) : value(v) {}
    MoveOnly(MoveOnly const&) = delete;
    MoveOnly(MoveOnly&&) = default;
};

struct MoveOnlyNT {
    int value;
    constexpr MoveOnlyNT(int v) : value(v) {}
    MoveOnlyNT(MoveOnlyNT const&) = delete;
    constexpr MoveOnlyNT(MoveOnlyNT&& other) : value(other.value) {
        other.value = -1;
    }
};

struct NTMove {
    constexpr NTMove(int v) : value(v) {}
    NTMove(NTMove const&) = delete;
    NTMove(NTMove&& that) : value(that.value) { that.value = -1; }
    int value;
};

static_assert(!std::is_trivially_move_constructible<NTMove>::value, "");
static_assert(std::is_move_constructible<NTMove>::value, "");

struct TMove {
    constexpr TMove(int v) : value(v) {}
    TMove(TMove const&) = delete;
    TMove(TMove&&) = default;
    int value;
};

static_assert(std::is_trivially_move_constructible<TMove>::value, "");

struct TMoveNTCopy {
    constexpr TMoveNTCopy(int v) : value(v) {}
    TMoveNTCopy(TMoveNTCopy const& that) : value(that.value) {}
    TMoveNTCopy(TMoveNTCopy&&) = default;
    int value;
};

static_assert(std::is_trivially_move_constructible<TMoveNTCopy>::value, "");

#if RXX_WITH_EXCEPTIONS
struct MakeEmptyT {
    static int alive;
    MakeEmptyT() { ++alive; }
    MakeEmptyT(MakeEmptyT const&) {
        ++alive;
        // Don't throw from the copy constructor since variant's assignment
        // operator performs a copy before committing to the assignment.
    }
    MakeEmptyT(MakeEmptyT&&) { throw 42; }
    MakeEmptyT& operator=(MakeEmptyT const&) { throw 42; }
    MakeEmptyT& operator=(MakeEmptyT&&) { throw 42; }
    ~MakeEmptyT() { --alive; }
};

int MakeEmptyT::alive = 0;

template <class Variant>
void makeEmpty(Variant& v) {
    Variant v2(std::in_place_type<MakeEmptyT>);
    try {
        v = std::move(v2);
        assert(false);
    } catch (...) {
        assert(v.valueless_by_exception());
    }
}
#endif // RXX_WITH_EXCEPTIONS

constexpr void test_move_noexcept() {
    {
        using V = __RXX variant<int, long>;
        static_assert(std::is_nothrow_move_constructible<V>::value, "");
    }
    {
        using V = __RXX variant<int, MoveOnly>;
        static_assert(std::is_nothrow_move_constructible<V>::value, "");
    }
    {
        using V = __RXX variant<int, MoveOnlyNT>;
        static_assert(!std::is_nothrow_move_constructible<V>::value, "");
    }
    {
        using V = __RXX variant<int, ThrowsMove>;
        static_assert(!std::is_nothrow_move_constructible<V>::value, "");
    }
}

constexpr void test_move_ctor_sfinae() {
    {
        using V = __RXX variant<int, long>;
        static_assert(std::is_move_constructible<V>::value, "");
    }
    {
        using V = __RXX variant<int, MoveOnly>;
        static_assert(std::is_move_constructible<V>::value, "");
    }
    {
        using V = __RXX variant<int, MoveOnlyNT>;
        static_assert(std::is_move_constructible<V>::value, "");
    }
    {
        using V = __RXX variant<int, NoCopy>;
        static_assert(!std::is_move_constructible<V>::value, "");
    }

    // Make sure we properly propagate triviality (see P0602R4).
    {
        using V = __RXX variant<int, long>;
        static_assert(std::is_trivially_move_constructible<V>::value, "");
    }
    {
        using V = __RXX variant<int, NTMove>;
        static_assert(!std::is_trivially_move_constructible<V>::value, "");
        static_assert(std::is_move_constructible<V>::value, "");
    }
    {
        using V = __RXX variant<int, TMove>;
        static_assert(std::is_trivially_move_constructible<V>::value, "");
    }
    {
        using V = __RXX variant<int, TMoveNTCopy>;
        static_assert(std::is_trivially_move_constructible<V>::value, "");
    }
}

template <typename T>
struct Result {
    std::size_t index;
    T value;
};

constexpr void test_move_ctor_basic() {
    {
        __RXX variant<int> v(std::in_place_index<0>, 42);
        __RXX variant<int> v2 = std::move(v);
        assert(v2.index() == 0);
        assert(__RXX get<0>(v2) == 42);
    }
    {
        __RXX variant<int, long> v(std::in_place_index<1>, 42);
        __RXX variant<int, long> v2 = std::move(v);
        assert(v2.index() == 1);
        assert(__RXX get<1>(v2) == 42);
    }
    {
        __RXX variant<MoveOnly> v(std::in_place_index<0>, 42);
        assert(v.index() == 0);
        __RXX variant<MoveOnly> v2(std::move(v));
        assert(v2.index() == 0);
        assert(__RXX get<0>(v2).value == 42);
    }
    {
        __RXX variant<int, MoveOnly> v(std::in_place_index<1>, 42);
        assert(v.index() == 1);
        __RXX variant<int, MoveOnly> v2(std::move(v));
        assert(v2.index() == 1);
        assert(__RXX get<1>(v2).value == 42);
    }
    {
        __RXX variant<MoveOnlyNT> v(std::in_place_index<0>, 42);
        assert(v.index() == 0);
        __RXX variant<MoveOnlyNT> v2(std::move(v));
        assert(v2.index() == 0);
        assert(__RXX get<0>(v).value == -1);
        assert(__RXX get<0>(v2).value == 42);
    }
    {
        __RXX variant<int, MoveOnlyNT> v(std::in_place_index<1>, 42);
        assert(v.index() == 1);
        __RXX variant<int, MoveOnlyNT> v2(std::move(v));
        assert(v2.index() == 1);
        assert(__RXX get<1>(v).value == -1);
        assert(__RXX get<1>(v2).value == 42);
    }

    // Make sure we properly propagate triviality, which implies constexpr-ness
    // (see P0602R4).
    {
        struct {
            constexpr Result<int> operator()() const {
                __RXX variant<int> v(std::in_place_index<0>, 42);
                __RXX variant<int> v2 = std::move(v);
                return {v2.index(), __RXX get<0>(std::move(v2))};
            }
        } test;
        constexpr auto result = test();
        static_assert(result.index == 0, "");
        static_assert(result.value == 42, "");
    }
    {
        struct {
            constexpr Result<long> operator()() const {
                __RXX variant<int, long> v(std::in_place_index<1>, 42);
                __RXX variant<int, long> v2 = std::move(v);
                return {v2.index(), __RXX get<1>(std::move(v2))};
            }
        } test;
        constexpr auto result = test();
        static_assert(result.index == 1, "");
        static_assert(result.value == 42, "");
    }
    {
        struct {
            constexpr Result<TMove> operator()() const {
                __RXX variant<TMove> v(std::in_place_index<0>, 42);
                __RXX variant<TMove> v2(std::move(v));
                return {v2.index(), __RXX get<0>(std::move(v2))};
            }
        } test;
        constexpr auto result = test();
        static_assert(result.index == 0, "");
        static_assert(result.value.value == 42, "");
    }
    {
        struct {
            constexpr Result<TMove> operator()() const {
                __RXX variant<int, TMove> v(std::in_place_index<1>, 42);
                __RXX variant<int, TMove> v2(std::move(v));
                return {v2.index(), __RXX get<1>(std::move(v2))};
            }
        } test;
        constexpr auto result = test();
        static_assert(result.index == 1, "");
        static_assert(result.value.value == 42, "");
    }
    {
        struct {
            constexpr Result<TMoveNTCopy> operator()() const {
                __RXX variant<TMoveNTCopy> v(std::in_place_index<0>, 42);
                __RXX variant<TMoveNTCopy> v2(std::move(v));
                return {v2.index(), __RXX get<0>(std::move(v2))};
            }
        } test;
        constexpr auto result = test();
        static_assert(result.index == 0, "");
        static_assert(result.value.value == 42, "");
    }
    {
        struct {
            constexpr Result<TMoveNTCopy> operator()() const {
                __RXX variant<int, TMoveNTCopy> v(std::in_place_index<1>, 42);
                __RXX variant<int, TMoveNTCopy> v2(std::move(v));
                return {v2.index(), __RXX get<1>(std::move(v2))};
            }
        } test;
        constexpr auto result = test();
        static_assert(result.index == 1, "");
        static_assert(result.value.value == 42, "");
    }
}

void test_move_ctor_valueless_by_exception() {
#if RXX_WITH_EXCEPTIONS
    using V = __RXX variant<int, MakeEmptyT>;
    V v1;
    makeEmpty(v1);
    V v(std::move(v1));
    assert(v.valueless_by_exception());
#endif // RXX_WITH_EXCEPTIONS
}

template <std::size_t Idx, class T>
constexpr void test_constexpr_ctor_imp(T const& v) {
    auto copy = v;
    auto v2 = std::move(copy);
    assert(v2.index() == v.index());
    assert(v2.index() == Idx);
    assert(__RXX get<Idx>(v2) == __RXX get<Idx>(v));
}

constexpr void test_constexpr_move_ctor_trivial() {
    // Make sure we properly propagate triviality, which implies constexpr-ness
    // (see P0602R4).
    using V = __RXX variant<long, void*, int const>;
#if defined(TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE) || \
    (RXX_COMPILER_GCC && !RXX_COMPILER_GCC_AT_LEAST(14, 2, 0))
    static_assert(std::is_trivially_destructible<V>::value, "");
    static_assert(std::is_trivially_copy_constructible<V>::value, "");
    static_assert(std::is_trivially_move_constructible<V>::value, "");
    static_assert(!std::is_copy_assignable<V>::value, "");
    static_assert(!std::is_move_assignable<V>::value, "");
#else  // TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
    static_assert(std::is_trivially_copyable<V>::value, "");
#endif // TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
    static_assert(std::is_trivially_move_constructible<V>::value, "");
    test_constexpr_ctor_imp<0>(V(42l));
    test_constexpr_ctor_imp<1>(V(nullptr));
    test_constexpr_ctor_imp<2>(V(101));
}

struct NonTrivialMoveCtor {
    int i = 0;
    constexpr NonTrivialMoveCtor(int ii) : i(ii) {}
    constexpr NonTrivialMoveCtor(NonTrivialMoveCtor const& other) = default;
    constexpr NonTrivialMoveCtor(NonTrivialMoveCtor&& other) : i(other.i) {}
    constexpr ~NonTrivialMoveCtor() = default;
    friend constexpr bool operator==(
        NonTrivialMoveCtor const& x, NonTrivialMoveCtor const& y) {
        return x.i == y.i;
    }
};

constexpr void test_constexpr_move_ctor_non_trivial() {
    using V = __RXX variant<long, NonTrivialMoveCtor, void*>;
    static_assert(!std::is_trivially_move_constructible<V>::value, "");
    test_constexpr_ctor_imp<0>(V(42l));
    test_constexpr_ctor_imp<1>(V(NonTrivialMoveCtor(5)));
    test_constexpr_ctor_imp<2>(V(nullptr));
}

void non_constexpr_test() {
    test_move_ctor_valueless_by_exception();
}

constexpr bool cxx17_constexpr_test() {
    test_move_noexcept();
    test_move_ctor_sfinae();
    test_constexpr_move_ctor_trivial();

    return true;
}

constexpr bool cxx20_constexpr_test() {
    test_move_ctor_basic();
    test_constexpr_move_ctor_non_trivial();

    return true;
}

int main(int, char**) {
    non_constexpr_test();
    cxx17_constexpr_test();
    cxx20_constexpr_test();

    static_assert(cxx17_constexpr_test());
    static_assert(cxx20_constexpr_test());

    return 0;
}
