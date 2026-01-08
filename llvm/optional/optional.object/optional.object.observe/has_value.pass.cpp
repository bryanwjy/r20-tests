// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14
// <optional>

// constexpr bool optional<T>::has_value() const noexcept;

#include "../../../static_asserts.h"
#include "rxx/optional.h"

#include <cassert>

int main(int, char**) {
    using __RXX optional;
    {
        optional<int> const opt;
        ((void)opt);
        ASSERT_NOEXCEPT(opt.has_value());
        ASSERT_SAME_TYPE(decltype(opt.has_value()), bool);
    }
    {
        constexpr optional<int> opt;
        static_assert(!opt.has_value(), "");
    }
    {
        constexpr optional<int> opt(0);
        static_assert(opt.has_value(), "");
    }
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    {
        static constexpr int i = 0;
        constexpr optional<int const&> opt{i};
        static_assert(opt.has_value());
    }
#endif

    return 0;
}
