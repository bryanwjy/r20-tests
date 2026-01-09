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

// ~optional();

#include "rxx/optional.h"

#include <cassert>
#include <type_traits>

using __RXX optional;

struct PODType {
    int value;
    int value2;
};

class X {
public:
    static bool dtor_called;
    X() = default;
    X(X const&) = default;
    X& operator=(X const&) = default;
    ~X() { dtor_called = true; }
};

bool X::dtor_called = false;

int main(int, char**) {
    {
        typedef int T;
        static_assert(std::is_trivially_destructible<T>::value, "");
        static_assert(std::is_trivially_destructible<optional<T>>::value, "");
    }
    {
        typedef double T;
        static_assert(std::is_trivially_destructible<T>::value, "");
        static_assert(std::is_trivially_destructible<optional<T>>::value, "");
    }
    {
        typedef PODType T;
        static_assert(std::is_trivially_destructible<T>::value, "");
        static_assert(std::is_trivially_destructible<optional<T>>::value, "");
    }
    {
        typedef X T;
        static_assert(!std::is_trivially_destructible<T>::value, "");
        static_assert(!std::is_trivially_destructible<optional<T>>::value, "");
        {
            X x;
            optional<X> opt{x};
            assert(X::dtor_called == false);
        }
        assert(X::dtor_called == true);
    }
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    {
        typedef X& T;
        static_assert(std::is_trivially_destructible_v<T>);
        static_assert(std::is_trivially_destructible_v<optional<T>>);
    }
    X::dtor_called = false;
    X x;
    {
        optional<X&> opt{x};
        assert(X::dtor_called == false);
    }
    assert(X::dtor_called == false);

    {
        static_assert(std::is_trivially_destructible_v<X (&)()>);
        static_assert(std::is_trivially_destructible_v<optional<X (&)()>>);
    }
#endif
    return 0;
}
