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

// xranges::rbegin
// xranges::crbegin

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "rxx/ranges/access.h"

#include <cassert>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

using RangeRBeginT = decltype(xranges::rbegin);
using RangeCRBeginT = decltype(xranges::crbegin);

static int globalBuff[8];

static_assert(!std::is_invocable_v<RangeRBeginT, int (&&)[10]>);
static_assert(std::is_invocable_v<RangeRBeginT, int (&)[10]>);
static_assert(!std::is_invocable_v<RangeRBeginT, int (&&)[]>);
static_assert(!std::is_invocable_v<RangeRBeginT, int (&)[]>);
static_assert(!std::is_invocable_v<RangeCRBeginT, int (&&)[10]>);
static_assert(std::is_invocable_v<RangeCRBeginT, int (&)[10]>);
static_assert(!std::is_invocable_v<RangeCRBeginT, int (&&)[]>);
static_assert(!std::is_invocable_v<RangeCRBeginT, int (&)[]>);

struct Incomplete;

static_assert(!std::is_invocable_v<RangeRBeginT, Incomplete (&&)[]>);
static_assert(!std::is_invocable_v<RangeRBeginT, Incomplete const (&&)[]>);
static_assert(!std::is_invocable_v<RangeCRBeginT, Incomplete (&&)[]>);
static_assert(!std::is_invocable_v<RangeCRBeginT, Incomplete const (&&)[]>);

static_assert(!std::is_invocable_v<RangeRBeginT, Incomplete (&&)[10]>);
static_assert(!std::is_invocable_v<RangeRBeginT, Incomplete const (&&)[10]>);
static_assert(!std::is_invocable_v<RangeCRBeginT, Incomplete (&&)[10]>);
static_assert(!std::is_invocable_v<RangeCRBeginT, Incomplete const (&&)[10]>);

// This case is IFNDR; we handle it SFINAE-friendly.
static_assert(!std::is_invocable_v<RangeRBeginT, Incomplete (&)[]>);
static_assert(!std::is_invocable_v<RangeRBeginT, Incomplete const (&)[]>);
static_assert(!std::is_invocable_v<RangeCRBeginT, Incomplete (&)[]>);
static_assert(!std::is_invocable_v<RangeCRBeginT, Incomplete const (&)[]>);

// This case is IFNDR; we handle it SFINAE-friendly.
static_assert(!std::is_invocable_v<RangeRBeginT, Incomplete (&)[10]>);
static_assert(!std::is_invocable_v<RangeRBeginT, Incomplete const (&)[10]>);
static_assert(!std::is_invocable_v<RangeCRBeginT, Incomplete (&)[10]>);
static_assert(!std::is_invocable_v<RangeCRBeginT, Incomplete const (&)[10]>);

struct RBeginMember {
    int x;
    constexpr int const* rbegin() const { return &x; }
    constexpr int const* rend() const { return &x - 1; }
    constexpr int const* begin() const { return &x; }
    constexpr int const* end() const { return &x + 1; }
};

// Ensure that we can't call with rvalues with borrowing disabled.
static_assert(std::is_invocable_v<RangeRBeginT, RBeginMember&>);
static_assert(!std::is_invocable_v<RangeRBeginT, RBeginMember&&>);
static_assert(std::is_invocable_v<RangeRBeginT, RBeginMember const&>);
static_assert(!std::is_invocable_v<RangeRBeginT, RBeginMember const&&>);
static_assert(std::is_invocable_v<RangeCRBeginT, RBeginMember&>);
static_assert(!std::is_invocable_v<RangeCRBeginT, RBeginMember&&>);
static_assert(std::is_invocable_v<RangeCRBeginT, RBeginMember const&>);
static_assert(!std::is_invocable_v<RangeCRBeginT, RBeginMember const&&>);

constexpr bool testReturnTypes() {
    {
        int* x[2];
        ASSERT_SAME_TYPE(
            decltype(xranges::rbegin(x)), std::reverse_iterator<int**>);
        ASSERT_SAME_TYPE(
            decltype(xranges::crbegin(x)), std::reverse_iterator<int* const*>);
    }
    {
        int x[2][2];
        ASSERT_SAME_TYPE(
            decltype(xranges::rbegin(x)), std::reverse_iterator<int(*)[2]>);
        ASSERT_SAME_TYPE(decltype(xranges::crbegin(x)),
            std::reverse_iterator<int const(*)[2]>);
    }
    {
        struct Different {
            char*& rbegin();
            short*& rbegin() const;
            char*& begin();
            short*& begin() const;
            char*& end();
            short*& end() const;
        } x;
        ASSERT_SAME_TYPE(decltype(xranges::rbegin(x)), char*);
        ASSERT_SAME_TYPE(
            decltype(xranges::crbegin(x)), rxx::basic_const_iterator<short*>);
    }
    return true;
}

constexpr bool testArray() {
    int a[2];
    assert(xranges::rbegin(a).base() == a + 2);
    assert(xranges::crbegin(a).base() == a + 2);

    int b[2][2];
    assert(xranges::rbegin(b).base() == b + 2);
    assert(xranges::crbegin(b).base() == b + 2);

    RBeginMember c[2];
    assert(xranges::rbegin(c).base() == c + 2);
    assert(xranges::crbegin(c).base() == c + 2);

    return true;
}

struct RBeginMemberReturnsInt {
    int rbegin() const;
};
static_assert(
    !std::is_invocable_v<RangeRBeginT, RBeginMemberReturnsInt const&>);

struct RBeginMemberReturnsVoidPtr {
    void const* rbegin() const;
};
static_assert(
    !std::is_invocable_v<RangeRBeginT, RBeginMemberReturnsVoidPtr const&>);

struct PtrConvertibleRBeginMember {
    struct iterator {
        operator int*() const;
    };
    iterator rbegin() const;
};
static_assert(
    !std::is_invocable_v<RangeRBeginT, PtrConvertibleRBeginMember const&>);

struct NonConstRBeginMember {
    int x;
    constexpr int* rbegin() { return &x; }
};
static_assert(std::is_invocable_v<RangeRBeginT, NonConstRBeginMember&>);
static_assert(!std::is_invocable_v<RangeRBeginT, NonConstRBeginMember const&>);
static_assert(!std::is_invocable_v<RangeCRBeginT, NonConstRBeginMember&>);
static_assert(!std::is_invocable_v<RangeCRBeginT, NonConstRBeginMember const&>);

struct EnabledBorrowingRBeginMember {
    constexpr int* rbegin() const { return globalBuff; }
    constexpr int* begin() const { return globalBuff; }
    constexpr int* end() const {
        return globalBuff + xranges::size(globalBuff);
    }
};
template <>
inline constexpr bool
    std::ranges::enable_borrowed_range<EnabledBorrowingRBeginMember> = true;

struct RBeginMemberFunction {
    int x;
    constexpr int const* rbegin() const { return &x; }
    constexpr int const* begin() const { return &x; }
    constexpr int const* end() const { return &x + 1; }
    friend int* rbegin(RBeginMemberFunction const&);
};

struct EmptyPtrRBeginMember {
    struct Empty {};
    Empty x;
    constexpr Empty const* rbegin() const { return &x; }
    constexpr Empty const* begin() const { return &x; }
    constexpr Empty const* end() const { return &x + 1; }
};

constexpr bool testRBeginMember() {
    RBeginMember a;
    assert(xranges::rbegin(a) == &a.x);
    assert(xranges::crbegin(a) == &a.x);
    static_assert(!std::is_invocable_v<RangeRBeginT, RBeginMember&&>);
    static_assert(!std::is_invocable_v<RangeCRBeginT, RBeginMember&&>);

    NonConstRBeginMember b;
    assert(xranges::rbegin(b) == &b.x);
    static_assert(!std::is_invocable_v<RangeCRBeginT, NonConstRBeginMember&>);

    EnabledBorrowingRBeginMember c;
    assert(xranges::rbegin(c) == globalBuff);
    assert(xranges::crbegin(c) == globalBuff);
    assert(xranges::rbegin(std::move(c)) == globalBuff);
    assert(xranges::crbegin(std::move(c)) == globalBuff);

    RBeginMemberFunction d;
    assert(xranges::rbegin(d) == &d.x);
    assert(xranges::crbegin(d) == &d.x);

    EmptyPtrRBeginMember e;
    assert(xranges::rbegin(e) == &e.x);
    assert(xranges::crbegin(e) == &e.x);

    return true;
}

struct RBeginFunction {
    int x;
    friend constexpr int const* rbegin(RBeginFunction const& bf) {
        return &bf.x;
    }

    friend constexpr int const* begin(RBeginFunction const& bf) {
        return &bf.x;
    }

    friend constexpr int const* end(RBeginFunction const& bf) {
        return &bf.x + 1;
    }
};
static_assert(std::is_invocable_v<RangeRBeginT, RBeginFunction const&>);
static_assert(!std::is_invocable_v<RangeRBeginT, RBeginFunction&&>);
static_assert(std::is_invocable_v<RangeRBeginT,
    RBeginFunction&>); // Ill-formed before P2602R2 Poison Pills are Too Toxic
static_assert(std::is_invocable_v<RangeCRBeginT, RBeginFunction const&>);
static_assert(std::is_invocable_v<RangeCRBeginT, RBeginFunction&>);

struct RBeginFunctionReturnsInt {
    friend int rbegin(RBeginFunctionReturnsInt const&);
    friend int begin(RBeginFunctionReturnsInt const&);
    friend int end(RBeginFunctionReturnsInt const&);
};
static_assert(
    !std::is_invocable_v<RangeRBeginT, RBeginFunctionReturnsInt const&>);

struct RBeginFunctionReturnsVoidPtr {
    friend void* rbegin(RBeginFunctionReturnsVoidPtr const&);
    friend void* begin(RBeginFunctionReturnsVoidPtr const&);
    friend void* end(RBeginFunctionReturnsVoidPtr const&);
};
static_assert(
    !std::is_invocable_v<RangeRBeginT, RBeginFunctionReturnsVoidPtr const&>);

struct RBeginFunctionReturnsEmpty {
    struct Empty {};
    friend Empty rbegin(RBeginFunctionReturnsEmpty const&);
    friend Empty begin(RBeginFunctionReturnsEmpty const&);
    friend Empty end(RBeginFunctionReturnsEmpty const&);
};
static_assert(
    !std::is_invocable_v<RangeRBeginT, RBeginFunctionReturnsEmpty const&>);

struct RBeginFunctionReturnsPtrConvertible {
    struct iterator {
        operator int*() const;
    };
    friend iterator rbegin(RBeginFunctionReturnsPtrConvertible const&);
    friend iterator begin(RBeginFunctionReturnsPtrConvertible const&);
    friend iterator end(RBeginFunctionReturnsPtrConvertible const&);
};
static_assert(!std::is_invocable_v<RangeRBeginT,
              RBeginFunctionReturnsPtrConvertible const&>);

struct RBeginFunctionByValue {
    friend constexpr int* rbegin(RBeginFunctionByValue) {
        return globalBuff + 1;
    }

    friend constexpr int* begin(RBeginFunctionByValue) {
        return globalBuff + 1;
    }
    friend constexpr int* end(RBeginFunctionByValue) { return globalBuff + 2; }
};
static_assert(!std::is_invocable_v<RangeCRBeginT, RBeginFunctionByValue>);

struct RBeginFunctionEnabledBorrowing {
    friend constexpr int* rbegin(RBeginFunctionEnabledBorrowing) {
        return globalBuff + 2;
    }
    friend constexpr int* begin(RBeginFunctionEnabledBorrowing) {
        return globalBuff + 2;
    }
    friend constexpr int* end(RBeginFunctionEnabledBorrowing) {
        return globalBuff + 3;
    }
};
template <>
inline constexpr bool
    std::ranges::enable_borrowed_range<RBeginFunctionEnabledBorrowing> = true;

struct RBeginFunctionReturnsEmptyPtr {
    struct Empty {};
    Empty x;
    friend constexpr Empty const* rbegin(
        RBeginFunctionReturnsEmptyPtr const& bf) {
        return &bf.x;
    }
    friend constexpr Empty const* begin(
        RBeginFunctionReturnsEmptyPtr const& bf) {
        return &bf.x;
    }
    friend constexpr Empty const* end(RBeginFunctionReturnsEmptyPtr const& bf) {
        return &bf.x + 1;
    }
};

struct RBeginFunctionWithDataMember {
    int x;
    int rbegin;
    friend constexpr int const* rbegin(RBeginFunctionWithDataMember const& bf) {
        return &bf.x;
    }
    friend constexpr int const* begin(RBeginFunctionWithDataMember const& bf) {
        return &bf.x;
    }
    friend constexpr int const* end(RBeginFunctionWithDataMember const& bf) {
        return &bf.x + 1;
    }
};

struct RBeginFunctionWithPrivateBeginMember {
    int y;
    friend constexpr int const* rbegin(
        RBeginFunctionWithPrivateBeginMember const& bf) {
        return &bf.y;
    }

    friend constexpr int const* begin(
        RBeginFunctionWithPrivateBeginMember const& bf) {
        return &bf.y;
    }

    friend constexpr int const* end(
        RBeginFunctionWithPrivateBeginMember const& bf) {
        return &bf.y + 1;
    }

private:
    int const* rbegin() const;
};

constexpr bool testRBeginFunction() {
    RBeginFunction a{};
    RBeginFunction const aa{};
    assert(xranges::rbegin(a) ==
        &a.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::crbegin(a) == &a.x);
    assert(xranges::rbegin(aa) == &aa.x);
    assert(xranges::crbegin(aa) == &aa.x);

    RBeginFunctionByValue b{};
    RBeginFunctionByValue const bb{};
    assert(xranges::rbegin(b) == globalBuff + 1);
    assert(xranges::crbegin(b) == globalBuff + 1);
    assert(xranges::rbegin(bb) == globalBuff + 1);
    assert(xranges::crbegin(bb) == globalBuff + 1);

    RBeginFunctionEnabledBorrowing c{};
    RBeginFunctionEnabledBorrowing const cc{};
    assert(xranges::rbegin(std::move(c)) == globalBuff + 2);
    assert(xranges::crbegin(std::move(c)) == globalBuff + 2);
    assert(xranges::rbegin(std::move(cc)) == globalBuff + 2);
    assert(xranges::crbegin(std::move(cc)) == globalBuff + 2);

    RBeginFunctionReturnsEmptyPtr d{};
    RBeginFunctionReturnsEmptyPtr const dd{};
    assert(xranges::rbegin(d) ==
        &d.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::crbegin(d) == &d.x);
    assert(xranges::rbegin(dd) == &dd.x);
    assert(xranges::crbegin(dd) == &dd.x);

    RBeginFunctionWithDataMember e{};
    RBeginFunctionWithDataMember const ee{};
    assert(xranges::rbegin(e) ==
        &e.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::rbegin(ee) == &ee.x);
    assert(xranges::crbegin(e) == &e.x);
    assert(xranges::crbegin(ee) == &ee.x);

    RBeginFunctionWithPrivateBeginMember f{};
    RBeginFunctionWithPrivateBeginMember const ff{};
    assert(xranges::rbegin(f) ==
        &f.y); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::crbegin(f) == &f.y);
    assert(xranges::rbegin(ff) == &ff.y);
    assert(xranges::crbegin(ff) == &ff.y);

    return true;
}

struct MemberBeginEnd {
    int b, e;
    char cb, ce;
    constexpr bidirectional_iterator<int*> begin() {
        return bidirectional_iterator<int*>(&b);
    }
    constexpr bidirectional_iterator<int*> end() {
        return bidirectional_iterator<int*>(&e);
    }
    constexpr bidirectional_iterator<char const*> begin() const {
        return bidirectional_iterator<char const*>(&cb);
    }
    constexpr bidirectional_iterator<char const*> end() const {
        return bidirectional_iterator<char const*>(&ce);
    }
};
static_assert(std::is_invocable_v<RangeRBeginT, MemberBeginEnd&>);
static_assert(std::is_invocable_v<RangeRBeginT, MemberBeginEnd const&>);
static_assert(std::is_invocable_v<RangeCRBeginT, MemberBeginEnd const&>);

struct FunctionBeginEnd {
    int b, e;
    char cb, ce;
    friend constexpr bidirectional_iterator<int*> begin(FunctionBeginEnd& v) {
        return bidirectional_iterator<int*>(&v.b);
    }
    friend constexpr bidirectional_iterator<int*> end(FunctionBeginEnd& v) {
        return bidirectional_iterator<int*>(&v.e);
    }
    friend constexpr bidirectional_iterator<char const*> begin(
        FunctionBeginEnd const& v) {
        return bidirectional_iterator<char const*>(&v.cb);
    }
    friend constexpr bidirectional_iterator<char const*> end(
        FunctionBeginEnd const& v) {
        return bidirectional_iterator<char const*>(&v.ce);
    }
};
static_assert(std::is_invocable_v<RangeRBeginT, FunctionBeginEnd&>);
static_assert(std::is_invocable_v<RangeRBeginT, FunctionBeginEnd const&>);
static_assert(std::is_invocable_v<RangeCRBeginT, FunctionBeginEnd const&>);

struct MemberBeginFunctionEnd {
    int b, e;
    char cb, ce;
    constexpr bidirectional_iterator<int*> begin() {
        return bidirectional_iterator<int*>(&b);
    }
    friend constexpr bidirectional_iterator<int*> end(
        MemberBeginFunctionEnd& v) {
        return bidirectional_iterator<int*>(&v.e);
    }
    constexpr bidirectional_iterator<char const*> begin() const {
        return bidirectional_iterator<char const*>(&cb);
    }
    friend constexpr bidirectional_iterator<char const*> end(
        MemberBeginFunctionEnd const& v) {
        return bidirectional_iterator<char const*>(&v.ce);
    }
};
static_assert(std::is_invocable_v<RangeRBeginT, MemberBeginFunctionEnd&>);
static_assert(std::is_invocable_v<RangeRBeginT, MemberBeginFunctionEnd const&>);
static_assert(
    std::is_invocable_v<RangeCRBeginT, MemberBeginFunctionEnd const&>);

struct FunctionBeginMemberEnd {
    int b, e;
    char cb, ce;
    friend constexpr bidirectional_iterator<int*> begin(
        FunctionBeginMemberEnd& v) {
        return bidirectional_iterator<int*>(&v.b);
    }
    constexpr bidirectional_iterator<int*> end() {
        return bidirectional_iterator<int*>(&e);
    }
    friend constexpr bidirectional_iterator<char const*> begin(
        FunctionBeginMemberEnd const& v) {
        return bidirectional_iterator<char const*>(&v.cb);
    }
    constexpr bidirectional_iterator<char const*> end() const {
        return bidirectional_iterator<char const*>(&ce);
    }
};
static_assert(std::is_invocable_v<RangeRBeginT, FunctionBeginMemberEnd&>);
static_assert(std::is_invocable_v<RangeRBeginT, FunctionBeginMemberEnd const&>);
static_assert(
    std::is_invocable_v<RangeCRBeginT, FunctionBeginMemberEnd const&>);

struct MemberBeginEndDifferentTypes {
    bidirectional_iterator<int*> begin();
    bidirectional_iterator<int const*> end();
};
static_assert(
    !std::is_invocable_v<RangeRBeginT, MemberBeginEndDifferentTypes&>);
static_assert(
    !std::is_invocable_v<RangeCRBeginT, MemberBeginEndDifferentTypes&>);

struct FunctionBeginEndDifferentTypes {
    friend bidirectional_iterator<int*> begin(FunctionBeginEndDifferentTypes&);
    friend bidirectional_iterator<int const*> end(
        FunctionBeginEndDifferentTypes&);
};
static_assert(
    !std::is_invocable_v<RangeRBeginT, FunctionBeginEndDifferentTypes&>);
static_assert(
    !std::is_invocable_v<RangeCRBeginT, FunctionBeginEndDifferentTypes&>);

struct MemberBeginEndForwardIterators {
    forward_iterator<int*> begin();
    forward_iterator<int*> end();
};
static_assert(
    !std::is_invocable_v<RangeRBeginT, MemberBeginEndForwardIterators&>);
static_assert(
    !std::is_invocable_v<RangeCRBeginT, MemberBeginEndForwardIterators&>);

struct FunctionBeginEndForwardIterators {
    friend forward_iterator<int*> begin(FunctionBeginEndForwardIterators&);
    friend forward_iterator<int*> end(FunctionBeginEndForwardIterators&);
};
static_assert(
    !std::is_invocable_v<RangeRBeginT, FunctionBeginEndForwardIterators&>);
static_assert(
    !std::is_invocable_v<RangeCRBeginT, FunctionBeginEndForwardIterators&>);

struct MemberBeginOnly {
    bidirectional_iterator<int*> begin() const;
};
static_assert(!std::is_invocable_v<RangeRBeginT, MemberBeginOnly&>);
static_assert(!std::is_invocable_v<RangeCRBeginT, MemberBeginOnly&>);

struct FunctionBeginOnly {
    friend bidirectional_iterator<int*> begin(FunctionBeginOnly&);
};
static_assert(!std::is_invocable_v<RangeRBeginT, FunctionBeginOnly&>);
static_assert(!std::is_invocable_v<RangeCRBeginT, FunctionBeginOnly&>);

struct MemberEndOnly {
    bidirectional_iterator<int*> end() const;
};
static_assert(!std::is_invocable_v<RangeRBeginT, MemberEndOnly&>);
static_assert(!std::is_invocable_v<RangeCRBeginT, MemberEndOnly&>);

struct FunctionEndOnly {
    friend bidirectional_iterator<int*> end(FunctionEndOnly&);
};
static_assert(!std::is_invocable_v<RangeRBeginT, FunctionEndOnly&>);
static_assert(!std::is_invocable_v<RangeCRBeginT, FunctionEndOnly&>);

// Make sure there is no clash between the following cases:
// - the case that handles classes defining member `rbegin` and `rend`
// functions;
// - the case that handles classes defining `begin` and `end` functions
// returning reversible iterators.
struct MemberBeginAndRBegin {
    int* begin() const;
    int* end() const;
    int* rbegin() const;
    int* rend() const;
};
static_assert(std::is_invocable_v<RangeRBeginT, MemberBeginAndRBegin&>);
static_assert(std::is_invocable_v<RangeCRBeginT, MemberBeginAndRBegin&>);
static_assert(
    std::same_as<std::invoke_result_t<RangeRBeginT, MemberBeginAndRBegin&>,
        int*>);
static_assert(
    std::same_as<std::invoke_result_t<RangeCRBeginT, MemberBeginAndRBegin&>,
        rxx::basic_const_iterator<int*>>);

constexpr bool testBeginEnd() {
    MemberBeginEnd a{};
    MemberBeginEnd const aa{};
    assert(base(xranges::rbegin(a).base()) == &a.e);
    assert(base(xranges::crbegin(a).base()) == &a.ce);
    assert(base(xranges::rbegin(aa).base()) == &aa.ce);
    assert(base(xranges::crbegin(aa).base()) == &aa.ce);

    FunctionBeginEnd b{};
    FunctionBeginEnd const bb{};
    assert(base(xranges::rbegin(b).base()) == &b.e);
    assert(base(xranges::crbegin(b).base()) == &b.ce);
    assert(base(xranges::rbegin(bb).base()) == &bb.ce);
    assert(base(xranges::crbegin(bb).base()) == &bb.ce);

    MemberBeginFunctionEnd c{};
    MemberBeginFunctionEnd const cc{};
    assert(base(xranges::rbegin(c).base()) == &c.e);
    assert(base(xranges::crbegin(c).base()) == &c.ce);
    assert(base(xranges::rbegin(cc).base()) == &cc.ce);
    assert(base(xranges::crbegin(cc).base()) == &cc.ce);

    FunctionBeginMemberEnd d{};
    FunctionBeginMemberEnd const dd{};
    assert(base(xranges::rbegin(d).base()) == &d.e);
    assert(base(xranges::crbegin(d).base()) == &d.ce);
    assert(base(xranges::rbegin(dd).base()) == &dd.ce);
    assert(base(xranges::crbegin(dd).base()) == &dd.ce);

    return true;
}

// std::make_reverse_iterator not noexcept
ASSERT_NOT_NOEXCEPT(xranges::rbegin(std::declval<int (&)[10]>()));
ASSERT_NOT_NOEXCEPT(xranges::crbegin(std::declval<int (&)[10]>()));

struct NoThrowMemberRBegin {
    ThrowingIterator<int>
    rbegin() const noexcept; // auto(t.rbegin()) doesn't throw

    ThrowingIterator<int> begin() const noexcept;
    ThrowingIterator<int> end() const noexcept;
} ntmb;
static_assert(noexcept(xranges::rbegin(ntmb)));
static_assert(noexcept(xranges::crbegin(ntmb)));

struct NoThrowADLRBegin {
    friend ThrowingIterator<int> rbegin(
        NoThrowADLRBegin&) noexcept; // auto(rbegin(t)) doesn't throw
    friend ThrowingIterator<int> rbegin(NoThrowADLRBegin const&) noexcept;
    friend ThrowingIterator<int> begin(NoThrowADLRBegin const&) noexcept;
    friend ThrowingIterator<int> end(NoThrowADLRBegin const&) noexcept;
} ntab;
static_assert(noexcept(xranges::rbegin(ntab)));
static_assert(noexcept(xranges::crbegin(ntab)));

struct NoThrowMemberRBeginReturnsRef {
    ThrowingIterator<int>&
    rbegin() const noexcept; // auto(t.rbegin()) may throw
    ThrowingIterator<int>& begin() const noexcept;
    ThrowingIterator<int>& end() const noexcept;
} ntmbrr;
static_assert(!noexcept(xranges::rbegin(ntmbrr)));
static_assert(!noexcept(xranges::crbegin(ntmbrr)));

struct RBeginReturnsArrayRef {
    auto rbegin() const noexcept -> int (&)[10];
    auto begin() const noexcept -> int (&)[10];
    auto end() const noexcept -> int*;
} brar;
static_assert(noexcept(xranges::rbegin(brar)));
static_assert(noexcept(xranges::crbegin(brar)));

struct NoThrowBeginThrowingEnd {
    int* begin() const noexcept;
    int* end() const;
} ntbte;
static_assert(!noexcept(xranges::rbegin(ntbte)));
static_assert(!noexcept(xranges::crbegin(ntbte)));

struct NoThrowEndThrowingBegin {
    int* begin() const;
    int* end() const noexcept;
} ntetb;
// std::make_reverse_iterator not noexcept
static_assert(!noexcept(xranges::rbegin(ntetb)));
static_assert(!noexcept(xranges::crbegin(ntetb)));

// Test ADL-proofing.
struct Incomplete;
template <class T>
struct Holder {
    T t;
};
static_assert(!std::is_invocable_v<RangeRBeginT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeRBeginT, Holder<Incomplete>*&>);
static_assert(!std::is_invocable_v<RangeCRBeginT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeCRBeginT, Holder<Incomplete>*&>);

int main(int, char**) {
    static_assert(testReturnTypes());

    testArray();
    static_assert(testArray());

    testRBeginMember();
    static_assert(testRBeginMember());

    testRBeginFunction();
    static_assert(testRBeginFunction());

    testBeginEnd();
    static_assert(testBeginEnd());

    return 0;
}
