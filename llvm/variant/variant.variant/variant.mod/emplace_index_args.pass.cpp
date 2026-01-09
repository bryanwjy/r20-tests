// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <variant>

// template <class ...Types> class variant;

// template <size_t I, class ...Args>
//   variant_alternative_t<I, variant<Types...>>& emplace(Args&&... args);

#include "../../../archetypes.h"
#include "rxx/variant.h"

#include <cassert>
#include <string>
#include <type_traits>

template <class Var, std::size_t I, class... Args>
constexpr auto test_emplace_exists_imp(int)
    -> decltype(std::declval<Var>().template emplace<I>(
                    std::declval<Args>()...),
        true) {
    return true;
}

template <class, std::size_t, class...>
constexpr auto test_emplace_exists_imp(long) -> bool {
    return false;
}

template <class Var, std::size_t I, class... Args>
constexpr bool emplace_exists() {
    return test_emplace_exists_imp<Var, I, Args...>(0);
}

constexpr void test_emplace_sfinae() {
    {
        using V = __RXX variant<int, void*, void const*, TestTypes::NoCtors>;
        static_assert(emplace_exists<V, 0>(), "");
        static_assert(emplace_exists<V, 0, int>(), "");
        static_assert(
            !emplace_exists<V, 0, decltype(nullptr)>(), "cannot construct");
        static_assert(emplace_exists<V, 1, decltype(nullptr)>(), "");
        static_assert(emplace_exists<V, 1, int*>(), "");
        static_assert(!emplace_exists<V, 1, int const*>(), "");
        static_assert(!emplace_exists<V, 1, int>(), "cannot construct");
        static_assert(emplace_exists<V, 2, int const*>(), "");
        static_assert(emplace_exists<V, 2, int*>(), "");
        static_assert(!emplace_exists<V, 3>(), "cannot construct");
    }
}

struct NoCtor {
    NoCtor() = delete;
};

constexpr void test_basic() {
    {
        using V = __RXX variant<int>;
        V v(42);
        auto& ref1 = v.emplace<0>();
        static_assert(std::is_same_v<int&, decltype(ref1)>, "");
        assert(__RXX get<0>(v) == 0);
        assert(&ref1 == &__RXX get<0>(v));
        auto& ref2 = v.emplace<0>(42);
        static_assert(std::is_same_v<int&, decltype(ref2)>, "");
        assert(__RXX get<0>(v) == 42);
        assert(&ref2 == &__RXX get<0>(v));
    }

    {
        using V = __RXX variant<int, long, void const*, NoCtor, std::string>;
        int const x = 100;
        V v(std::in_place_index<0>, -1);
        // default emplace a value
        auto& ref1 = v.emplace<1>();
        static_assert(std::is_same_v<long&, decltype(ref1)>, "");
        assert(__RXX get<1>(v) == 0);
        assert(&ref1 == &__RXX get<1>(v));
        auto& ref2 = v.emplace<2>(&x);
        static_assert(std::is_same_v<void const*&, decltype(ref2)>, "");
        assert(__RXX get<2>(v) == &x);
        assert(&ref2 == &__RXX get<2>(v));
        // Broken constexpr string in libstdc++
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AT_LEAST(14)
        if (!std::is_constant_evaluated())
#endif
        {
            // emplace with multiple args
            auto& ref3 = v.emplace<4>(3u, 'a');
            static_assert(std::is_same_v<std::string&, decltype(ref3)>, "");
            assert(__RXX get<4>(v) == "aaa");
            assert(&ref3 == &__RXX get<4>(v));
        }
    }
}

constexpr bool test() {
    test_basic();
    test_emplace_sfinae();

    return true;
}

int main(int, char**) {
    test();

    static_assert(test());

    return 0;
}
