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

// xviews::lazy_split

#include "../../test_iterators.h"
#include "../../test_range.h"
#include "rxx/ranges.h"
#include "types.h"

#include <array>
#include <cassert>
#include <concepts>
#include <string_view>
#include <utility>

struct SomeView : xranges::view_base {
    std::string_view const* v_;
    constexpr SomeView(std::string_view const& v) : v_(&v) {}
    constexpr auto begin() const { return v_->begin(); }
    constexpr auto end() const { return v_->end(); }
};

struct NotAView {};

static_assert(!std::is_invocable_v<decltype(xviews::lazy_split)>);
static_assert(
    !std::is_invocable_v<decltype(xviews::lazy_split), SomeView, NotAView>);
static_assert(
    !std::is_invocable_v<decltype(xviews::lazy_split), NotAView, SomeView>);
static_assert(
    std::is_invocable_v<decltype(xviews::lazy_split), SomeView, SomeView>);

// Regression test for #75002, views::lazy_split shouldn't be a range adaptor
// closure
static_assert(!CanBePiped<SomeView&, decltype(xviews::lazy_split)>);
static_assert(!CanBePiped<char (&)[10], decltype(xviews::lazy_split)>);
static_assert(!CanBePiped<char (&&)[10], decltype(xviews::lazy_split)>);
static_assert(!CanBePiped<NotAView, decltype(xviews::lazy_split)>);

static_assert(CanBePiped<SomeView&, decltype(xviews::lazy_split('x'))>);
static_assert(CanBePiped<char (&)[10], decltype(xviews::lazy_split('x'))>);
static_assert(!CanBePiped<char (&&)[10], decltype(xviews::lazy_split('x'))>);
static_assert(!CanBePiped<NotAView, decltype(xviews::lazy_split('x'))>);

static_assert(std::same_as<decltype(xviews::lazy_split),
    decltype(xranges::views::lazy_split)>);

constexpr bool test() {
    std::string_view input = "abc";
    std::string_view sep = "a";

    // Test that `xviews::lazy_split` is a range adaptor.

    // Test `views::lazy_split(input, sep)`.
    {
        SomeView view(input);

        using Result = xranges::lazy_split_view<SomeView, std::string_view>;
        std::same_as<Result> decltype(auto) result =
            xviews::lazy_split(view, sep);
        assert(result.base().begin() == input.begin());
        assert(result.base().end() == input.end());
    }

    // Test `views::lazy_split(sep)(input)`.
    {
        SomeView view(input);

        using Result = xranges::lazy_split_view<SomeView, std::string_view>;
        std::same_as<Result> decltype(auto) result =
            xviews::lazy_split(sep)(view);
        assert(result.base().begin() == input.begin());
        assert(result.base().end() == input.end());
    }

    // Test `view | views::lazy_split`.
    {
        SomeView view(input);

        using Result = xranges::lazy_split_view<SomeView, std::string_view>;
        std::same_as<Result> decltype(auto) result =
            view | xviews::lazy_split(sep);
        assert(result.base().begin() == input.begin());
        assert(result.base().end() == input.end());
    }

    // Test `adaptor | views::lazy_split`.
    {
        SomeView view(input);
        auto f = [](char c) { return c; };
        auto partial = xviews::transform(f) | xviews::lazy_split(sep);

        using Result = xranges::lazy_split_view<
            xranges::transform_view<SomeView, decltype(f)>, std::string_view>;
        std::same_as<Result> decltype(auto) result = partial(view);
        assert(result.base().base().begin() == input.begin());
        assert(result.base().base().end() == input.end());
    }

    // Test `views::lazy_split | adaptor`.
    {
        SomeView view(input);
        auto f = [](auto v) { return v; };
        auto partial = xviews::lazy_split(sep) | xviews::transform(f);

        using Result = xranges::transform_view<
            xranges::lazy_split_view<SomeView, std::string_view>, decltype(f)>;
        std::same_as<Result> decltype(auto) result = partial(view);
        assert(result.base().base().begin() == input.begin());
        assert(result.base().base().end() == input.end());
    }

    // Test that one can call `xviews::lazy_split` with arbitrary stuff, as
    // long as we don't try to actually complete the call by passing it a range.
    //
    // That makes no sense and we can't do anything with the result, but it's
    // valid.
    {
        struct X {};
        [[maybe_unused]] auto partial = xviews::lazy_split(X{});
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
