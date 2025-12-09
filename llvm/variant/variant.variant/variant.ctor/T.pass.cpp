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

// template <class T> constexpr variant(T&&) noexcept(see below);

#include "../../../variant_test_helpers.h"
#include "rxx/variant.h"

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

struct Dummy {
    Dummy() = default;
};

struct ThrowsT {
    ThrowsT(int) noexcept(false) {}
};

struct NoThrowT {
    NoThrowT(int) noexcept(true) {}
};

struct AnyConstructible {
    template <typename T>
    AnyConstructible(T&&) {}
};
struct NoConstructible {
    NoConstructible() = delete;
};
template <class T>
struct RValueConvertibleFrom {
    RValueConvertibleFrom(T&&) {}
};

void test_T_ctor_noexcept() {
    {
        using V = __RXX variant<Dummy, NoThrowT>;
        static_assert(std::is_nothrow_constructible<V, int>::value, "");
    }
    {
        using V = __RXX variant<Dummy, ThrowsT>;
        static_assert(!std::is_nothrow_constructible<V, int>::value, "");
    }
}

void test_T_ctor_sfinae() {
    {
        using V = __RXX variant<long, long long>;
        static_assert(!std::is_constructible<V, int>::value, "ambiguous");
    }
    {
        using V = __RXX variant<std::string, std::string>;
        static_assert(
            !std::is_constructible<V, char const*>::value, "ambiguous");
    }
    {
        using V = __RXX variant<std::string, void*>;
        static_assert(
            !std::is_constructible<V, int>::value, "no matching constructor");
    }
    {
        using V = __RXX variant<std::string, float>;
        static_assert(
            !std::is_constructible<V, int>::value, "no matching constructor");
    }
    {
        using V = __RXX variant<std::unique_ptr<int>, bool>;
        static_assert(!std::is_constructible<V, std::unique_ptr<char>>::value,
            "no explicit bool in constructor");
        struct X {
            operator void*();
        };
        static_assert(!std::is_constructible<V, X>::value,
            "no boolean conversion in constructor");
        static_assert(std::is_constructible<V, std::false_type>::value,
            "converted to bool in constructor");
    }
    {
        struct X {};
        struct Y {
            operator X();
        };
        using V = __RXX variant<X>;
        static_assert(std::is_constructible<V, Y>::value,
            "regression on user-defined conversions in constructor");
    }
    {
        using V = __RXX variant<AnyConstructible, NoConstructible>;
        static_assert(!std::is_constructible<V,
                          std::in_place_type_t<NoConstructible>>::value,
            "no matching constructor");
        static_assert(
            !std::is_constructible<V, std::in_place_index_t<1>>::value,
            "no matching constructor");
    }
}

void test_T_ctor_basic() {
    {
        constexpr __RXX variant<int> v(42);
        static_assert(v.index() == 0, "");
        static_assert(__RXX get<0>(v) == 42, "");
    }
    {
        constexpr __RXX variant<int, long> v(42l);
        static_assert(v.index() == 1, "");
        static_assert(__RXX get<1>(v) == 42, "");
    }
    {
        constexpr __RXX variant<unsigned, long> v(42);
        static_assert(v.index() == 1, "");
        static_assert(__RXX get<1>(v) == 42, "");
    }
    {
        __RXX variant<std::string, bool const> v = "foo";
        assert(v.index() == 0);
        assert(__RXX get<0>(v) == "foo");
    }
    {
        __RXX variant<bool, std::unique_ptr<int>> v = nullptr;
        assert(v.index() == 1);
        assert(__RXX get<1>(v) == nullptr);
    }
    {
        __RXX variant<bool const, int> v = true;
        assert(v.index() == 0);
        assert(__RXX get<0>(v));
    }
    {
        __RXX variant<RValueConvertibleFrom<int>> v1 = 42;
        assert(v1.index() == 0);

        int x = 42;
        __RXX variant<RValueConvertibleFrom<int>, AnyConstructible> v2 = x;
        assert(v2.index() == 1);
    }
}

struct BoomOnAnything {
    template <class T>
    constexpr BoomOnAnything(T) {
        static_assert(!std::is_same<T, T>::value, "");
    }
};

void test_no_narrowing_check_for_class_types() {
    using V = __RXX variant<int, BoomOnAnything>;
    V v(42);
    assert(v.index() == 0);
    assert(__RXX get<0>(v) == 42);
}

struct Bar {};
struct Baz {};
void test_construction_with_repeated_types() {
    using V = __RXX variant<int, Bar, Baz, int, Baz, int, int>;
    static_assert(!std::is_constructible<V, int>::value, "");
    static_assert(!std::is_constructible<V, Baz>::value, "");
    // OK, the selected type appears only once and so it shouldn't
    // be affected by the duplicate types.
    static_assert(std::is_constructible<V, Bar>::value, "");
}

void test_vector_bool() {
    std::vector<bool> vec = {true};
    __RXX variant<bool, int> v = vec[0];
    assert(v.index() == 0);
    assert(__RXX get<0>(v) == true);
}

struct ConvertibleFromAny {
    template <class V>
    ConvertibleFromAny(V) {}
};

int main(int, char**) {
    test_T_ctor_basic();
    test_T_ctor_noexcept();
    test_T_ctor_sfinae();
    test_no_narrowing_check_for_class_types();
    test_construction_with_repeated_types();
    test_vector_bool();

#if !RXX_COMPILER_CLANG
    // Until clang fixes their compiler bug, disable for now

    { // Check that the constraints are evaluated lazily (see llvm.org/PR151328)
        struct Matcher {
            Matcher() {}
            Matcher(__RXX variant<ConvertibleFromAny>) {}
        };

        Matcher vec;
        [[maybe_unused]] Matcher m = std::move(vec);
    }
#endif

    return 0;
}
