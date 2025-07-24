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

// xranges::size

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "rxx/ranges/access.h"

#include <cassert>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

using RangeSizeT = decltype(xranges::size);

static_assert(!std::is_invocable_v<RangeSizeT, int[]>);
static_assert(std::is_invocable_v<RangeSizeT, int[1]>);
static_assert(std::is_invocable_v<RangeSizeT, int (&&)[1]>);
static_assert(std::is_invocable_v<RangeSizeT, int (&)[1]>);

struct Incomplete;
static_assert(!std::is_invocable_v<RangeSizeT, Incomplete[]>);
static_assert(!std::is_invocable_v<RangeSizeT, Incomplete (&)[]>);
static_assert(!std::is_invocable_v<RangeSizeT, Incomplete (&&)[]>);

extern Incomplete array_of_incomplete[42];
#if RXX_COMPILER_CLANG
// The standard doesn't specify whether or not an incomplete object
// is valid; clang supports this but GCC's is_invocable does not work
// on incomplete types.
static_assert(std::is_invocable_v<RangeSizeT, Incomplete[42]>);
static_assert(std::is_invocable_v<RangeSizeT, Incomplete (&)[42]>);
static_assert(std::is_invocable_v<RangeSizeT, Incomplete (&&)[42]>);
static_assert(xranges::size(array_of_incomplete) == 42);
static_assert(xranges::size(std::move(array_of_incomplete)) == 42);
static_assert(xranges::size(std::as_const(array_of_incomplete)) == 42);
static_assert(xranges::size(static_cast<Incomplete const (&&)[42]>(
                  array_of_incomplete)) == 42);
#endif

struct SizeMember {
    constexpr std::size_t size() { return 42; }
};

struct StaticSizeMember {
    constexpr static std::size_t size() { return 42; }
};

static_assert(!std::is_invocable_v<RangeSizeT, SizeMember const>);

struct SizeFunction {
    friend constexpr std::size_t size(SizeFunction) { return 42; }
};

// Make sure the size member is preferred.
struct SizeMemberAndFunction {
    constexpr std::size_t size() { return 42; }
    friend constexpr std::size_t size(SizeMemberAndFunction) { return 0; }
};

bool constexpr testArrayType() {
    int a[4];
    int b[1];
    SizeMember c[4];
    SizeFunction d[4];

    assert(xranges::size(a) == 4);
    ASSERT_SAME_TYPE(decltype(xranges::size(a)), std::size_t);
    assert(xranges::size(b) == 1);
    ASSERT_SAME_TYPE(decltype(xranges::size(b)), std::size_t);
    assert(xranges::size(c) == 4);
    ASSERT_SAME_TYPE(decltype(xranges::size(c)), std::size_t);
    assert(xranges::size(d) == 4);
    ASSERT_SAME_TYPE(decltype(xranges::size(d)), std::size_t);

    return true;
}

struct SizeMemberConst {
    constexpr std::size_t size() const { return 42; }
};

struct SizeMemberSigned {
    constexpr long size() { return 42; }
};

bool constexpr testHasSizeMember() {
    assert(xranges::size(SizeMember()) == 42);
    ASSERT_SAME_TYPE(decltype(xranges::size(SizeMember())), std::size_t);

    SizeMemberConst const sizeMemberConst;
    assert(xranges::size(sizeMemberConst) == 42);

    assert(xranges::size(SizeMemberAndFunction()) == 42);

    assert(xranges::size(SizeMemberSigned()) == 42);
    ASSERT_SAME_TYPE(decltype(xranges::size(SizeMemberSigned())), long);

    assert(xranges::size(StaticSizeMember()) == 42);
    ASSERT_SAME_TYPE(decltype(xranges::size(StaticSizeMember())), std::size_t);

    return true;
}

struct MoveOnlySizeFunction {
    MoveOnlySizeFunction() = default;
    MoveOnlySizeFunction(MoveOnlySizeFunction&&) = default;
    MoveOnlySizeFunction(MoveOnlySizeFunction const&) = delete;

    friend constexpr std::size_t size(MoveOnlySizeFunction) { return 42; }
};

enum EnumSizeFunction {
    a,
    b
};

constexpr std::size_t size(EnumSizeFunction) {
    return 42;
}

struct SizeFunctionConst {
    friend constexpr std::size_t size(SizeFunctionConst const) { return 42; }
};

struct SizeFunctionRef {
    friend constexpr std::size_t size(SizeFunctionRef&) { return 42; }
};

struct SizeFunctionConstRef {
    friend constexpr std::size_t size(SizeFunctionConstRef const&) {
        return 42;
    }
};

struct SizeFunctionSigned {
    friend constexpr long size(SizeFunctionSigned) { return 42; }
};

bool constexpr testHasSizeFunction() {
    assert(xranges::size(SizeFunction()) == 42);
    ASSERT_SAME_TYPE(decltype(xranges::size(SizeFunction())), std::size_t);
    static_assert(!std::is_invocable_v<RangeSizeT, MoveOnlySizeFunction>);
    assert(xranges::size(EnumSizeFunction()) == 42);
    assert(xranges::size(SizeFunctionConst()) == 42);

    SizeFunctionRef a;
    assert(xranges::size(a) == 42);

    SizeFunctionConstRef const b;
    assert(xranges::size(b) == 42);

    assert(xranges::size(SizeFunctionSigned()) == 42);
    ASSERT_SAME_TYPE(decltype(xranges::size(SizeFunctionSigned())), long);

    return true;
}

struct Empty {};
static_assert(!std::is_invocable_v<RangeSizeT, Empty>);

struct InvalidReturnTypeMember {
    Empty size();
};

struct InvalidReturnTypeFunction {
    friend Empty size(InvalidReturnTypeFunction);
};

struct Convertible {
    operator std::size_t();
};

struct ConvertibleReturnTypeMember {
    Convertible size();
};

struct ConvertibleReturnTypeFunction {
    friend Convertible size(ConvertibleReturnTypeFunction);
};

struct BoolReturnTypeMember {
    bool size() const;
};

struct BoolReturnTypeFunction {
    friend bool size(BoolReturnTypeFunction const&);
};

static_assert(!std::is_invocable_v<RangeSizeT, InvalidReturnTypeMember>);
static_assert(!std::is_invocable_v<RangeSizeT, InvalidReturnTypeFunction>);
static_assert(std::is_invocable_v<RangeSizeT, InvalidReturnTypeMember (&)[4]>);
static_assert(
    std::is_invocable_v<RangeSizeT, InvalidReturnTypeFunction (&)[4]>);
static_assert(!std::is_invocable_v<RangeSizeT, ConvertibleReturnTypeMember>);
static_assert(!std::is_invocable_v<RangeSizeT, ConvertibleReturnTypeFunction>);
static_assert(!std::is_invocable_v<RangeSizeT, BoolReturnTypeMember const&>);
static_assert(!std::is_invocable_v<RangeSizeT, BoolReturnTypeFunction const&>);

struct SizeMemberDisabled {
    std::size_t size() { return 42; }
};

template <>
inline constexpr bool std::ranges::disable_sized_range<SizeMemberDisabled> =
    true;

struct ImproperlyDisabledMember {
    std::size_t size() const { return 42; }
};

// Intentionally disabling "const ConstSizeMemberDisabled". This doesn't disable
// anything because T is always uncvrefed before being checked.
template <>
inline constexpr bool
    std::ranges::disable_sized_range<ImproperlyDisabledMember const> = true;

struct SizeFunctionDisabled {
    friend std::size_t size(SizeFunctionDisabled) { return 42; }
};

template <>
inline constexpr bool std::ranges::disable_sized_range<SizeFunctionDisabled> =
    true;

struct ImproperlyDisabledFunction {
    friend std::size_t size(ImproperlyDisabledFunction const&) { return 42; }
};

template <>
inline constexpr bool
    std::ranges::disable_sized_range<ImproperlyDisabledFunction const> = true;

static_assert(std::is_invocable_v<RangeSizeT, ImproperlyDisabledMember&>);
static_assert(std::is_invocable_v<RangeSizeT, ImproperlyDisabledMember const&>);
static_assert(std::is_invocable_v<RangeSizeT,
    ImproperlyDisabledFunction&>); // Ill-formed before P2602R2 Poison Pills are
                                   // Too Toxic
static_assert(
    std::is_invocable_v<RangeSizeT, ImproperlyDisabledFunction const&>);

// No begin end.
struct HasMinusOperator {
    friend constexpr std::size_t operator-(HasMinusOperator, HasMinusOperator) {
        return 2;
    }
};
static_assert(!std::is_invocable_v<RangeSizeT, HasMinusOperator>);

struct HasMinusBeginEnd {
    struct sentinel {
        friend bool operator==(sentinel, forward_iterator<int*>);
        friend constexpr std::ptrdiff_t operator-(
            sentinel const, forward_iterator<int*> const) {
            return 2;
        }
        friend constexpr std::ptrdiff_t operator-(
            forward_iterator<int*> const, sentinel const) {
            return 2;
        }
    };

    friend constexpr forward_iterator<int*> begin(HasMinusBeginEnd) {
        return {};
    }
    friend constexpr sentinel end(HasMinusBeginEnd) { return {}; }
};

struct other_forward_iterator : forward_iterator<int*> {};

struct InvalidMinusBeginEnd {
    struct sentinel {
        friend bool operator==(sentinel, other_forward_iterator);
        friend constexpr std::ptrdiff_t operator-(
            sentinel const, other_forward_iterator const) {
            return 2;
        }
        friend constexpr std::ptrdiff_t operator-(
            other_forward_iterator const, sentinel const) {
            return 2;
        }
    };

    friend constexpr other_forward_iterator begin(InvalidMinusBeginEnd) {
        return {};
    }
    friend constexpr sentinel end(InvalidMinusBeginEnd) { return {}; }
};

// short is integer-like, but it is not other_forward_iterator's
// difference_type.
static_assert(!std::same_as<other_forward_iterator::difference_type, short>);
static_assert(!std::is_invocable_v<RangeSizeT, InvalidMinusBeginEnd>);

struct RandomAccessRange {
    struct sentinel {
        friend bool operator==(sentinel, random_access_iterator<int*>);
        friend constexpr std::ptrdiff_t operator-(
            sentinel const, random_access_iterator<int*> const) {
            return 2;
        }
        friend constexpr std::ptrdiff_t operator-(
            random_access_iterator<int*> const, sentinel const) {
            return 2;
        }
    };

    constexpr random_access_iterator<int*> begin() { return {}; }
    constexpr sentinel end() { return {}; }
};

struct IntPtrBeginAndEnd {
    int buff[8];
    constexpr int* begin() { return buff; }
    constexpr int* end() { return buff + 8; }
};

struct DisabledSizeRangeWithBeginEnd {
    int buff[8];
    constexpr int* begin() { return buff; }
    constexpr int* end() { return buff + 8; }
    constexpr std::size_t size() { return 1; }
};

template <>
inline constexpr bool
    std::ranges::disable_sized_range<DisabledSizeRangeWithBeginEnd> = true;

struct SizeBeginAndEndMembers {
    int buff[8];
    constexpr int* begin() { return buff; }
    constexpr int* end() { return buff + 8; }
    constexpr std::size_t size() { return 1; }
};

constexpr bool testRanges() {
    HasMinusBeginEnd a;
    assert(xranges::size(a) == 2);
    // Ensure that this is converted to an *unsigned* type.
    ASSERT_SAME_TYPE(decltype(xranges::size(a)), std::size_t);

    IntPtrBeginAndEnd b;
    assert(xranges::size(b) == 8);

    DisabledSizeRangeWithBeginEnd c;
    assert(xranges::size(c) == 8);

    RandomAccessRange d;
    assert(xranges::size(d) == 2);
    ASSERT_SAME_TYPE(decltype(xranges::size(d)), std::size_t);

    SizeBeginAndEndMembers e;
    assert(xranges::size(e) == 1);

    return true;
}

// Test ADL-proofing.
struct Incomplete;
template <class T>
struct Holder {
    T t;
};
static_assert(!std::is_invocable_v<RangeSizeT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeSizeT, Holder<Incomplete>*&>);

int main(int, char**) {
    testArrayType();
    static_assert(testArrayType());

    testHasSizeMember();
    static_assert(testHasSizeMember());

    testHasSizeFunction();
    static_assert(testHasSizeFunction());

    testRanges();
    static_assert(testRanges());

    return 0;
}
