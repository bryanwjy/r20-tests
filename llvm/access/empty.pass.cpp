//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// xranges::empty

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "rxx/access.h"

#include <cassert>
#include <ranges>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

using RangeEmptyT = decltype(xranges::empty);

static_assert(!std::is_invocable_v<RangeEmptyT, int[]>);
static_assert(!std::is_invocable_v<RangeEmptyT, int (&)[]>);
static_assert(!std::is_invocable_v<RangeEmptyT, int (&&)[]>);
static_assert(std::is_invocable_v<RangeEmptyT, int[1]>);
static_assert(std::is_invocable_v<RangeEmptyT, int const[1]>);
static_assert(std::is_invocable_v<RangeEmptyT, int (&&)[1]>);
static_assert(std::is_invocable_v<RangeEmptyT, int (&)[1]>);
static_assert(std::is_invocable_v<RangeEmptyT, int const (&)[1]>);

struct Incomplete;
static_assert(!std::is_invocable_v<RangeEmptyT, Incomplete[]>);
static_assert(!std::is_invocable_v<RangeEmptyT, Incomplete (&)[]>);
static_assert(!std::is_invocable_v<RangeEmptyT, Incomplete (&&)[]>);
static_assert(!std::is_invocable_v<RangeEmptyT, Incomplete (&)[1]>);
static_assert(!std::is_invocable_v<RangeEmptyT, Incomplete (&&)[1]>);
static_assert(!std::is_invocable_v<RangeEmptyT, Incomplete const (&)[1]>);
static_assert(!std::is_invocable_v<RangeEmptyT, Incomplete const (&&)[1]>);

struct InputRangeWithoutSize {
    cpp17_input_iterator<int*> begin() const;
    cpp17_input_iterator<int*> end() const;
};
static_assert(!std::is_invocable_v<RangeEmptyT, InputRangeWithoutSize const&>);

struct NonConstEmpty {
    bool empty();
};
static_assert(!std::is_invocable_v<RangeEmptyT, NonConstEmpty const&>);

struct HasMemberAndFunction {
    constexpr bool empty() const { return true; }
    // We should never do ADL lookup for xranges::empty.
    friend bool empty(HasMemberAndFunction const&) { return false; }
};

struct BadReturnType {
    BadReturnType empty() { return {}; }
};
static_assert(!std::is_invocable_v<RangeEmptyT, BadReturnType&>);

struct BoolConvertible {
    constexpr explicit operator bool() noexcept(false) { return true; }
};
struct BoolConvertibleReturnType {
    constexpr BoolConvertible empty() noexcept { return {}; }
};
static_assert(!noexcept(xranges::empty(BoolConvertibleReturnType())));

struct InputIterators {
    cpp17_input_iterator<int*> begin() const;
    cpp17_input_iterator<int*> end() const;
};
static_assert(
    std::is_same_v<decltype(InputIterators().begin() == InputIterators().end()),
        bool>);
static_assert(!std::is_invocable_v<RangeEmptyT, InputIterators const&>);

constexpr bool testEmptyMember() {
    HasMemberAndFunction a;
    assert(xranges::empty(a));

    BoolConvertibleReturnType b;
    assert(xranges::empty(b));

    return true;
}

struct SizeMember {
    std::size_t size_;
    constexpr std::size_t size() const { return size_; }
};

struct SizeFunction {
    std::size_t size_;
    friend constexpr std::size_t size(SizeFunction sf) { return sf.size_; }
};

struct BeginEndSizedSentinel {
    constexpr int* begin() const { return nullptr; }
    constexpr auto end() const { return sized_sentinel<int*>(nullptr); }
};
static_assert(xranges::forward_range<BeginEndSizedSentinel>);
static_assert(xranges::sized_range<BeginEndSizedSentinel>);

constexpr bool testUsingRangesSize() {
    SizeMember a{1};
    assert(!xranges::empty(a));
    SizeMember b{0};
    assert(xranges::empty(b));

    SizeFunction c{1};
    assert(!xranges::empty(c));
    SizeFunction d{0};
    assert(xranges::empty(d));

    BeginEndSizedSentinel e;
    assert(xranges::empty(e));

    return true;
}

struct BeginEndNotSizedSentinel {
    constexpr int* begin() const { return nullptr; }
    constexpr auto end() const { return sentinel_wrapper<int*>(nullptr); }
};
static_assert(xranges::forward_range<BeginEndNotSizedSentinel>);
static_assert(!xranges::sized_range<BeginEndNotSizedSentinel>);

// size is disabled here, so we have to compare begin and end.
struct DisabledSizeRangeWithBeginEnd {
    constexpr int* begin() const { return nullptr; }
    constexpr auto end() const { return sentinel_wrapper<int*>(nullptr); }
    std::size_t size() const;
};
template <>
inline constexpr bool
    std::ranges::disable_sized_range<DisabledSizeRangeWithBeginEnd> = true;
static_assert(xranges::contiguous_range<DisabledSizeRangeWithBeginEnd>);
static_assert(!xranges::sized_range<DisabledSizeRangeWithBeginEnd>);

struct BeginEndAndEmpty {
    constexpr int* begin() const { return nullptr; }
    constexpr auto end() const { return sentinel_wrapper<int*>(nullptr); }
    constexpr bool empty() { return false; }
};

struct EvilBeginEnd {
    bool empty() &&;
    constexpr int* begin() & { return nullptr; }
    constexpr int* end() & { return nullptr; }
};

constexpr bool testBeginEqualsEnd() {
    BeginEndNotSizedSentinel a;
    assert(xranges::empty(a));

    DisabledSizeRangeWithBeginEnd d;
    assert(xranges::empty(d));

    BeginEndAndEmpty e;
    assert(!xranges::empty(e));               // e.empty()
    assert(xranges::empty(std::as_const(e))); // e.begin() == e.end()

    assert(xranges::empty(EvilBeginEnd()));

    return true;
}

// Test ADL-proofing.
struct Incomplete;
template <class T>
struct Holder {
    T t;
};
static_assert(!std::is_invocable_v<RangeEmptyT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeEmptyT, Holder<Incomplete>*&>);

int main(int, char**) {
    testEmptyMember();
    static_assert(testEmptyMember());

    testUsingRangesSize();
    static_assert(testUsingRangesSize());

    testBeginEqualsEnd();
    static_assert(testBeginEqualsEnd());

    return 0;
}
