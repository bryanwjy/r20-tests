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

// rxx::ranges::data

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "rxx/access.h"

#include <cassert>
#include <ranges>
#include <type_traits>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

using RangeDataT = decltype(xranges::data);
using RangeCDataT = decltype(xranges::cdata);

static int globalBuff[2];

struct Incomplete;

static_assert(!std::is_invocable_v<RangeDataT, Incomplete[]>);
static_assert(!std::is_invocable_v<RangeDataT, Incomplete (&&)[2]>);
static_assert(!std::is_invocable_v<RangeDataT, Incomplete (&&)[2][2]>);
static_assert(!std::is_invocable_v<RangeDataT, int[1]>);
static_assert(!std::is_invocable_v<RangeDataT, int (&&)[1]>);
static_assert(std::is_invocable_v<RangeDataT, int (&)[1]>);

static_assert(!std::is_invocable_v<RangeCDataT, Incomplete[]>);
static_assert(!std::is_invocable_v<RangeCDataT, Incomplete (&&)[2]>);
static_assert(!std::is_invocable_v<RangeCDataT, Incomplete (&&)[2][2]>);
static_assert(!std::is_invocable_v<RangeCDataT, int[1]>);
static_assert(!std::is_invocable_v<RangeCDataT, int (&&)[1]>);
static_assert(std::is_invocable_v<RangeCDataT, int (&)[1]>);

struct DataMember {
    int x;
    constexpr int const* data() const { return &x; }
    constexpr int const* begin() const { return &x; }
    constexpr int const* end() const { return &x + 1; }
};
static_assert(std::is_invocable_v<RangeDataT, DataMember&>);
static_assert(!std::is_invocable_v<RangeDataT, DataMember&&>);
static_assert(std::is_invocable_v<RangeDataT, DataMember const&>);
static_assert(!std::is_invocable_v<RangeDataT, DataMember const&&>);
static_assert(std::is_invocable_v<RangeCDataT, DataMember&>);
static_assert(!std::is_invocable_v<RangeCDataT, DataMember&&>);
static_assert(std::is_invocable_v<RangeCDataT, DataMember const&>);
static_assert(!std::is_invocable_v<RangeCDataT, DataMember const&&>);

constexpr bool testReturnTypes() {
    {
        int* x[2];
        ASSERT_SAME_TYPE(decltype(xranges::data(x)), int**);
        ASSERT_SAME_TYPE(decltype(xranges::cdata(x)), int* const*);
    }
    {
        int x[2][2];
        ASSERT_SAME_TYPE(decltype(xranges::data(x)), int(*)[2]);
        ASSERT_SAME_TYPE(decltype(xranges::cdata(x)), int const(*)[2]);
    }
    RXX_DISABLE_WARNING_PUSH()
#if RXX_COMPILER_CLANG | RXX_COMPILER_GCC
    RXX_DISABLE_WARNING("-Wundefined-inline")
#endif
    {
        struct D {
            char*& data();
            short*& data() const;
            constexpr char* begin();
            constexpr char* end();
            constexpr short const* begin() const;
            constexpr short const* end() const;
        };
        ASSERT_SAME_TYPE(
            decltype(xranges::data(std::declval<D const&>())), short*);
        static_assert(!std::is_invocable_v<RangeDataT, D const&&>);
        ASSERT_SAME_TYPE(
            decltype(xranges::cdata(std::declval<D&>())), char const*);
        static_assert(!std::is_invocable_v<RangeCDataT, D&&>);
        ASSERT_SAME_TYPE(
            decltype(xranges::cdata(std::declval<D const&>())), short const*);
        static_assert(!std::is_invocable_v<RangeCDataT, D const&&>);
    }
    {
        struct NC {
            char* begin() const;
            char* end() const;
            int* data();
        };
        static_assert(!xranges::contiguous_range<NC>);
        static_assert(xranges::contiguous_range<const NC>);
        ASSERT_SAME_TYPE(decltype(xranges::data(std::declval<NC&>())), int*);
        static_assert(!std::is_invocable_v<RangeDataT, NC&&>);
        ASSERT_SAME_TYPE(
            decltype(xranges::data(std::declval<const NC&>())), char*);
        static_assert(!std::is_invocable_v<RangeDataT, const NC&&>);
        ASSERT_SAME_TYPE(
            decltype(xranges::cdata(std::declval<NC&>())), char const*);
        static_assert(!std::is_invocable_v<RangeCDataT, NC&&>);
        ASSERT_SAME_TYPE(
            decltype(xranges::cdata(std::declval<const NC&>())), char const*);
        static_assert(!std::is_invocable_v<RangeCDataT, const NC&&>);
    }
    RXX_DISABLE_WARNING_POP()
    return true;
}

struct VoidDataMember {
    void* data() const;
};
static_assert(!std::is_invocable_v<RangeDataT, VoidDataMember const&>);
static_assert(!std::is_invocable_v<RangeCDataT, VoidDataMember const&>);

struct Empty {};
struct EmptyDataMember {
    Empty data() const;
};
static_assert(!std::is_invocable_v<RangeDataT, EmptyDataMember const&>);
static_assert(!std::is_invocable_v<RangeCDataT, EmptyDataMember const&>);

struct PtrConvertibleDataMember {
    struct Ptr {
        operator int*() const;
    };
    Ptr data() const;
};
static_assert(
    !std::is_invocable_v<RangeDataT, PtrConvertibleDataMember const&>);
static_assert(
    !std::is_invocable_v<RangeCDataT, PtrConvertibleDataMember const&>);

struct NonConstDataMember {
    int x;
    constexpr int* data() { return &x; }
};

struct EnabledBorrowingDataMember {
    constexpr int* data() { return &globalBuff[0]; }
};
template <>
inline constexpr bool
    std::ranges::enable_borrowed_range<EnabledBorrowingDataMember> = true;

struct DataMemberAndBegin {
    int x;
    constexpr int const* data() const { return &x; }
    int const* begin() const;
    int const* end() const;
};

constexpr bool testDataMember() {
    DataMember a;
    assert(xranges::data(a) == &a.x);
    assert(xranges::cdata(a) == &a.x);

    NonConstDataMember b;
    assert(xranges::data(b) == &b.x);
    static_assert(!std::is_invocable_v<RangeCDataT, decltype((b))>);

    EnabledBorrowingDataMember c;
    assert(xranges::data(std::move(c)) == &globalBuff[0]);
    static_assert(!std::is_invocable_v<RangeCDataT, decltype(std::move(c))>);

    DataMemberAndBegin d;
    assert(xranges::data(d) == &d.x);
    assert(xranges::cdata(d) == &d.x);

    return true;
}

using ContiguousIter = contiguous_iterator<int const*>;

struct BeginMemberContiguousIterator {
    int buff[8];

    constexpr ContiguousIter begin() const { return ContiguousIter(buff); }
    constexpr ContiguousIter end() const { return ContiguousIter(buff + 8); }
};
static_assert(std::is_invocable_v<RangeDataT, BeginMemberContiguousIterator&>);
static_assert(
    !std::is_invocable_v<RangeDataT, BeginMemberContiguousIterator&&>);
static_assert(
    std::is_invocable_v<RangeDataT, BeginMemberContiguousIterator const&>);
static_assert(
    !std::is_invocable_v<RangeDataT, BeginMemberContiguousIterator const&&>);
static_assert(std::is_invocable_v<RangeCDataT, BeginMemberContiguousIterator&>);
static_assert(
    !std::is_invocable_v<RangeCDataT, BeginMemberContiguousIterator&&>);
static_assert(
    std::is_invocable_v<RangeCDataT, BeginMemberContiguousIterator const&>);
static_assert(
    !std::is_invocable_v<RangeCDataT, BeginMemberContiguousIterator const&&>);

struct BeginMemberRandomAccess {
    int buff[8];

    random_access_iterator<int const*> begin() const;
};
static_assert(!std::is_invocable_v<RangeDataT, BeginMemberRandomAccess&>);
static_assert(!std::is_invocable_v<RangeDataT, BeginMemberRandomAccess&&>);
static_assert(!std::is_invocable_v<RangeDataT, BeginMemberRandomAccess const&>);
static_assert(
    !std::is_invocable_v<RangeDataT, BeginMemberRandomAccess const&&>);
static_assert(!std::is_invocable_v<RangeCDataT, BeginMemberRandomAccess&>);
static_assert(!std::is_invocable_v<RangeCDataT, BeginMemberRandomAccess&&>);
static_assert(
    !std::is_invocable_v<RangeCDataT, BeginMemberRandomAccess const&>);
static_assert(
    !std::is_invocable_v<RangeCDataT, BeginMemberRandomAccess const&&>);

struct BeginFriendContiguousIterator {
    int buff[8];

    friend constexpr ContiguousIter begin(
        BeginFriendContiguousIterator const& iter) {
        return ContiguousIter(iter.buff);
    }

    friend constexpr ContiguousIter end(
        BeginFriendContiguousIterator const& iter) {
        return ContiguousIter(iter.buff + 8);
    }
};
static_assert(std::is_invocable_v<RangeDataT, BeginMemberContiguousIterator&>);
static_assert(
    !std::is_invocable_v<RangeDataT, BeginMemberContiguousIterator&&>);
static_assert(
    std::is_invocable_v<RangeDataT, BeginMemberContiguousIterator const&>);
static_assert(
    !std::is_invocable_v<RangeDataT, BeginMemberContiguousIterator const&&>);
static_assert(std::is_invocable_v<RangeCDataT, BeginMemberContiguousIterator&>);
static_assert(
    !std::is_invocable_v<RangeCDataT, BeginMemberContiguousIterator&&>);
static_assert(
    std::is_invocable_v<RangeCDataT, BeginMemberContiguousIterator const&>);
static_assert(
    !std::is_invocable_v<RangeCDataT, BeginMemberContiguousIterator const&&>);

struct BeginFriendRandomAccess {
    friend random_access_iterator<int const*> begin(
        BeginFriendRandomAccess const iter);
};
static_assert(!std::is_invocable_v<RangeDataT, BeginFriendRandomAccess&>);
static_assert(!std::is_invocable_v<RangeDataT, BeginFriendRandomAccess&&>);
static_assert(!std::is_invocable_v<RangeDataT, BeginFriendRandomAccess const&>);
static_assert(
    !std::is_invocable_v<RangeDataT, BeginFriendRandomAccess const&&>);
static_assert(!std::is_invocable_v<RangeCDataT, BeginFriendRandomAccess&>);
static_assert(!std::is_invocable_v<RangeCDataT, BeginFriendRandomAccess&&>);
static_assert(
    !std::is_invocable_v<RangeCDataT, BeginFriendRandomAccess const&>);
static_assert(
    !std::is_invocable_v<RangeCDataT, BeginFriendRandomAccess const&&>);

struct BeginMemberRvalue {
    int buff[8];

    ContiguousIter begin() &&;
};
static_assert(!std::is_invocable_v<RangeDataT, BeginMemberRvalue&>);
static_assert(!std::is_invocable_v<RangeDataT, BeginMemberRvalue&&>);
static_assert(!std::is_invocable_v<RangeDataT, BeginMemberRvalue const&>);
static_assert(!std::is_invocable_v<RangeDataT, BeginMemberRvalue const&&>);
static_assert(!std::is_invocable_v<RangeCDataT, BeginMemberRvalue&>);
static_assert(!std::is_invocable_v<RangeCDataT, BeginMemberRvalue&&>);
static_assert(!std::is_invocable_v<RangeCDataT, BeginMemberRvalue const&>);
static_assert(!std::is_invocable_v<RangeCDataT, BeginMemberRvalue const&&>);

struct BeginMemberBorrowingEnabled {
    constexpr contiguous_iterator<int*> begin() {
        return contiguous_iterator<int*>{&globalBuff[1]};
    }
};
template <>
inline constexpr bool
    std::ranges::enable_borrowed_range<BeginMemberBorrowingEnabled> = true;
static_assert(std::is_invocable_v<RangeDataT, BeginMemberBorrowingEnabled&>);
static_assert(std::is_invocable_v<RangeDataT, BeginMemberBorrowingEnabled&&>);
static_assert(
    !std::is_invocable_v<RangeDataT, BeginMemberBorrowingEnabled const&>);
static_assert(
    !std::is_invocable_v<RangeDataT, BeginMemberBorrowingEnabled const&&>);
static_assert(!std::is_invocable_v<RangeCDataT, BeginMemberBorrowingEnabled&>);
static_assert(!std::is_invocable_v<RangeCDataT, BeginMemberBorrowingEnabled&&>);
static_assert(
    !std::is_invocable_v<RangeCDataT, BeginMemberBorrowingEnabled const&>);
static_assert(
    !std::is_invocable_v<RangeCDataT, BeginMemberBorrowingEnabled const&&>);

constexpr bool testViaRangesBegin() {
    int arr[2];
    assert(xranges::data(arr) == arr + 0);
    assert(xranges::cdata(arr) == arr + 0);

    BeginMemberContiguousIterator a;
    assert(xranges::data(a) == a.buff);
    assert(xranges::cdata(a) == a.buff);

    BeginFriendContiguousIterator const b{};
    assert(xranges::data(b) == b.buff);
    assert(xranges::cdata(b) == b.buff);

    BeginMemberBorrowingEnabled c;
    assert(xranges::data(std::move(c)) == &globalBuff[1]);
    static_assert(!std::is_invocable_v<RangeCDataT, decltype(std::move(c))>);

    return true;
}

// Test ADL-proofing.
struct Incomplete;
template <class T>
struct Holder {
    T t;
};
static_assert(!std::is_invocable_v<RangeDataT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeDataT, Holder<Incomplete>*&>);
static_assert(!std::is_invocable_v<RangeCDataT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeCDataT, Holder<Incomplete>*&>);

struct RandomButNotContiguous {
    random_access_iterator<int*> begin() const;
    random_access_iterator<int*> end() const;
};
static_assert(!std::is_invocable_v<RangeDataT, RandomButNotContiguous>);
static_assert(!std::is_invocable_v<RangeDataT, RandomButNotContiguous&>);
static_assert(!std::is_invocable_v<RangeCDataT, RandomButNotContiguous>);
static_assert(!std::is_invocable_v<RangeCDataT, RandomButNotContiguous&>);

int main(int, char**) {
    static_assert(testReturnTypes());

    testDataMember();
    static_assert(testDataMember());

    testViaRangesBegin();
    static_assert(testViaRangesBegin());

    return 0;
}
