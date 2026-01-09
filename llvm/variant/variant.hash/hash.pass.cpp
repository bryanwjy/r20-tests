//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <variant>

// template <class... Types> struct hash<variant<Types...>>;
// template <> struct hash<monostate>;

#include "../../poisoned_hash_helper.h"
#include "../../static_asserts.h"
#include "../../variant_test_helpers.h"
#include "rxx/variant.h"

#include <cassert>
#include <type_traits>

#if RXX_WITH_EXCEPTIONS
template <>
struct std::hash<::MakeEmptyT> {
    std::size_t operator()(::MakeEmptyT const&) const {
        assert(false);
        return 0;
    }
};
#endif

void test_hash_variant() {
    {
        using V = __RXX variant<int, long, int>;
        using H = std::hash<V>;
        V const v(std::in_place_index<0>, 42);
        V const v_copy = v;
        V v2(std::in_place_index<0>, 100);
        H const h{};
        assert(h(v) == h(v));
        assert(h(v) != h(v2));
        assert(h(v) == h(v_copy));
        {
            ASSERT_SAME_TYPE(decltype(h(v)), std::size_t);
            static_assert(std::is_copy_constructible<H>::value, "");
        }
    }
    {
        using V = __RXX variant<__RXX monostate, int, long, char const*>;
        using H = std::hash<V>;
        char const* str = "hello";
        V const v0;
        V const v0_other;
        V const v1(42);
        V const v1_other(100);
        V v2(100l);
        V v2_other(999l);
        V v3(str);
        V v3_other("not hello");
        H const h{};
        assert(h(v0) == h(v0));
        assert(h(v0) == h(v0_other));
        assert(h(v1) == h(v1));
        assert(h(v1) != h(v1_other));
        assert(h(v2) == h(v2));
        assert(h(v2) != h(v2_other));
        assert(h(v3) == h(v3));
        assert(h(v3) != h(v3_other));
        assert(h(v0) != h(v1));
        assert(h(v0) != h(v2));
        assert(h(v0) != h(v3));
        assert(h(v1) != h(v2));
        assert(h(v1) != h(v3));
        assert(h(v2) != h(v3));
    }
#if RXX_WITH_EXCEPTIONS
    {
        using V = __RXX variant<int, MakeEmptyT>;
        using H = std::hash<V>;
        V v;
        makeEmpty(v);
        V v2;
        makeEmpty(v2);
        H const h{};
        assert(h(v) == h(v2));
    }
#endif
}

void test_hash_monostate() {
    using H = std::hash<__RXX monostate>;
    H const h{};
    __RXX monostate m1{};
    __RXX monostate const m2{};
    assert(h(m1) == h(m1));
    assert(h(m2) == h(m2));
    assert(h(m1) == h(m2));
    {
        ASSERT_SAME_TYPE(decltype(h(m1)), std::size_t);
        ASSERT_NOEXCEPT(h(m1));
        static_assert(std::is_copy_constructible<H>::value, "");
    }
    { test_hash_enabled<__RXX monostate>(); }
}

void test_hash_variant_duplicate_elements() {
    // Test that the index of the alternative participates in the hash value.
    using V = __RXX variant<__RXX monostate, __RXX monostate>;
    using H = std::hash<V>;
    H h{};
    V const v1(std::in_place_index<0>);
    V const v2(std::in_place_index<1>);
    assert(h(v1) == h(v1));
    assert(h(v2) == h(v2));
    // LIBCPP_ASSERT(h(v1) != h(v2));
}

struct A {};
struct B {};

template <>
struct std::hash<B> {
    std::size_t operator()(B const&) const { return 0; }
};

void test_hash_variant_enabled() {
    {
        test_hash_enabled<__RXX variant<int>>();
        test_hash_enabled<__RXX variant<int*, long, double, int const>>();
    }
    {
        test_hash_disabled<__RXX variant<int, A>>();
        test_hash_disabled<__RXX variant<A const, void*>>();
    }
    {
        test_hash_enabled<__RXX variant<int, B>>();
        test_hash_enabled<__RXX variant<B const, int>>();
    }
}

int main(int, char**) {
    test_hash_variant();
    test_hash_variant_duplicate_elements();
    test_hash_monostate();
    test_hash_variant_enabled();

    return 0;
}
