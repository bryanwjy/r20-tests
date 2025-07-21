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

// std::ranges::begin
// std::ranges::cbegin

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "rxx/ranges/access.h"

#include <cassert>
#include <ranges>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

using RangeBeginT = decltype(xranges::begin);
using RangeCBeginT = decltype(xranges::cbegin);

static int globalBuff[8];

static_assert(!std::is_invocable_v<RangeBeginT, int (&&)[10]>);
static_assert(std::is_invocable_v<RangeBeginT, int (&)[10]>);
static_assert(!std::is_invocable_v<RangeBeginT, int (&&)[]>);
static_assert(std::is_invocable_v<RangeBeginT, int (&)[]>);
static_assert(!std::is_invocable_v<RangeCBeginT, int (&&)[10]>);
static_assert(std::is_invocable_v<RangeCBeginT, int (&)[10]>);
static_assert(!std::is_invocable_v<RangeCBeginT, int (&&)[]>);
static_assert(!std::is_invocable_v<RangeCBeginT, int (&)[]>,
    "No longer allowed in C++23, must be input_range");

struct Incomplete;
static_assert(!std::is_invocable_v<RangeBeginT, Incomplete (&&)[]>);
static_assert(!std::is_invocable_v<RangeBeginT, Incomplete const (&&)[]>);
static_assert(!std::is_invocable_v<RangeCBeginT, Incomplete (&&)[]>);
static_assert(!std::is_invocable_v<RangeCBeginT, Incomplete const (&&)[]>);

static_assert(!std::is_invocable_v<RangeBeginT, Incomplete (&&)[10]>);
static_assert(!std::is_invocable_v<RangeBeginT, Incomplete const (&&)[10]>);
static_assert(!std::is_invocable_v<RangeCBeginT, Incomplete (&&)[10]>);
static_assert(!std::is_invocable_v<RangeCBeginT, Incomplete const (&&)[10]>);

// This case is IFNDR; we handle it SFINAE-friendly.
static_assert(!std::is_invocable_v<RangeBeginT, Incomplete (&)[]>);
static_assert(!std::is_invocable_v<RangeBeginT, Incomplete const (&)[]>);
static_assert(!std::is_invocable_v<RangeCBeginT, Incomplete (&)[]>);
static_assert(!std::is_invocable_v<RangeCBeginT, Incomplete const (&)[]>);

// This case is IFNDR; we handle it SFINAE-friendly.
static_assert(!std::is_invocable_v<RangeBeginT, Incomplete (&)[10]>);
static_assert(!std::is_invocable_v<RangeBeginT, Incomplete const (&)[10]>);
static_assert(!std::is_invocable_v<RangeCBeginT, Incomplete (&)[10]>);
static_assert(!std::is_invocable_v<RangeCBeginT, Incomplete const (&)[10]>);

struct BeginMember {
    int x;
    constexpr int const* begin() const { return &x; }
};

// Ensure that we can't call with rvalues with borrowing disabled.
static_assert(std::is_invocable_v<RangeBeginT, BeginMember&>);
static_assert(!std::is_invocable_v<RangeBeginT, BeginMember&&>);
static_assert(std::is_invocable_v<RangeBeginT, BeginMember const&>);
static_assert(!std::is_invocable_v<RangeBeginT, BeginMember const&&>);
static_assert(!std::is_invocable_v<RangeCBeginT, BeginMember&>,
    "No longer allowed in C++23, must be input_range");
static_assert(!std::is_invocable_v<RangeCBeginT, BeginMember&&>);
static_assert(!std::is_invocable_v<RangeCBeginT, BeginMember const&>,
    "No longer allowed in C++23, must be input_range");
static_assert(!std::is_invocable_v<RangeCBeginT, BeginMember const&&>);

constexpr bool testReturnTypes() {
    {
        int* x[2];
        ASSERT_SAME_TYPE(decltype(xranges::begin(x)), int**);
        ASSERT_SAME_TYPE(decltype(xranges::cbegin(x)), int* const*);
    }
    {
        int x[2][2];
        ASSERT_SAME_TYPE(decltype(xranges::begin(x)), int(*)[2]);
        ASSERT_SAME_TYPE(decltype(xranges::cbegin(x)), int const(*)[2]);
    }
    {
        struct Different {
            char*& begin();
            short*& begin() const;
            char*& end();
            short*& end() const;
        } x;

        ASSERT_SAME_TYPE(decltype(xranges::begin(x)), char*);
        ASSERT_SAME_TYPE(
            decltype(xranges::cbegin(x)), rxx::basic_const_iterator<short*>);
    }
    return true;
}

constexpr bool testArray() {
    int a[2];
    assert(xranges::begin(a) == a);
    assert(xranges::cbegin(a) == a);

    int b[2][2];
    assert(xranges::begin(b) == b);
    assert(xranges::cbegin(b) == b);

    BeginMember c[2];
    assert(xranges::begin(c) == c);
    assert(xranges::cbegin(c) == c);

    return true;
}

struct BeginMemberReturnsInt {
    int begin() const;
};
static_assert(!std::is_invocable_v<RangeBeginT, BeginMemberReturnsInt const&>);

struct BeginMemberReturnsVoidPtr {
    void const* begin() const;
};
static_assert(
    !std::is_invocable_v<RangeBeginT, BeginMemberReturnsVoidPtr const&>);

struct EmptyBeginMember {
    struct iterator {};
    iterator begin() const;
};
static_assert(!std::is_invocable_v<RangeBeginT, EmptyBeginMember const&>);

struct PtrConvertibleBeginMember {
    struct iterator {
        operator int*() const;
    };
    iterator begin() const;
};
static_assert(
    !std::is_invocable_v<RangeBeginT, PtrConvertibleBeginMember const&>);

struct NonConstBeginMember {
    int x;
    constexpr int* begin() { return &x; }
};
static_assert(std::is_invocable_v<RangeBeginT, NonConstBeginMember&>);
static_assert(!std::is_invocable_v<RangeBeginT, NonConstBeginMember const&>);
static_assert(!std::is_invocable_v<RangeCBeginT, NonConstBeginMember&>);
static_assert(!std::is_invocable_v<RangeCBeginT, NonConstBeginMember const&>);

struct EnabledBorrowingBeginMember {
    constexpr int* begin() const { return &globalBuff[0]; }
    constexpr int* end() const {
        return globalBuff + std::ranges::size(globalBuff);
    }
};
template <>
inline constexpr bool
    std::ranges::enable_borrowed_range<EnabledBorrowingBeginMember> = true;

struct BeginMemberFunction {
    int x;
    constexpr int const* begin() const { return &x; }
    constexpr int const* end() const { return begin() + 1; }
    friend int* begin(BeginMemberFunction const&);
};

struct EmptyPtrBeginMember {
    struct Empty {};
    Empty x;
    constexpr Empty const* begin() const { return &x; }
    constexpr Empty const* end() const { return &x + 1; }
};

constexpr bool testBeginMember() {
    BeginMember a;
    assert(xranges::begin(a) == &a.x);
    // assert(xranges::cbegin(a) == &a.x);
    static_assert(!std::is_invocable_v<RangeBeginT, BeginMember&&>);
    static_assert(!std::is_invocable_v<RangeCBeginT, BeginMember&&>);

    NonConstBeginMember b;
    assert(xranges::begin(b) == &b.x);
    static_assert(!std::is_invocable_v<RangeCBeginT, NonConstBeginMember&>);

    EnabledBorrowingBeginMember c;
    assert(xranges::begin(c) == &globalBuff[0]);
    // assert(xranges::cbegin(c) == &globalBuff[0]);
    assert(xranges::begin(std::move(c)) == &globalBuff[0]);
    assert(xranges::cbegin(std::move(c)) == &globalBuff[0]);

    BeginMemberFunction d;
    assert(xranges::begin(d) == &d.x);
    assert(xranges::cbegin(d) == &d.x);

    EmptyPtrBeginMember e;
    assert(xranges::begin(e) == &e.x);
    assert(xranges::cbegin(e) == &e.x);

    return true;
}

struct BeginFunction {
    int x;
    friend constexpr int const* begin(BeginFunction const& bf) { return &bf.x; }
    friend constexpr int const* end(BeginFunction const& bf) {
        return &bf.x + 1;
    }
};
static_assert(std::is_invocable_v<RangeBeginT, BeginFunction const&>);
static_assert(!std::is_invocable_v<RangeBeginT, BeginFunction&&>);
static_assert(std::is_invocable_v<RangeBeginT,
    BeginFunction&>); // Ill-formed before P2602R2 Poison Pills are Too Toxic
static_assert(std::is_invocable_v<RangeCBeginT, BeginFunction const&>);
static_assert(std::is_invocable_v<RangeCBeginT, BeginFunction&>);

struct BeginFunctionReturnsInt {
    friend int begin(BeginFunctionReturnsInt const&);
};
static_assert(
    !std::is_invocable_v<RangeBeginT, BeginFunctionReturnsInt const&>);

struct BeginFunctionReturnsVoidPtr {
    friend void* begin(BeginFunctionReturnsVoidPtr const&);
};
static_assert(
    !std::is_invocable_v<RangeBeginT, BeginFunctionReturnsVoidPtr const&>);

struct BeginFunctionReturnsPtrConvertible {
    struct iterator {
        operator int*() const;
    };
    friend iterator begin(BeginFunctionReturnsPtrConvertible const&);
};
static_assert(!std::is_invocable_v<RangeBeginT,
              BeginFunctionReturnsPtrConvertible const&>);

struct BeginFunctionByValue {
    friend constexpr int* begin(BeginFunctionByValue) { return globalBuff + 1; }
    friend constexpr int* end(BeginFunctionByValue) { return globalBuff + 2; }
};
static_assert(!std::is_invocable_v<RangeCBeginT, BeginFunctionByValue>);

struct BeginFunctionEnabledBorrowing {
    friend constexpr int* begin(BeginFunctionEnabledBorrowing) {
        return globalBuff + 2;
    }

    friend constexpr int* end(BeginFunctionEnabledBorrowing) {
        return globalBuff + 3;
    }
};
template <>
inline constexpr bool
    std::ranges::enable_borrowed_range<BeginFunctionEnabledBorrowing> = true;

struct BeginFunctionReturnsEmptyPtr {
    struct Empty {};
    Empty x;
    friend constexpr Empty const* begin(
        BeginFunctionReturnsEmptyPtr const& bf) {
        return &bf.x;
    }

    friend constexpr Empty const* end(BeginFunctionReturnsEmptyPtr const& bf) {
        return &bf.x + 1;
    }
};

struct BeginFunctionWithDataMember {
    int x;
    int begin;
    friend constexpr int const* begin(BeginFunctionWithDataMember const& bf) {
        return &bf.x;
    }

    friend constexpr int const* end(BeginFunctionWithDataMember const& bf) {
        return &bf.x + 1;
    }
};

struct BeginFunctionWithPrivateBeginMember {
    int y;
    friend constexpr int const* begin(
        BeginFunctionWithPrivateBeginMember const& bf) {
        return &bf.y;
    }

    friend constexpr int const* end(
        BeginFunctionWithPrivateBeginMember const& bf) {
        return &bf.y + 1;
    }

private:
    int const* begin() const;
};

constexpr bool testBeginFunction() {
    BeginFunction a{};
    BeginFunction const aa{};
    assert(xranges::begin(a) ==
        &a.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::cbegin(a) == &a.x);
    assert(xranges::begin(aa) == &aa.x);
    assert(xranges::cbegin(aa) == &aa.x);

    BeginFunctionByValue b{};
    BeginFunctionByValue const bb{};
    assert(xranges::begin(b) == &globalBuff[1]);
    assert(xranges::cbegin(b) == &globalBuff[1]);
    assert(xranges::begin(bb) == &globalBuff[1]);
    assert(xranges::cbegin(bb) == &globalBuff[1]);

    BeginFunctionEnabledBorrowing c{};
    BeginFunctionEnabledBorrowing const cc{};
    assert(xranges::begin(std::move(c)) == &globalBuff[2]);
    assert(xranges::cbegin(std::move(c)) == &globalBuff[2]);
    assert(xranges::begin(std::move(cc)) == &globalBuff[2]);
    assert(xranges::cbegin(std::move(cc)) == &globalBuff[2]);

    BeginFunctionReturnsEmptyPtr d{};
    BeginFunctionReturnsEmptyPtr const dd{};
    assert(xranges::begin(d) ==
        &d.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::cbegin(d) == &d.x);
    assert(xranges::begin(dd) == &dd.x);
    assert(xranges::cbegin(dd) == &dd.x);

    BeginFunctionWithDataMember e{};
    BeginFunctionWithDataMember const ee{};
    assert(xranges::begin(e) ==
        &e.x); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::begin(ee) == &ee.x);
    assert(xranges::cbegin(e) == &e.x);
    assert(xranges::cbegin(ee) == &ee.x);

    BeginFunctionWithPrivateBeginMember f{};
    BeginFunctionWithPrivateBeginMember const ff{};
    assert(xranges::begin(f) ==
        &f.y); // Ill-formed before P2602R2 Poison Pills are Too Toxic
    assert(xranges::cbegin(f) == &f.y);
    assert(xranges::begin(ff) == &ff.y);
    assert(xranges::cbegin(ff) == &ff.y);

    return true;
}

ASSERT_NOEXCEPT(xranges::begin(std::declval<int (&)[10]>()));
ASSERT_NOEXCEPT(xranges::cbegin(std::declval<int (&)[10]>()));

struct NoThrowMemberBegin {
    ThrowingIterator<int>
    begin() const noexcept; // auto(t.begin()) doesn't throw

    ThrowingIterator<int> end() const noexcept; // auto(t.begin()) doesn't throw
} ntmb;
static_assert(noexcept(xranges::begin(ntmb)));
static_assert(noexcept(xranges::cbegin(ntmb)));

struct NoThrowADLBegin {
    friend ThrowingIterator<int> begin(
        NoThrowADLBegin&) noexcept; // auto(begin(t)) doesn't throw
    friend ThrowingIterator<int> begin(NoThrowADLBegin const&) noexcept;
    friend ThrowingIterator<int> end(NoThrowADLBegin&) noexcept;
    friend ThrowingIterator<int> end(NoThrowADLBegin const&) noexcept;
} ntab;
static_assert(noexcept(xranges::begin(ntab)));
static_assert(noexcept(xranges::cbegin(ntab)));

struct NoThrowMemberBeginReturnsRef {
    ThrowingIterator<int>& begin() const noexcept; // auto(t.begin()) may throw
    ThrowingIterator<int>& end() const noexcept;   // auto(t.end()) may throw
} ntmbrr;
static_assert(!noexcept(xranges::begin(ntmbrr)));
static_assert(!noexcept(xranges::cbegin(ntmbrr)));

struct BeginReturnsArrayRef {
    auto begin() const noexcept -> int (&)[10];

    auto end() const noexcept -> int*;
} brar;
static_assert(noexcept(xranges::begin(brar)));
static_assert(noexcept(xranges::cbegin(brar)));

// Test ADL-proofing.
struct Incomplete;
template <class T>
struct Holder {
    T t;
};
static_assert(!std::is_invocable_v<RangeBeginT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeBeginT, Holder<Incomplete>*&>);
static_assert(!std::is_invocable_v<RangeCBeginT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeCBeginT, Holder<Incomplete>*&>);

int main(int, char**) {
    static_assert(testReturnTypes());

    testArray();
    static_assert(testArray());

    testBeginMember();
    static_assert(testBeginMember());

    testBeginFunction();
    static_assert(testBeginFunction());

    return 0;
}
