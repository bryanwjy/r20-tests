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

// template <class... Args> T& optional<T>::emplace(Args&&... args);

#include "../../../archetypes.h"
#include "rxx/optional.h"

#include <cassert>
#include <memory>
#include <type_traits>

using __RXX optional;

class X {
    int i_;
    int j_ = 0;

public:
    constexpr X() : i_(0) {}
    constexpr X(int i) : i_(i) {}
    constexpr X(int i, int j) : i_(i), j_(j) {}

    friend constexpr bool operator==(X const& x, X const& y) {
        return x.i_ == y.i_ && x.j_ == y.j_;
    }
};

class Y {
public:
    static bool dtor_called;
    Y() = default;
    Y(int) { RXX_THROW(6); }
    Y(Y const&) = default;
    Y& operator=(Y const&) = default;
    ~Y() { dtor_called = true; }
};

bool Y::dtor_called = false;

template <class T>
constexpr bool test_one_arg() {
    using Opt = __RXX optional<T>;
    {
        Opt opt;
        auto& v = opt.emplace();
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(0));
        assert(&v == &*opt);
    }
    {
        Opt opt;
        auto& v = opt.emplace(1);
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(1));
        assert(&v == &*opt);
    }
    {
        Opt opt(2);
        auto& v = opt.emplace();
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(0));
        assert(&v == &*opt);
    }
    {
        Opt opt(2);
        auto& v = opt.emplace(1);
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(1));
        assert(&v == &*opt);
    }
    return true;
}

template <class T>
constexpr bool test_multi_arg() {
    test_one_arg<T>();
    using Opt = __RXX optional<T>;
    {
        Opt opt;
        auto& v = opt.emplace(101, 41);
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(static_cast<bool>(opt) == true);
        assert(v == T(101, 41));
        assert(*opt == T(101, 41));
    }
    {
        Opt opt;
        auto& v = opt.emplace({1, 2, 3, 4});
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(static_cast<bool>(opt) == true);
        assert(v == T(4)); // T sets its value to the size of the init list
        assert(*opt == T(4));
    }
    {
        Opt opt;
        auto& v = opt.emplace({1, 2, 3, 4, 5}, 6);
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(static_cast<bool>(opt) == true);
        assert(v == T(5));    // T sets its value to the size of the init list
        assert(*opt == T(5)); // T sets its value to the size of the init list
    }
    return true;
}

template <class T>
void test_on_test_type() {

    T::reset();
    optional<T> opt;
    assert(T::alive == 0);
    {
        T::reset_constructors();
        auto& v = opt.emplace();
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::default_constructed == 1);
        assert(T::destroyed == 0);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T());
        assert(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto& v = opt.emplace();
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::default_constructed == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T());
        assert(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto& v = opt.emplace(101);
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(101));
        assert(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto& v = opt.emplace(-10, 99);
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(-10, 99));
        assert(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto& v = opt.emplace(-10, 99);
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(-10, 99));
        assert(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto& v = opt.emplace({-10, 99, 42, 1});
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(4)); // size of the initializer list
        assert(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto& v = opt.emplace({-10, 99, 42, 1}, 42);
        static_assert(std::is_same_v<T&, decltype(v)>, "");
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(4)); // size of the initializer list
        assert(&v == &*opt);
    }
}

constexpr bool test_empty_emplace() {
    optional<int const> opt;
    auto& v = opt.emplace(42);
    static_assert(std::is_same_v<int const&, decltype(v)>, "");
    assert(*opt == 42);
    assert(v == 42);
    opt.emplace();
    assert(*opt == 0);
    return true;
}

#if RXX_SUPPORTS_OPTIONAL_REFERENCES
template <class T, std::remove_reference_t<T> _Val>
constexpr bool test_ref() {
    using Opt = __RXX optional<T&>;
    T t{_Val};
    {
        Opt opt;
        auto& v = opt.emplace(t);
        static_assert(std::is_same_v<T&, decltype(v)>);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == t);
        assert(&v == &*opt);
        assert(&t == &*opt);
    }
    return true;
}
#endif

int main(int, char**) {
    {
        test_on_test_type<TestTypes::TestType>();
        test_on_test_type<ExplicitTestTypes::TestType>();
    }
    {
        using T = int;
        test_one_arg<T>();
        test_one_arg<T const>();
        static_assert(test_one_arg<T>());
        static_assert(test_one_arg<T const>());
    }
    {
        using T = ConstexprTestTypes::TestType;
        test_multi_arg<T>();
        static_assert(test_multi_arg<T>());
    }
    {
        using T = ExplicitConstexprTestTypes::TestType;
        test_multi_arg<T>();
        static_assert(test_multi_arg<T>());
    }
    {
        using T = TrivialTestTypes::TestType;
        test_multi_arg<T>();
        static_assert(test_multi_arg<T>());
    }
    {
        using T = ExplicitTrivialTestTypes::TestType;
        test_multi_arg<T>();
        static_assert(test_multi_arg<T>());
    }
    {
        test_empty_emplace();
        static_assert(test_empty_emplace());
    }
#if RXX_WITH_EXCEPTIONS
    Y::dtor_called = false;
    {
        Y y;
        optional<Y> opt(y);
        try {
            assert(static_cast<bool>(opt) == true);
            assert(Y::dtor_called == false);
            auto& v = opt.emplace(1);
            static_assert(std::is_same_v<Y&, decltype(v)>, "");
            assert(false);
        } catch (int i) {
            assert(i == 6);
            assert(static_cast<bool>(opt) == false);
            assert(Y::dtor_called == true);
        }
    }
#endif
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    static_assert(test_ref<int, 1>());
    static_assert(test_ref<double, 15.0>());
    assert((test_ref<int, 1>()));
    assert((test_ref<double, 15.0>()));
#endif
    return 0;
}
