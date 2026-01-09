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

// void swap(optional&)
//     noexcept(is_nothrow_move_constructible<T>::value &&
//              is_nothrow_swappable<T>::value)

#include "rxx/optional.h"

#include <cassert>
#include <memory>

using __RXX optional;

class X {
    int i_;

public:
    static unsigned dtor_called;
    X(int i) : i_(i) {}
    X(X&& x) = default;
    X& operator=(X&&) = default;
    ~X() { ++dtor_called; }

    friend bool operator==(X const& x, X const& y) { return x.i_ == y.i_; }
};

unsigned X::dtor_called = 0;

class Y {
    int i_;

public:
    static unsigned dtor_called;
    Y(int i) : i_(i) {}
    Y(Y&&) = default;
    ~Y() { ++dtor_called; }

    friend constexpr bool operator==(Y const& x, Y const& y) {
        return x.i_ == y.i_;
    }
    friend void swap(Y& x, Y& y) { std::swap(x.i_, y.i_); }
};

unsigned Y::dtor_called = 0;

class Z {
    int i_;

public:
    Z(int i) : i_(i) {}
    Z(Z&&) { RXX_THROW(7); }

    friend constexpr bool operator==(Z const& x, Z const& y) {
        return x.i_ == y.i_;
    }
    friend void swap(Z&, Z&) { RXX_THROW(6); }
};

class W {
    int i_;

public:
    constexpr W(int i) : i_(i) {}

    friend constexpr bool operator==(W const& x, W const& y) {
        return x.i_ == y.i_;
    }
    friend constexpr void swap(W& x, W& y) noexcept { std::swap(x.i_, y.i_); }
};

template <class T>
constexpr bool check_swap() {
    {
        optional<T> opt1;
        optional<T> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<T> opt1(1);
        optional<T> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<T> opt1;
        optional<T> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<T> opt1(1);
        optional<T> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    return true;
}

#if RXX_SUPPORTS_OPTIONAL_REFERENCES
template <typename T>
constexpr bool check_swap_ref() {
    {
        optional<T&> opt1;
        optional<T&> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
    }

    {
        T one{1};
        optional<T&> opt1(one);
        optional<T&> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true);
        assert(static_cast<bool>(opt1) == true);
        assert(std::addressof(*opt1) == std::addressof(one));
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(std::addressof(*opt2) == std::addressof(one));
    }

    {
        T two{2};
        optional<T&> opt1;
        optional<T&> opt2(two);
        static_assert(noexcept(opt1.swap(opt2)) == true);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(std::addressof(*opt2) == std::addressof(two));
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == true);
        assert(std::addressof(*opt1) == std::addressof(two));
        assert(static_cast<bool>(opt2) == false);
    }

    {
        T one{1};
        T two{2};

        optional<T&> opt1(one);
        optional<T&> opt2(two);
        static_assert(noexcept(opt1.swap(opt2)) == true);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(std::addressof(*opt1) == std::addressof(one));
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        assert(std::addressof(*opt2) == std::addressof(two));
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(std::addressof(*opt1) == std::addressof(two));
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
        assert(std::addressof(*opt2) == std::addressof(one));
    }

    return true;
}
#endif

int main(int, char**) {
    check_swap<int>();
    check_swap<W>();
    static_assert(check_swap<int>());
    static_assert(check_swap<W>());
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    static_assert(check_swap_ref<int>());
    static_assert(check_swap_ref<W>());
    check_swap_ref<int>();
    check_swap_ref<W>();
#endif
    {
        optional<X> opt1;
        optional<X> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        assert(X::dtor_called == 0);
    }
    {
        optional<X> opt1(1);
        optional<X> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        X::dtor_called = 0;
        opt1.swap(opt2);
        assert(X::dtor_called == 1);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<X> opt1;
        optional<X> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        X::dtor_called = 0;
        opt1.swap(opt2);
        assert(X::dtor_called == 1);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<X> opt1(1);
        optional<X> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        X::dtor_called = 0;
        opt1.swap(opt2);
        assert(X::dtor_called == 1); // from inside std::swap
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<Y> opt1;
        optional<Y> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        assert(Y::dtor_called == 0);
    }
    {
        optional<Y> opt1(1);
        optional<Y> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        Y::dtor_called = 0;
        opt1.swap(opt2);
        assert(Y::dtor_called == 1);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<Y> opt1;
        optional<Y> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        Y::dtor_called = 0;
        opt1.swap(opt2);
        assert(Y::dtor_called == 1);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<Y> opt1(1);
        optional<Y> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        Y::dtor_called = 0;
        opt1.swap(opt2);
        assert(Y::dtor_called == 0);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<Z> opt1;
        optional<Z> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
    }
#if RXX_WITH_EXCEPTIONS
    {
        optional<Z> opt1;
        opt1.emplace(1);
        optional<Z> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        try {
            opt1.swap(opt2);
            assert(false);
        } catch (int i) {
            assert(i == 7);
        }
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<Z> opt1;
        optional<Z> opt2;
        opt2.emplace(2);
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        try {
            opt1.swap(opt2);
            assert(false);
        } catch (int i) {
            assert(i == 7);
        }
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
    }
    {
        optional<Z> opt1;
        opt1.emplace(1);
        optional<Z> opt2;
        opt2.emplace(2);
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        try {
            opt1.swap(opt2);
            assert(false);
        } catch (int i) {
            assert(i == 6);
        }
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
    }
#endif

    return 0;
}
