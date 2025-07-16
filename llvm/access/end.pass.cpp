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

// xranges::end
// xranges::cend

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "rxx/access.h"

#include <cassert>
#include <ranges>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

using RangeEndT = decltype(xranges::end);
using RangeCEndT = decltype(xranges::cend);

static int globalBuff[8];

static_assert(!std::is_invocable_v<RangeEndT, int (&&)[]>);
static_assert(!std::is_invocable_v<RangeEndT, int (&)[]>);
static_assert(!std::is_invocable_v<RangeEndT, int (&&)[10]>);
static_assert(std::is_invocable_v<RangeEndT, int (&)[10]>);
static_assert(!std::is_invocable_v<RangeCEndT, int (&&)[]>);
static_assert(!std::is_invocable_v<RangeCEndT, int (&)[]>);
static_assert(!std::is_invocable_v<RangeCEndT, int (&&)[10]>);
static_assert(std::is_invocable_v<RangeCEndT, int (&)[10]>);

struct Incomplete;
static_assert(!std::is_invocable_v<RangeEndT, Incomplete (&&)[]>);
static_assert(!std::is_invocable_v<RangeEndT, Incomplete (&&)[42]>);
static_assert(!std::is_invocable_v<RangeCEndT, Incomplete (&&)[]>);
static_assert(!std::is_invocable_v<RangeCEndT, Incomplete (&&)[42]>);

struct EndMember {
    int x;
    int const* begin() const;
    constexpr int const* end() const { return &x; }
};

// Ensure that we can't call with rvalues with borrowing disabled.
static_assert(std::is_invocable_v<RangeEndT, EndMember&>);
static_assert(!std::is_invocable_v<RangeEndT, EndMember&&>);
static_assert(std::is_invocable_v<RangeEndT, EndMember const&>);
static_assert(!std::is_invocable_v<RangeEndT, EndMember const&&>);
static_assert(std::is_invocable_v<RangeCEndT, EndMember&>);
static_assert(!std::is_invocable_v<RangeCEndT, EndMember&&>);
static_assert(std::is_invocable_v<RangeCEndT, EndMember const&>);
static_assert(!std::is_invocable_v<RangeCEndT, EndMember const&&>);

constexpr bool testReturnTypes() {
    {
        int* x[2];
        ASSERT_SAME_TYPE(decltype(xranges::end(x)), int**);
        ASSERT_SAME_TYPE(decltype(xranges::cend(x)), int* const*);
    }
    {
        int x[2][2];
        ASSERT_SAME_TYPE(decltype(xranges::end(x)), int(*)[2]);
        ASSERT_SAME_TYPE(decltype(xranges::cend(x)), int const(*)[2]);
    }
    {
        struct Different {
            char* begin();
            sentinel_wrapper<char*>& end();
            short* begin() const;
            sentinel_wrapper<short*>& end() const;
        } x;
        ASSERT_SAME_TYPE(decltype(xranges::end(x)), sentinel_wrapper<char*>);
        ASSERT_SAME_TYPE(decltype(xranges::cend(x)), sentinel_wrapper<short*>);
    }
    return true;
}

constexpr bool testArray() {
    int a[2];
    assert(xranges::end(a) == a + 2);
    assert(xranges::cend(a) == a + 2);

    int b[2][2];
    assert(xranges::end(b) == b + 2);
    assert(xranges::cend(b) == b + 2);

    EndMember c[2];
    assert(xranges::end(c) == c + 2);
    assert(xranges::cend(c) == c + 2);

    return true;
}

struct EndMemberReturnsInt {
    int begin() const;
    int end() const;
};
static_assert(!std::is_invocable_v<RangeEndT, EndMemberReturnsInt const&>);

struct EndMemberReturnsVoidPtr {
    void const* begin() const;
    void const* end() const;
};
static_assert(!std::is_invocable_v<RangeEndT, EndMemberReturnsVoidPtr const&>);

struct PtrConvertible {
    operator int*() const;
};
struct PtrConvertibleEndMember {
    PtrConvertible begin() const;
    PtrConvertible end() const;
};
static_assert(!std::is_invocable_v<RangeEndT, PtrConvertibleEndMember const&>);

struct NoBeginMember {
    constexpr int const* end();
};
static_assert(!std::is_invocable_v<RangeEndT, NoBeginMember const&>);

struct NonConstEndMember {
    int x;
    constexpr int* begin() { return nullptr; }
    constexpr int* end() { return &x; }
};
static_assert(std::is_invocable_v<RangeEndT, NonConstEndMember&>);
static_assert(!std::is_invocable_v<RangeEndT, NonConstEndMember const&>);
static_assert(std::is_invocable_v<RangeCEndT, NonConstEndMember&>);
static_assert(!std::is_invocable_v<RangeCEndT, NonConstEndMember const&>);

struct EnabledBorrowingEndMember {
    constexpr int* begin() const { return nullptr; }
    constexpr int* end() const { return &globalBuff[0]; }
};

template <>
inline constexpr bool
    std::ranges::enable_borrowed_range<EnabledBorrowingEndMember> = true;

struct EndMemberFunction {
    int x;
    constexpr int const* begin() const { return nullptr; }
    constexpr int const* end() const { return &x; }
    friend constexpr int* end(EndMemberFunction const&);
};

struct Empty {};
struct EmptyEndMember {
    Empty begin() const;
    Empty end() const;
};
static_assert(!std::is_invocable_v<RangeEndT, EmptyEndMember const&>);

struct EmptyPtrEndMember {
    Empty x;
    constexpr Empty const* begin() const { return nullptr; }
    constexpr Empty const* end() const { return &x; }
};

constexpr bool testEndMember() {
    EndMember a;
    assert(xranges::end(a) == &a.x);
    assert(xranges::cend(a) == &a.x);

    NonConstEndMember b;
    assert(xranges::end(b) == &b.x);
    static_assert(std::is_invocable_v<RangeCEndT, decltype((b))>);

    EnabledBorrowingEndMember c;
    assert(xranges::end(std::move(c)) == &globalBuff[0]);
    assert(xranges::cend(std::move(c)) == &globalBuff[0]);

    EndMemberFunction d;
    assert(xranges::end(d) == &d.x);
    assert(xranges::cend(d) == &d.x);

    EmptyPtrEndMember e;
    assert(xranges::end(e) == &e.x);
    assert(xranges::cend(e) == &e.x);

    return true;
}

struct EndFunction {
    int x;
    friend constexpr int const* begin(EndFunction const&) { return nullptr; }
    friend constexpr int const* end(EndFunction const& bf) { return &bf.x; }
};

static_assert(std::is_invocable_v<RangeEndT, EndFunction const&>);
static_assert(!std::is_invocable_v<RangeEndT, EndFunction&&>);

static_assert(std::is_invocable_v<RangeEndT, EndFunction const&>);
static_assert(!std::is_invocable_v<RangeEndT, EndFunction&&>);
static_assert(std::is_invocable_v<RangeEndT,
    EndFunction&>); // Ill-formed before P2602R2 Poison Pills are Too Toxic
static_assert(std::is_invocable_v<RangeCEndT, EndFunction const&>);
static_assert(std::is_invocable_v<RangeCEndT, EndFunction&>);

struct EndFunctionReturnsInt {
    friend constexpr int begin(EndFunctionReturnsInt const&);
    friend constexpr int end(EndFunctionReturnsInt const&);
};
static_assert(!std::is_invocable_v<RangeEndT, EndFunctionReturnsInt const&>);

struct EndFunctionReturnsVoidPtr {
    friend constexpr void* begin(EndFunctionReturnsVoidPtr const&);
    friend constexpr void* end(EndFunctionReturnsVoidPtr const&);
};
static_assert(
    !std::is_invocable_v<RangeEndT, EndFunctionReturnsVoidPtr const&>);

struct EndFunctionReturnsEmpty {
    friend constexpr Empty begin(EndFunctionReturnsEmpty const&);
    friend constexpr Empty end(EndFunctionReturnsEmpty const&);
};
static_assert(!std::is_invocable_v<RangeEndT, EndFunctionReturnsEmpty const&>);

struct EndFunctionReturnsPtrConvertible {
    friend constexpr PtrConvertible begin(
        EndFunctionReturnsPtrConvertible const&);
    friend constexpr PtrConvertible end(
        EndFunctionReturnsPtrConvertible const&);
};
static_assert(
    !std::is_invocable_v<RangeEndT, EndFunctionReturnsPtrConvertible const&>);

struct NoBeginFunction {
    friend constexpr int const* end(NoBeginFunction const&);
};
static_assert(!std::is_invocable_v<RangeEndT, NoBeginFunction const&>);

struct EndFunctionByValue {
    friend constexpr int* begin(EndFunctionByValue) { return nullptr; }
    friend constexpr int* end(EndFunctionByValue) { return &globalBuff[1]; }
};
static_assert(!std::is_invocable_v<RangeCEndT, EndFunctionByValue>);

struct EndFunctionEnabledBorrowing {
    friend constexpr int* begin(EndFunctionEnabledBorrowing) { return nullptr; }
    friend constexpr int* end(EndFunctionEnabledBorrowing) {
        return &globalBuff[2];
    }
};
template <>
inline constexpr bool
    std::ranges::enable_borrowed_range<EndFunctionEnabledBorrowing> = true;

struct EndFunctionReturnsEmptyPtr {
    Empty x;
    friend constexpr Empty const* begin(EndFunctionReturnsEmptyPtr const&) {
        return nullptr;
    }
    friend constexpr Empty const* end(EndFunctionReturnsEmptyPtr const& bf) {
        return &bf.x;
    }
};

struct EndFunctionWithDataMember {
    int x;
    int end;
    friend constexpr int const* begin(EndFunctionWithDataMember const&) {
        return nullptr;
    }
    friend constexpr int const* end(EndFunctionWithDataMember const& bf) {
        return &bf.x;
    }
};

struct EndFunctionWithPrivateEndMember {
    int y;
    friend constexpr int const* begin(EndFunctionWithPrivateEndMember const&) {
        return nullptr;
    }
    friend constexpr int const* end(EndFunctionWithPrivateEndMember const& bf) {
        return &bf.y;
    }

private:
    int const* end() const;
};

struct BeginMemberEndFunction {
    int x;
    constexpr int const* begin() const { return nullptr; }
    friend constexpr int const* end(BeginMemberEndFunction const& bf) {
        return &bf.x;
    }
};

constexpr bool testEndFunction() {
    EndFunction const a{};
    assert(xranges::end(a) == &a.x);
    assert(xranges::cend(a) == &a.x);
    EndFunction aa{};
    assert(xranges::end(aa) ==
        &aa.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::cend(aa) == &aa.x);

    EndFunctionByValue b;
    assert(xranges::end(b) == &globalBuff[1]);
    assert(xranges::cend(b) == &globalBuff[1]);

    EndFunctionEnabledBorrowing c;
    assert(xranges::end(std::move(c)) == &globalBuff[2]);
    assert(xranges::cend(std::move(c)) == &globalBuff[2]);

    EndFunctionReturnsEmptyPtr const d{};
    assert(xranges::end(d) == &d.x);
    assert(xranges::cend(d) == &d.x);
    EndFunctionReturnsEmptyPtr dd{};
    assert(xranges::end(dd) ==
        &dd.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::cend(dd) == &dd.x);

    EndFunctionWithDataMember const e{};
    assert(xranges::end(e) == &e.x);
    assert(xranges::cend(e) == &e.x);
    EndFunctionWithDataMember ee{};
    assert(xranges::end(ee) ==
        &ee.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::cend(ee) == &ee.x);

    EndFunctionWithPrivateEndMember const f{};
    assert(xranges::end(f) == &f.y);
    assert(xranges::cend(f) == &f.y);
    EndFunctionWithPrivateEndMember ff{};
    assert(xranges::end(ff) ==
        &ff.y); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::cend(ff) == &ff.y);

    BeginMemberEndFunction const g{};
    assert(xranges::end(g) == &g.x);
    assert(xranges::cend(g) == &g.x);
    BeginMemberEndFunction gg{};
    assert(xranges::end(gg) ==
        &gg.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::cend(gg) == &gg.x);

    return true;
}

ASSERT_NOEXCEPT(xranges::end(std::declval<int (&)[10]>()));
ASSERT_NOEXCEPT(xranges::cend(std::declval<int (&)[10]>()));

struct NoThrowMemberEnd {
    ThrowingIterator<int> begin() const;
    ThrowingIterator<int> end() const noexcept; // auto(t.end()) doesn't throw
} ntme;
static_assert(noexcept(xranges::end(ntme)));
static_assert(noexcept(xranges::cend(ntme)));

struct NoThrowADLEnd {
    ThrowingIterator<int> begin() const;
    friend ThrowingIterator<int> end(
        NoThrowADLEnd&) noexcept; // auto(end(t)) doesn't throw
    friend ThrowingIterator<int> end(NoThrowADLEnd const&) noexcept;
} ntae;
static_assert(noexcept(xranges::end(ntae)));
static_assert(noexcept(xranges::cend(ntae)));

struct NoThrowMemberEndReturnsRef {
    ThrowingIterator<int> begin() const;
    ThrowingIterator<int>& end() const noexcept; // auto(t.end()) may throw
} ntmerr;
static_assert(!noexcept(xranges::end(ntmerr)));
static_assert(!noexcept(xranges::cend(ntmerr)));

struct EndReturnsArrayRef {
    auto begin() const noexcept -> int (&)[10];
    auto end() const noexcept -> int (&)[10];
} erar;
static_assert(noexcept(xranges::end(erar)));
static_assert(noexcept(xranges::cend(erar)));

// Test ADL-proofing.
struct Incomplete;
template <class T>
struct Holder {
    T t;
};
static_assert(!std::is_invocable_v<RangeEndT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeEndT, Holder<Incomplete>*&>);
static_assert(!std::is_invocable_v<RangeCEndT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeCEndT, Holder<Incomplete>*&>);

int main(int, char**) {
    static_assert(testReturnTypes());

    testArray();
    static_assert(testArray());

    testEndMember();
    static_assert(testEndMember());

    testEndFunction();
    static_assert(testEndFunction());

    return 0;
}
