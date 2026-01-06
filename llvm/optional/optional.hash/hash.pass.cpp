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

// template <class T> struct hash<optional<T>>;

#include "../../poisoned_hash_helper.h"
#include "../../static_asserts.h"
#include "rxx/optional.h"

#include <cassert>
#include <memory>
#include <string>

struct A {};
struct B {};

template <>
struct std::hash<B> {
    std::size_t operator()(B const&) noexcept(false) { return 0; }
};

int main(int, char**) {
    using __RXX optional;
    std::size_t const nullopt_hash =
        std::hash<optional<double>>{}(optional<double>{});

    {
        optional<B> opt;
        ASSERT_NOT_NOEXCEPT(std::hash<optional<B>>()(opt));
        ASSERT_NOT_NOEXCEPT(std::hash<optional<B const>>()(opt));
    }

    {
        typedef int T;
        optional<T> opt;
        assert(std::hash<optional<T>>{}(opt) == nullopt_hash);
        opt = 2;
        assert(std::hash<optional<T>>{}(opt) == std::hash<T>{}(*opt));
    }
    {
        typedef std::string T;
        optional<T> opt;
        assert(std::hash<optional<T>>{}(opt) == nullopt_hash);
        opt = std::string("123");
        assert(std::hash<optional<T>>{}(opt) == std::hash<T>{}(*opt));
    }
    {
        typedef std::unique_ptr<int> T;
        optional<T> opt;
        assert(std::hash<optional<T>>{}(opt) == nullopt_hash);
        opt = std::unique_ptr<int>(new int(3));
        assert(std::hash<optional<T>>{}(opt) == std::hash<T>{}(*opt));
    }
    {
        test_hash_enabled<optional<int>>();
        test_hash_enabled<optional<int*>>();
        test_hash_enabled<optional<int const>>();
        test_hash_enabled<optional<int* const>>();

        test_hash_disabled<optional<A>>();
        test_hash_disabled<optional<A const>>();

        test_hash_enabled<optional<B>>();
        test_hash_enabled<optional<B const>>();
    }

    return 0;
}
