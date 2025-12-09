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

// constexpr variant& operator=(variant&&) noexcept(see below);

#include "../../../variant_test_helpers.h"
#include "rxx/variant.h"

#include <cassert>
#include <string>
#include <type_traits>
#include <utility>

struct NoCopy {
    NoCopy(NoCopy const&) = delete;
    NoCopy& operator=(NoCopy const&) = default;
};

struct CopyOnly {
    CopyOnly(CopyOnly const&) = default;
    CopyOnly(CopyOnly&&) = delete;
    CopyOnly& operator=(CopyOnly const&) = default;
    CopyOnly& operator=(CopyOnly&&) = delete;
};

struct MoveOnly {
    MoveOnly(MoveOnly const&) = delete;
    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(MoveOnly const&) = delete;
    MoveOnly& operator=(MoveOnly&&) = default;
};

struct MoveOnlyNT {
    MoveOnlyNT(MoveOnlyNT const&) = delete;
    MoveOnlyNT(MoveOnlyNT&&) {}
    MoveOnlyNT& operator=(MoveOnlyNT const&) = delete;
    MoveOnlyNT& operator=(MoveOnlyNT&&) = default;
};

struct MoveOnlyOddNothrow {
    MoveOnlyOddNothrow(MoveOnlyOddNothrow&&) noexcept(false) {}
    MoveOnlyOddNothrow(MoveOnlyOddNothrow const&) = delete;
    MoveOnlyOddNothrow& operator=(MoveOnlyOddNothrow&&) noexcept = default;
    MoveOnlyOddNothrow& operator=(MoveOnlyOddNothrow const&) = delete;
};

struct MoveAssignOnly {
    MoveAssignOnly(MoveAssignOnly&&) = delete;
    MoveAssignOnly& operator=(MoveAssignOnly&&) = default;
};

struct MoveAssign {
    constexpr MoveAssign(int v, int* move_ctor, int* move_assi)
        : value(v)
        , move_construct(move_ctor)
        , move_assign(move_assi) {}
    constexpr MoveAssign(MoveAssign&& o)
        : value(o.value)
        , move_construct(o.move_construct)
        , move_assign(o.move_assign) {
        ++*move_construct;
        o.value = -1;
    }
    constexpr MoveAssign& operator=(MoveAssign&& o) {
        value = o.value;
        move_construct = o.move_construct;
        move_assign = o.move_assign;
        ++*move_assign;
        o.value = -1;
        return *this;
    }
    int value;
    int* move_construct;
    int* move_assign;
};

struct NTMoveAssign {
    constexpr NTMoveAssign(int v) : value(v) {}
    NTMoveAssign(NTMoveAssign const&) = default;
    NTMoveAssign(NTMoveAssign&&) = default;
    NTMoveAssign& operator=(NTMoveAssign const& that) = default;
    NTMoveAssign& operator=(NTMoveAssign&& that) {
        value = that.value;
        that.value = -1;
        return *this;
    };
    int value;
};

static_assert(!std::is_trivially_move_assignable<NTMoveAssign>::value, "");
static_assert(std::is_move_assignable<NTMoveAssign>::value, "");

struct TMoveAssign {
    constexpr TMoveAssign(int v) : value(v) {}
    TMoveAssign(TMoveAssign const&) = delete;
    TMoveAssign(TMoveAssign&&) = default;
    TMoveAssign& operator=(TMoveAssign const&) = delete;
    TMoveAssign& operator=(TMoveAssign&&) = default;
    int value;
};

static_assert(std::is_trivially_move_assignable<TMoveAssign>::value, "");

struct TMoveAssignNTCopyAssign {
    constexpr TMoveAssignNTCopyAssign(int v) : value(v) {}
    TMoveAssignNTCopyAssign(TMoveAssignNTCopyAssign const&) = default;
    TMoveAssignNTCopyAssign(TMoveAssignNTCopyAssign&&) = default;
    TMoveAssignNTCopyAssign& operator=(TMoveAssignNTCopyAssign const& that) {
        value = that.value;
        return *this;
    }
    TMoveAssignNTCopyAssign& operator=(TMoveAssignNTCopyAssign&&) = default;
    int value;
};

static_assert(std::is_trivially_move_assignable_v<TMoveAssignNTCopyAssign>, "");

struct TrivialCopyNontrivialMove {
    TrivialCopyNontrivialMove(TrivialCopyNontrivialMove const&) = default;
    TrivialCopyNontrivialMove(TrivialCopyNontrivialMove&&) noexcept {}
    TrivialCopyNontrivialMove& operator=(
        TrivialCopyNontrivialMove const&) = default;
    TrivialCopyNontrivialMove& operator=(TrivialCopyNontrivialMove&&) noexcept {
        return *this;
    }
};

static_assert(
    std::is_trivially_copy_assignable_v<TrivialCopyNontrivialMove>, "");
static_assert(
    !std::is_trivially_move_assignable_v<TrivialCopyNontrivialMove>, "");

constexpr void test_move_assignment_noexcept() {
    {
        using V = __RXX variant<int>;
        static_assert(std::is_nothrow_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<MoveOnly>;
        static_assert(std::is_nothrow_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<int, long>;
        static_assert(std::is_nothrow_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<int, MoveOnly>;
        static_assert(std::is_nothrow_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<MoveOnlyNT>;
        static_assert(!std::is_nothrow_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<MoveOnlyOddNothrow>;
        static_assert(!std::is_nothrow_move_assignable<V>::value, "");
    }
}

constexpr void test_move_assignment_sfinae() {
    {
        using V = __RXX variant<int, long>;
        static_assert(std::is_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<int, CopyOnly>;
        static_assert(std::is_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<int, NoCopy>;
        static_assert(!std::is_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<int, MoveOnly>;
        static_assert(std::is_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<int, MoveOnlyNT>;
        static_assert(std::is_move_assignable<V>::value, "");
    }
    {
        // variant only provides move assignment when the types also provide
        // a move constructor.
        using V = __RXX variant<int, MoveAssignOnly>;
        static_assert(!std::is_move_assignable<V>::value, "");
    }

    // Make sure we properly propagate triviality (see P0602R4).
    {
        using V = __RXX variant<int, long>;
        static_assert(std::is_trivially_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<int, NTMoveAssign>;
        static_assert(!std::is_trivially_move_assignable<V>::value, "");
        static_assert(std::is_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<int, TMoveAssign>;
        static_assert(std::is_trivially_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<int, TMoveAssignNTCopyAssign>;
        static_assert(std::is_trivially_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<int, TrivialCopyNontrivialMove>;
        static_assert(!std::is_trivially_move_assignable<V>::value, "");
    }
    {
        using V = __RXX variant<int, CopyOnly>;
        static_assert(std::is_trivially_move_assignable<V>::value, "");
    }
}

void test_move_assignment_empty_empty() {
#if RXX_WITH_EXCEPTIONS
    using MET = MakeEmptyT;
    {
        using V = __RXX variant<int, long, MET>;
        V v1(std::in_place_index<0>);
        makeEmpty(v1);
        V v2(std::in_place_index<0>);
        makeEmpty(v2);
        V& vref = (v1 = std::move(v2));
        assert(&vref == &v1);
        assert(v1.valueless_by_exception());
        assert(v1.index() == __RXX variant_npos);
    }
#endif // TEST_HAS_NO_EXCEPTIONS
}

void test_move_assignment_non_empty_empty() {
#if RXX_WITH_EXCEPTIONS
    using MET = MakeEmptyT;
    {
        using V = __RXX variant<int, MET>;
        V v1(std::in_place_index<0>, 42);
        V v2(std::in_place_index<0>);
        makeEmpty(v2);
        V& vref = (v1 = std::move(v2));
        assert(&vref == &v1);
        assert(v1.valueless_by_exception());
        assert(v1.index() == __RXX variant_npos);
    }
    {
        using V = __RXX variant<int, MET, std::string>;
        V v1(std::in_place_index<2>, "hello");
        V v2(std::in_place_index<0>);
        makeEmpty(v2);
        V& vref = (v1 = std::move(v2));
        assert(&vref == &v1);
        assert(v1.valueless_by_exception());
        assert(v1.index() == __RXX variant_npos);
    }
#endif // TEST_HAS_NO_EXCEPTIONS
}

void test_move_assignment_empty_non_empty() {
#if RXX_WITH_EXCEPTIONS
    using MET = MakeEmptyT;
    {
        using V = __RXX variant<int, MET>;
        V v1(std::in_place_index<0>);
        makeEmpty(v1);
        V v2(std::in_place_index<0>, 42);
        V& vref = (v1 = std::move(v2));
        assert(&vref == &v1);
        assert(v1.index() == 0);
        assert(__RXX get<0>(v1) == 42);
    }
    {
        using V = __RXX variant<int, MET, std::string>;
        V v1(std::in_place_index<0>);
        makeEmpty(v1);
        V v2(std::in_place_type<std::string>, "hello");
        V& vref = (v1 = std::move(v2));
        assert(&vref == &v1);
        assert(v1.index() == 2);
        assert(__RXX get<2>(v1) == "hello");
    }
#endif // TEST_HAS_NO_EXCEPTIONS
}

template <typename T>
struct Result {
    std::size_t index;
    T value;
};

constexpr void test_move_assignment_same_index() {
    {
        using V = __RXX variant<int>;
        V v1(43);
        V v2(42);
        V& vref = (v1 = std::move(v2));
        assert(&vref == &v1);
        assert(v1.index() == 0);
        assert(__RXX get<0>(v1) == 42);
    }
    {
        using V = __RXX variant<int, long, unsigned>;
        V v1(43l);
        V v2(42l);
        V& vref = (v1 = std::move(v2));
        assert(&vref == &v1);
        assert(v1.index() == 1);
        assert(__RXX get<1>(v1) == 42);
    }
    {
        using V = __RXX variant<int, MoveAssign, unsigned>;
        int move_construct = 0;
        int move_assign = 0;
        V v1(std::in_place_type<MoveAssign>, 43, &move_construct, &move_assign);
        V v2(std::in_place_type<MoveAssign>, 42, &move_construct, &move_assign);
        V& vref = (v1 = std::move(v2));
        assert(&vref == &v1);
        assert(v1.index() == 1);
        assert(__RXX get<1>(v1).value == 42);
        assert(move_construct == 0);
        assert(move_assign == 1);
    }

    // Make sure we properly propagate triviality, which implies constexpr-ness
    // (see P0602R4).
    {
        struct {
            constexpr Result<int> operator()() const {
                using V = __RXX variant<int>;
                V v(43);
                V v2(42);
                v = std::move(v2);
                return {v.index(), __RXX get<0>(v)};
            }
        } test;
        constexpr auto result = test();
        static_assert(result.index == 0, "");
        static_assert(result.value == 42, "");
    }
    {
        struct {
            constexpr Result<long> operator()() const {
                using V = __RXX variant<int, long, unsigned>;
                V v(43l);
                V v2(42l);
                v = std::move(v2);
                return {v.index(), __RXX get<1>(v)};
            }
        } test;
        constexpr auto result = test();
        static_assert(result.index == 1, "");
        static_assert(result.value == 42l, "");
    }
    {
        struct {
            constexpr Result<int> operator()() const {
                using V = __RXX variant<int, TMoveAssign, unsigned>;
                V v(std::in_place_type<TMoveAssign>, 43);
                V v2(std::in_place_type<TMoveAssign>, 42);
                v = std::move(v2);
                return {v.index(), __RXX get<1>(v).value};
            }
        } test;
        constexpr auto result = test();
        static_assert(result.index == 1, "");
        static_assert(result.value == 42, "");
    }
}

constexpr void test_move_assignment_different_index() {
    {
        using V = __RXX variant<int, long, unsigned>;
        V v1(43);
        V v2(42l);
        V& vref = (v1 = std::move(v2));
        assert(&vref == &v1);
        assert(v1.index() == 1);
        assert(__RXX get<1>(v1) == 42);
    }
    {
        using V = __RXX variant<int, MoveAssign, unsigned>;
        int move_construct = 0;
        int move_assign = 0;
        V v1(std::in_place_type<unsigned>, 43u);
        V v2(std::in_place_type<MoveAssign>, 42, &move_construct, &move_assign);
        V& vref = (v1 = std::move(v2));
        assert(&vref == &v1);
        assert(v1.index() == 1);
        assert(__RXX get<1>(v1).value == 42);
        assert(move_construct == 1);
        assert(move_assign == 0);
    }

    // Make sure we properly propagate triviality, which implies constexpr-ness
    // (see P0602R4).
    {
        struct {
            constexpr Result<long> operator()() const {
                using V = __RXX variant<int, long, unsigned>;
                V v(43);
                V v2(42l);
                v = std::move(v2);
                return {v.index(), __RXX get<1>(v)};
            }
        } test;
        constexpr auto result = test();
        static_assert(result.index == 1, "");
        static_assert(result.value == 42l, "");
    }
    {
        struct {
            constexpr Result<long> operator()() const {
                using V = __RXX variant<int, TMoveAssign, unsigned>;
                V v(std::in_place_type<unsigned>, 43u);
                V v2(std::in_place_type<TMoveAssign>, 42);
                v = std::move(v2);
                return {v.index(), __RXX get<1>(v).value};
            }
        } test;
        constexpr auto result = test();
        static_assert(result.index == 1, "");
        static_assert(result.value == 42, "");
    }
}

void test_assignment_throw() {
#if RXX_WITH_EXCEPTIONS
    using MET = MakeEmptyT;
    // same index
    {
        using V = __RXX variant<int, MET, std::string>;
        V v1(std::in_place_type<MET>);
        MET& mref = __RXX get<1>(v1);
        V v2(std::in_place_type<MET>);
        try {
            v1 = std::move(v2);
            assert(false);
        } catch (...) {}
        assert(v1.index() == 1);
        assert(&__RXX get<1>(v1) == &mref);
    }

    // different indices
    {
        using V = __RXX variant<int, MET, std::string>;
        V v1(std::in_place_type<int>);
        V v2(std::in_place_type<MET>);
        try {
            v1 = std::move(v2);
            assert(false);
        } catch (...) {}
        assert(v1.valueless_by_exception());
        assert(v1.index() == __RXX variant_npos);
    }
    {
        using V = __RXX variant<int, MET, std::string>;
        V v1(std::in_place_type<MET>);
        V v2(std::in_place_type<std::string>, "hello");
        V& vref = (v1 = std::move(v2));
        assert(&vref == &v1);
        assert(v1.index() == 2);
        assert(__RXX get<2>(v1) == "hello");
    }
#endif // TEST_HAS_NO_EXCEPTIONS
}

template <std::size_t NewIdx, class T, class ValueType>
constexpr void test_constexpr_assign_imp(T&& v, ValueType&& new_value) {
    using Variant = std::decay_t<T>;
    Variant v2(std::forward<ValueType>(new_value));
    auto const cp = v2;
    v = std::move(v2);
    assert(v.index() == NewIdx);
    assert(__RXX get<NewIdx>(v) == __RXX get<NewIdx>(cp));
}

constexpr void test_constexpr_move_assignment_trivial() {
    // Make sure we properly propagate triviality, which implies constexpr-ness
    // (see P0602R4).
    using V = __RXX variant<long, void*, int>;
    static_assert(std::is_trivially_copyable<V>::value, "");
    static_assert(std::is_trivially_move_assignable<V>::value, "");
    test_constexpr_assign_imp<0>(V(42l), 101l);
    test_constexpr_assign_imp<0>(V(nullptr), 101l);
    test_constexpr_assign_imp<1>(V(42l), nullptr);
    test_constexpr_assign_imp<2>(V(42l), 101);
}

struct NonTrivialMoveAssign {
    int i = 0;
    constexpr NonTrivialMoveAssign(int ii) : i(ii) {}
    constexpr NonTrivialMoveAssign(NonTrivialMoveAssign const& other) = default;
    constexpr NonTrivialMoveAssign(NonTrivialMoveAssign&& other) : i(other.i) {}
    constexpr NonTrivialMoveAssign& operator=(
        NonTrivialMoveAssign const&) = default;
    constexpr NonTrivialMoveAssign& operator=(NonTrivialMoveAssign&& o) {
        i = o.i;
        return *this;
    }
    constexpr ~NonTrivialMoveAssign() = default;
    friend constexpr bool operator==(
        NonTrivialMoveAssign const& x, NonTrivialMoveAssign const& y) {
        return x.i == y.i;
    }
};

constexpr void test_constexpr_move_assignment_non_trivial() {
    using V = __RXX variant<long, void*, NonTrivialMoveAssign>;
    static_assert(!std::is_trivially_copyable<V>::value);
    static_assert(!std::is_trivially_move_assignable<V>::value);
    test_constexpr_assign_imp<0>(V(42l), 101l);
    test_constexpr_assign_imp<0>(V(nullptr), 101l);
    test_constexpr_assign_imp<1>(V(42l), nullptr);
    test_constexpr_assign_imp<2>(V(42l), NonTrivialMoveAssign(5));
    test_constexpr_assign_imp<2>(
        V(NonTrivialMoveAssign(3)), NonTrivialMoveAssign(5));
}

void non_constexpr_test() {
    test_move_assignment_empty_empty();
    test_move_assignment_non_empty_empty();
    test_move_assignment_empty_non_empty();
    test_assignment_throw();
}

constexpr bool cxx17_constexpr_test() {
    test_move_assignment_sfinae();
    test_move_assignment_noexcept();
    test_constexpr_move_assignment_trivial();

    return true;
}

constexpr bool cxx20_constexpr_test() {
    test_move_assignment_same_index();
    test_move_assignment_different_index();
    test_constexpr_move_assignment_non_trivial();

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
