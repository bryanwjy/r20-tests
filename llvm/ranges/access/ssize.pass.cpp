// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// xranges::ssize

#include "../../static_asserts.h"
#include "../../test_iterators.h"
#include "rxx/ranges/access.h"

#include <cassert>

using RangeSSizeT = decltype(xranges::ssize);

static_assert(!std::is_invocable_v<RangeSSizeT, int[]>);
static_assert(std::is_invocable_v<RangeSSizeT, int[1]>);
static_assert(std::is_invocable_v<RangeSSizeT, int (&&)[1]>);
static_assert(std::is_invocable_v<RangeSSizeT, int (&)[1]>);

struct SizeMember {
    constexpr std::size_t size() { return 42; }
};
static_assert(
    !std::is_invocable_v<decltype(xranges::ssize), SizeMember const&>);

struct SizeFunction {
    friend constexpr std::size_t size(SizeFunction) { return 42; }
};

struct SizeFunctionSigned {
    friend constexpr std::ptrdiff_t size(SizeFunctionSigned) { return 42; }
};

struct SizedSentinelRange {
    int data_[2] = {};
    constexpr int* begin() { return data_; }
    constexpr auto end() { return sized_sentinel<int*>(data_ + 2); }
};

struct ShortUnsignedReturnType {
    constexpr unsigned short size() { return 42; }
};

// size_t changes depending on the platform.
using SignedSizeT = std::make_signed_t<std::size_t>;

constexpr bool test() {
    int a[4];

    assert(xranges::ssize(a) == 4);
    ASSERT_SAME_TYPE(decltype(xranges::ssize(a)), SignedSizeT);

    assert(xranges::ssize(SizeMember()) == 42);
    ASSERT_SAME_TYPE(decltype(xranges::ssize(SizeMember())), SignedSizeT);

    assert(xranges::ssize(SizeFunction()) == 42);
    ASSERT_SAME_TYPE(decltype(xranges::ssize(SizeFunction())), SignedSizeT);

    assert(xranges::ssize(SizeFunctionSigned()) == 42);
    ASSERT_SAME_TYPE(
        decltype(xranges::ssize(SizeFunctionSigned())), std::ptrdiff_t);

    SizedSentinelRange b;
    assert(xranges::ssize(b) == 2);
    ASSERT_SAME_TYPE(decltype(xranges::ssize(b)), std::ptrdiff_t);

    // This gets converted to ptrdiff_t because it's wider.
    ShortUnsignedReturnType c;
    assert(xranges::ssize(c) == 42);
    ASSERT_SAME_TYPE(decltype(xranges::ssize(c)), std::ptrdiff_t);

    return true;
}

// Test ADL-proofing.
struct Incomplete;
template <class T>
struct Holder {
    T t;
};
static_assert(!std::is_invocable_v<RangeSSizeT, Holder<Incomplete>*>);
static_assert(!std::is_invocable_v<RangeSSizeT, Holder<Incomplete>*&>);

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
