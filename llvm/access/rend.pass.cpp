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

// rxx::ranges::rend
// rxx::ranges::crend

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "rxx/ranges/access.h"

#include <cassert>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

using RangeREndT = decltype(xranges::rend);
using RangeCREndT = decltype(xranges::crend);

static int globalBuff[8];

static_assert(!std::is_invocable_v<RangeREndT, int (&&)[]>);
static_assert(!std::is_invocable_v<RangeREndT, int (&)[]>);
static_assert(!std::is_invocable_v<RangeREndT, int (&&)[10]>);
static_assert(std::is_invocable_v<RangeREndT, int (&)[10]>);
static_assert(!std::is_invocable_v<RangeCREndT, int (&&)[]>);
static_assert(!std::is_invocable_v<RangeCREndT, int (&)[]>);
static_assert(!std::is_invocable_v<RangeCREndT, int (&&)[10]>);
static_assert(std::is_invocable_v<RangeCREndT, int (&)[10]>);

struct Incomplete;
static_assert(!std::is_invocable_v<RangeREndT, Incomplete (&&)[]>);
static_assert(!std::is_invocable_v<RangeREndT, Incomplete (&&)[42]>);
static_assert(!std::is_invocable_v<RangeCREndT, Incomplete (&&)[]>);
static_assert(!std::is_invocable_v<RangeCREndT, Incomplete (&&)[42]>);

struct REndMember {
    int x;
    int const* rbegin() const;
    constexpr int const* rend() const { return &x; }

    int const* begin() const { return &x; }
    constexpr int const* end() const { return &x + 1; }
};

// Ensure that we can't call with rvalues with borrowing disabled.
static_assert(std::is_invocable_v<RangeREndT, REndMember&>);
static_assert(!std::is_invocable_v<RangeREndT, REndMember&&>);
static_assert(std::is_invocable_v<RangeREndT, REndMember const&>);
static_assert(!std::is_invocable_v<RangeREndT, REndMember const&&>);
static_assert(std::is_invocable_v<RangeCREndT, REndMember&>);
static_assert(!std::is_invocable_v<RangeCREndT, REndMember&&>);
static_assert(std::is_invocable_v<RangeCREndT, REndMember const&>);
static_assert(!std::is_invocable_v<RangeCREndT, REndMember const&&>);

constexpr bool testReturnTypes() {
    {
        int* x[2];
        ASSERT_SAME_TYPE(
            decltype(xranges::rend(x)), std::reverse_iterator<int**>);
        ASSERT_SAME_TYPE(
            decltype(xranges::crend(x)), std::reverse_iterator<int* const*>);
    }

    {
        int x[2][2];
        ASSERT_SAME_TYPE(
            decltype(xranges::rend(x)), std::reverse_iterator<int(*)[2]>);
        ASSERT_SAME_TYPE(decltype(xranges::crend(x)),
            std::reverse_iterator<int const(*)[2]>);
    }

    {
        struct Different {
            char* rbegin();
            sentinel_wrapper<char*>& rend();
            short* rbegin() const;
            sentinel_wrapper<short*>& rend() const;

            char* begin();
            sentinel_wrapper<char*>& end();
            short* begin() const;
            sentinel_wrapper<short*>& end() const;
        } x;
        ASSERT_SAME_TYPE(decltype(xranges::rend(x)), sentinel_wrapper<char*>);
        ASSERT_SAME_TYPE(decltype(xranges::crend(x)), sentinel_wrapper<short*>);
    }

    return true;
}

constexpr bool testArray() {
    int a[2];
    assert(xranges::rend(a).base() == a);
    assert(xranges::crend(a).base() == a);

    int b[2][2];
    assert(xranges::rend(b).base() == b);
    assert(xranges::crend(b).base() == b);

    REndMember c[2];
    assert(xranges::rend(c).base() == c);
    assert(xranges::crend(c).base() == c);

    return true;
}

struct REndMemberReturnsInt {
    int rbegin() const;
    int rend() const;
};
static_assert(!std::is_invocable_v<RangeREndT, REndMemberReturnsInt const&>);

struct REndMemberReturnsVoidPtr {
    void const* rbegin() const;
    void const* rend() const;
};
static_assert(
    !std::is_invocable_v<RangeREndT, REndMemberReturnsVoidPtr const&>);

struct PtrConvertible {
    operator int*() const;
};
struct PtrConvertibleREndMember {
    PtrConvertible rbegin() const;
    PtrConvertible rend() const;
};
static_assert(
    !std::is_invocable_v<RangeREndT, PtrConvertibleREndMember const&>);

struct NoRBeginMember {
    constexpr int const* rend();
};
static_assert(!std::is_invocable_v<RangeREndT, NoRBeginMember const&>);

struct NonConstREndMember {
    int x;
    constexpr int* rbegin() { return nullptr; }
    constexpr int* rend() { return &x; }
};
static_assert(std::is_invocable_v<RangeREndT, NonConstREndMember&>);
static_assert(!std::is_invocable_v<RangeREndT, NonConstREndMember const&>);
static_assert(!std::is_invocable_v<RangeCREndT, NonConstREndMember&>);
static_assert(!std::is_invocable_v<RangeCREndT, NonConstREndMember const&>);

struct EnabledBorrowingREndMember {
    constexpr int* rbegin() const { return nullptr; }
    constexpr int* rend() const { return &globalBuff[0]; }

    constexpr int* begin() const { return &globalBuff[0]; }
    constexpr int* end() const { return nullptr; }
};

template <>
inline constexpr bool
    std::ranges::enable_borrowed_range<EnabledBorrowingREndMember> = true;

struct REndMemberFunction {
    int x;
    constexpr int const* rbegin() const { return nullptr; }
    constexpr int const* rend() const { return &x; }
    constexpr int const* begin() const { return &x; }
    constexpr int const* end() const { return nullptr; }
    friend constexpr int* rend(REndMemberFunction const&);
};

struct Empty {};
struct EmptyEndMember {
    Empty rbegin() const;
    Empty rend() const;
};
static_assert(!std::is_invocable_v<RangeREndT, EmptyEndMember const&>);

struct EmptyPtrREndMember {
    Empty x;
    constexpr Empty const* rbegin() const { return nullptr; }
    constexpr Empty const* rend() const { return &x; }
    constexpr Empty const* begin() const { return &x; }
    constexpr Empty const* end() const { return nullptr; }
};

constexpr bool testREndMember() {
    REndMember a;
    assert(xranges::rend(a) == &a.x);
    assert(xranges::crend(a) == &a.x);

    NonConstREndMember b;
    assert(xranges::rend(b) == &b.x);
    static_assert(!std::is_invocable_v<RangeCREndT, decltype((b))>);

    EnabledBorrowingREndMember c;
    assert(xranges::rend(std::move(c)) == &globalBuff[0]);
    assert(xranges::crend(std::move(c)) == &globalBuff[0]);

    REndMemberFunction d;
    assert(xranges::rend(d) == &d.x);
    assert(xranges::crend(d) == &d.x);

    EmptyPtrREndMember e;
    assert(xranges::rend(e) == &e.x);
    assert(xranges::crend(e) == &e.x);

    return true;
}

struct REndFunction {
    int x;
    friend constexpr int const* rbegin(REndFunction const&) { return nullptr; }
    friend constexpr int const* rend(REndFunction const& bf) { return &bf.x; }

    friend constexpr int const* begin(REndFunction const& bf) { return &bf.x; }
    friend constexpr int const* end(REndFunction const&) { return nullptr; }
};

static_assert(std::is_invocable_v<RangeREndT, REndFunction const&>);
static_assert(!std::is_invocable_v<RangeREndT, REndFunction&&>);

static_assert(std::is_invocable_v<RangeREndT, REndFunction const&>);
static_assert(!std::is_invocable_v<RangeREndT, REndFunction&&>);
static_assert(std::is_invocable_v<RangeREndT,
    REndFunction&>); // Ill-formed before P2602R2 Poison Pills are Too Toxic
static_assert(std::is_invocable_v<RangeCREndT, REndFunction const&>);
static_assert(std::is_invocable_v<RangeCREndT, REndFunction&>);

struct REndFunctionReturnsInt {
    friend constexpr int rbegin(REndFunctionReturnsInt const&);
    friend constexpr int rend(REndFunctionReturnsInt const&);
    friend constexpr int begin(REndFunctionReturnsInt const&);
    friend constexpr int end(REndFunctionReturnsInt const&);
};
static_assert(!std::is_invocable_v<RangeREndT, REndFunctionReturnsInt const&>);

struct REndFunctionReturnsVoidPtr {
    friend constexpr void* rbegin(REndFunctionReturnsVoidPtr const&);
    friend constexpr void* rend(REndFunctionReturnsVoidPtr const&);
    friend constexpr void* begin(REndFunctionReturnsVoidPtr const&);
    friend constexpr void* end(REndFunctionReturnsVoidPtr const&);
};
static_assert(
    !std::is_invocable_v<RangeREndT, REndFunctionReturnsVoidPtr const&>);

struct REndFunctionReturnsEmpty {
    friend constexpr Empty rbegin(REndFunctionReturnsEmpty const&);
    friend constexpr Empty rend(REndFunctionReturnsEmpty const&);
    friend constexpr Empty begin(REndFunctionReturnsEmpty const&);
    friend constexpr Empty end(REndFunctionReturnsEmpty const&);
};
static_assert(
    !std::is_invocable_v<RangeREndT, REndFunctionReturnsEmpty const&>);

struct REndFunctionReturnsPtrConvertible {
    friend constexpr PtrConvertible rbegin(
        REndFunctionReturnsPtrConvertible const&);
    friend constexpr PtrConvertible rend(
        REndFunctionReturnsPtrConvertible const&);
    friend constexpr PtrConvertible begin(
        REndFunctionReturnsPtrConvertible const&);
    friend constexpr PtrConvertible end(
        REndFunctionReturnsPtrConvertible const&);
};
static_assert(
    !std::is_invocable_v<RangeREndT, REndFunctionReturnsPtrConvertible const&>);

struct NoRBeginFunction {
    friend constexpr int const* rend(NoRBeginFunction const&);
};
static_assert(!std::is_invocable_v<RangeREndT, NoRBeginFunction const&>);

struct REndFunctionByValue {
    friend constexpr int* rbegin(REndFunctionByValue) { return nullptr; }
    friend constexpr int* rend(REndFunctionByValue) { return &globalBuff[1]; }

    friend constexpr int* begin(REndFunctionByValue) { return &globalBuff[1]; }
    friend constexpr int* end(REndFunctionByValue) { return nullptr; }
};
static_assert(!std::is_invocable_v<RangeCREndT, REndFunctionByValue>);

struct REndFunctionEnabledBorrowing {
    friend constexpr int* rbegin(REndFunctionEnabledBorrowing) {
        return nullptr;
    }
    friend constexpr int* rend(REndFunctionEnabledBorrowing) {
        return &globalBuff[2];
    }

    friend constexpr int* begin(REndFunctionEnabledBorrowing) {
        return &globalBuff[2];
    }
    friend constexpr int* end(REndFunctionEnabledBorrowing) { return nullptr; }
};
template <>
inline constexpr bool
    std::ranges::enable_borrowed_range<REndFunctionEnabledBorrowing> = true;

struct REndFunctionReturnsEmptyPtr {
    Empty x;
    friend constexpr Empty const* rbegin(REndFunctionReturnsEmptyPtr const&) {
        return nullptr;
    }
    friend constexpr Empty const* rend(REndFunctionReturnsEmptyPtr const& bf) {
        return &bf.x;
    }

    friend constexpr Empty const* begin(REndFunctionReturnsEmptyPtr const& bf) {
        return &bf.x;
    }
    friend constexpr Empty const* end(REndFunctionReturnsEmptyPtr const&) {
        return nullptr;
    }
};

struct REndFunctionWithDataMember {
    int x;
    int rend;
    friend constexpr int const* rbegin(REndFunctionWithDataMember const&) {
        return nullptr;
    }
    friend constexpr int const* rend(REndFunctionWithDataMember const& bf) {
        return &bf.x;
    }

    friend constexpr int const* begin(REndFunctionWithDataMember const& bf) {
        return &bf.x;
    }
    friend constexpr int const* end(REndFunctionWithDataMember const&) {
        return nullptr;
    }
};

struct REndFunctionWithPrivateEndMember : private REndMember {
    int y;
    friend constexpr int const* rbegin(
        REndFunctionWithPrivateEndMember const&) {
        return nullptr;
    }
    friend constexpr int const* rend(
        REndFunctionWithPrivateEndMember const& bf) {
        return &bf.y;
    }

    friend constexpr int const* begin(
        REndFunctionWithPrivateEndMember const& bf) {
        return &bf.y;
    }
    friend constexpr int const* end(REndFunctionWithPrivateEndMember const&) {
        return nullptr;
    }
};

struct RBeginMemberEndFunction {
    int x;
    constexpr int const* rbegin() const { return nullptr; }
    friend constexpr int const* rend(RBeginMemberEndFunction const& bf) {
        return &bf.x;
    }

    constexpr int const* begin() const { return &x; }
    constexpr int const* end() const { return nullptr; }
};

constexpr bool testREndFunction() {
    REndFunction const a{};
    assert(xranges::rend(a) == &a.x);
    assert(xranges::crend(a) == &a.x);
    REndFunction aa{};
    assert(xranges::rend(aa) ==
        &aa.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::crend(aa) == &aa.x);

    REndFunctionByValue b;
    assert(xranges::rend(b) == &globalBuff[1]);
    assert(xranges::crend(b) == &globalBuff[1]);

    REndFunctionEnabledBorrowing c;
    assert(xranges::rend(std::move(c)) == &globalBuff[2]);
    assert(xranges::crend(std::move(c)) == &globalBuff[2]);

    REndFunctionReturnsEmptyPtr const d{};
    assert(xranges::rend(d) == &d.x);
    assert(xranges::crend(d) == &d.x);
    REndFunctionReturnsEmptyPtr dd{};
    assert(xranges::rend(dd) ==
        &dd.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::crend(dd) == &dd.x);

    REndFunctionWithDataMember const e{};
    assert(xranges::rend(e) == &e.x);
    assert(xranges::crend(e) == &e.x);
    REndFunctionWithDataMember ee{};
    assert(xranges::rend(ee) ==
        &ee.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::crend(ee) == &ee.x);

    REndFunctionWithPrivateEndMember const f{};
    assert(xranges::rend(f) == &f.y);
    assert(xranges::crend(f) == &f.y);
    REndFunctionWithPrivateEndMember ff{};
    assert(xranges::rend(ff) ==
        &ff.y); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::crend(ff) == &ff.y);

    RBeginMemberEndFunction const g{};
    assert(xranges::rend(g) == &g.x);
    assert(xranges::crend(g) == &g.x);
    RBeginMemberEndFunction gg{};
    assert(xranges::rend(gg) ==
        &gg.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::crend(gg) == &gg.x);

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
static_assert(std::is_invocable_v<RangeREndT, MemberBeginEnd&>);
static_assert(std::is_invocable_v<RangeREndT, MemberBeginEnd const&>);
static_assert(std::is_invocable_v<RangeCREndT, MemberBeginEnd const&>);

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
static_assert(std::is_invocable_v<RangeREndT, FunctionBeginEnd&>);
static_assert(std::is_invocable_v<RangeREndT, FunctionBeginEnd const&>);
static_assert(std::is_invocable_v<RangeCREndT, FunctionBeginEnd const&>);

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
static_assert(std::is_invocable_v<RangeREndT, MemberBeginFunctionEnd&>);
static_assert(std::is_invocable_v<RangeREndT, MemberBeginFunctionEnd const&>);
static_assert(std::is_invocable_v<RangeCREndT, MemberBeginFunctionEnd const&>);

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
static_assert(std::is_invocable_v<RangeREndT, FunctionBeginMemberEnd&>);
static_assert(std::is_invocable_v<RangeREndT, FunctionBeginMemberEnd const&>);
static_assert(std::is_invocable_v<RangeCREndT, FunctionBeginMemberEnd const&>);

struct MemberBeginEndDifferentTypes {
    bidirectional_iterator<int*> begin();
    bidirectional_iterator<int const*> end();
};
static_assert(!std::is_invocable_v<RangeREndT, MemberBeginEndDifferentTypes&>);
static_assert(!std::is_invocable_v<RangeCREndT, MemberBeginEndDifferentTypes&>);

struct FunctionBeginEndDifferentTypes {
    friend bidirectional_iterator<int*> begin(FunctionBeginEndDifferentTypes&);
    friend bidirectional_iterator<int const*> end(
        FunctionBeginEndDifferentTypes&);
};
static_assert(
    !std::is_invocable_v<RangeREndT, FunctionBeginEndDifferentTypes&>);
static_assert(
    !std::is_invocable_v<RangeCREndT, FunctionBeginEndDifferentTypes&>);

struct MemberBeginEndForwardIterators {
    forward_iterator<int*> begin();
    forward_iterator<int*> end();
};
static_assert(
    !std::is_invocable_v<RangeREndT, MemberBeginEndForwardIterators&>);
static_assert(
    !std::is_invocable_v<RangeCREndT, MemberBeginEndForwardIterators&>);

struct FunctionBeginEndForwardIterators {
    friend forward_iterator<int*> begin(FunctionBeginEndForwardIterators&);
    friend forward_iterator<int*> end(FunctionBeginEndForwardIterators&);
};
static_assert(
    !std::is_invocable_v<RangeREndT, FunctionBeginEndForwardIterators&>);
static_assert(
    !std::is_invocable_v<RangeCREndT, FunctionBeginEndForwardIterators&>);

struct MemberBeginOnly {
    bidirectional_iterator<int*> begin() const;
};
static_assert(!std::is_invocable_v<RangeREndT, MemberBeginOnly&>);
static_assert(!std::is_invocable_v<RangeCREndT, MemberBeginOnly&>);

struct FunctionBeginOnly {
    friend bidirectional_iterator<int*> begin(FunctionBeginOnly&);
};
static_assert(!std::is_invocable_v<RangeREndT, FunctionBeginOnly&>);
static_assert(!std::is_invocable_v<RangeCREndT, FunctionBeginOnly&>);

struct MemberEndOnly {
    bidirectional_iterator<int*> end() const;
};
static_assert(!std::is_invocable_v<RangeREndT, MemberEndOnly&>);
static_assert(!std::is_invocable_v<RangeCREndT, MemberEndOnly&>);

struct FunctionEndOnly {
    friend bidirectional_iterator<int*> end(FunctionEndOnly&);
};
static_assert(!std::is_invocable_v<RangeREndT, FunctionEndOnly&>);
static_assert(!std::is_invocable_v<RangeCREndT, FunctionEndOnly&>);

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
static_assert(std::is_invocable_v<RangeREndT, MemberBeginAndRBegin&>);
static_assert(std::is_invocable_v<RangeCREndT, MemberBeginAndRBegin&>);
static_assert(
    std::same_as<std::invoke_result_t<RangeREndT, MemberBeginAndRBegin&>,
        int*>);
static_assert(
    std::same_as<std::invoke_result_t<RangeCREndT, MemberBeginAndRBegin&>,
        rxx::basic_const_iterator<int*>>);

constexpr bool testBeginEnd() {
    MemberBeginEnd a{};
    MemberBeginEnd const aa{};
    assert(base(xranges::rend(a).base()) == &a.b);
    assert(base(xranges::crend(a).base()) == &a.cb);
    assert(base(xranges::rend(aa).base()) == &aa.cb);
    assert(base(xranges::crend(aa).base()) == &aa.cb);

    FunctionBeginEnd b{};
    FunctionBeginEnd const bb{};
    assert(base(xranges::rend(b).base()) == &b.b);
    assert(base(xranges::crend(b).base()) == &b.cb);
    assert(base(xranges::rend(bb).base()) == &bb.cb);
    assert(base(xranges::crend(bb).base()) == &bb.cb);

    MemberBeginFunctionEnd c{};
    MemberBeginFunctionEnd const cc{};
    assert(base(xranges::rend(c).base()) == &c.b);
    assert(base(xranges::crend(c).base()) == &c.cb);
    assert(base(xranges::rend(cc).base()) == &cc.cb);
    assert(base(xranges::crend(cc).base()) == &cc.cb);

    FunctionBeginMemberEnd d{};
    FunctionBeginMemberEnd const dd{};
    assert(base(xranges::rend(d).base()) == &d.b);
    assert(base(xranges::crend(d).base()) == &d.cb);
    assert(base(xranges::rend(dd).base()) == &dd.cb);
    assert(base(xranges::crend(dd).base()) == &dd.cb);

    return true;
}

// make_reverse_iterator is not noexcept
ASSERT_NOT_NOEXCEPT(xranges::rend(std::declval<int (&)[10]>()));
// make_reverse_iterator is not noexcept
ASSERT_NOT_NOEXCEPT(xranges::crend(std::declval<int (&)[10]>()));

struct NoThrowMemberREnd {
    ThrowingIterator<int> rbegin() const;
    ThrowingIterator<int> rend() const noexcept; // auto(t.rend()) doesn't throw

    int const* begin() const;
    int const* end() const;
} ntmre;
static_assert(noexcept(xranges::rend(ntmre)));
static_assert(noexcept(xranges::crend(ntmre)));

struct NoThrowADLREnd {
    ThrowingIterator<int> rbegin() const;
    friend ThrowingIterator<int> rend(
        NoThrowADLREnd&) noexcept; // auto(rend(t)) doesn't throw
    friend ThrowingIterator<int> rend(NoThrowADLREnd const&) noexcept;
    int const* begin() const;
    int const* end() const;
} ntare;
static_assert(noexcept(xranges::rend(ntare)));
static_assert(noexcept(xranges::crend(ntare)));

struct NoThrowMemberREndReturnsRef {
    ThrowingIterator<int> rbegin() const;
    ThrowingIterator<int>& rend() const noexcept; // auto(t.rend()) may throw
    int const* begin() const;
    int const* end() const;
} ntmrerr;
static_assert(!noexcept(xranges::rend(ntmrerr)));
static_assert(!noexcept(xranges::crend(ntmrerr)));

struct REndReturnsArrayRef {
    auto rbegin() const noexcept -> int (&)[10];
    auto rend() const noexcept -> int (&)[10];
    auto begin() const noexcept -> int (&)[10];
    auto end() const noexcept -> int (&)[10];
} rerar;
static_assert(noexcept(xranges::rend(rerar)));
static_assert(noexcept(xranges::crend(rerar)));

struct NoThrowBeginThrowingEnd {
    int* begin() const noexcept;
    int* end() const;
} ntbte;
// make_reverse_iterator is not noexcept
static_assert(!noexcept(xranges::rend(ntbte)));
static_assert(!noexcept(xranges::crend(ntbte)));

struct NoThrowEndThrowingBegin {
    int* begin() const;
    int* end() const noexcept;
} ntetb;
static_assert(!noexcept(xranges::rend(ntetb)));
static_assert(!noexcept(xranges::crend(ntetb)));

// Test ADL-proofing.
struct Incomplete;
template <class T>
struct Holder {
    T t;
};
static_assert(!std::is_invocable_v<RangeREndT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeREndT, Holder<Incomplete>*&>);
static_assert(!std::is_invocable_v<RangeCREndT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeCREndT, Holder<Incomplete>*&>);

int main(int, char**) {
    static_assert(testReturnTypes());

    testArray();
    static_assert(testArray());

    testREndMember();
    static_assert(testREndMember());

    testREndFunction();
    static_assert(testREndFunction());

    testBeginEnd();
    static_assert(testBeginEnd());

    return 0;
}
