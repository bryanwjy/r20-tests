// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20, c++23

// <variant>

// class variant;

// template<class Self, class Visitor>
//   constexpr decltype(auto) visit(this Self&&, Visitor&&); // since C++26

#include "../../variant_test_helpers.h"
#include "rxx/variant.h"

#include <cassert>
#include <tuple>
#include <type_traits>
#include <utility>

void test_call_operator_forwarding() {
    using Fn = ForwardingCallObject;
    Fn obj{};
    Fn const& cobj = obj;

    { // test call operator forwarding - single variant, single arg
        using V = __RXX variant<int>;
        V v(42);

        v.visit(obj);
        assert(Fn::check_call<int&>(CT_NonConst | CT_LValue));
        v.visit(cobj);
        assert(Fn::check_call<int&>(CT_Const | CT_LValue));
        v.visit(std::move(obj));
        assert(Fn::check_call<int&>(CT_NonConst | CT_RValue));
        v.visit(std::move(cobj));
        assert(Fn::check_call<int&>(CT_Const | CT_RValue));
    }
    { // test call operator forwarding - single variant, multi arg
        using V = __RXX variant<int, long, double>;
        V v(42L);

        v.visit(obj);
        assert(Fn::check_call<long&>(CT_NonConst | CT_LValue));
        v.visit(cobj);
        assert(Fn::check_call<long&>(CT_Const | CT_LValue));
        v.visit(std::move(obj));
        assert(Fn::check_call<long&>(CT_NonConst | CT_RValue));
        v.visit(std::move(cobj));
        assert(Fn::check_call<long&>(CT_Const | CT_RValue));
    }
}

// Applies to non-member `std::visit` only.
void test_argument_forwarding() {
    using Fn = ForwardingCallObject;
    Fn obj{};
    auto const val = CT_LValue | CT_NonConst;

    { // single argument - value type
        using V = __RXX variant<int>;
        V v(42);
        V const& cv = v;

        v.visit(obj);
        assert(Fn::check_call<int&>(val));
        cv.visit(obj);
        assert(Fn::check_call<int const&>(val));
        std::move(v).visit(obj);
        assert(Fn::check_call<int&&>(val));
        std::move(cv).visit(obj);
        assert(Fn::check_call<int const&&>(val));
    }
}

void test_return_type() {
    using Fn = ForwardingCallObject;
    Fn obj{};
    Fn const& cobj = obj;

    { // test call operator forwarding - single variant, single arg
        using V = __RXX variant<int>;
        V v(42);

        static_assert(std::is_same_v<decltype(v.visit(obj)), Fn&>);
        static_assert(std::is_same_v<decltype(v.visit(cobj)), Fn const&>);
        static_assert(std::is_same_v<decltype(v.visit(std::move(obj))), Fn&&>);
        static_assert(
            std::is_same_v<decltype(v.visit(std::move(cobj))), Fn const&&>);
    }
    { // test call operator forwarding - single variant, multi arg
        using V = __RXX variant<int, long, double>;
        V v(42L);

        static_assert(std::is_same_v<decltype(v.visit(obj)), Fn&>);
        static_assert(std::is_same_v<decltype(v.visit(cobj)), Fn const&>);
        static_assert(std::is_same_v<decltype(v.visit(std::move(obj))), Fn&&>);
        static_assert(
            std::is_same_v<decltype(v.visit(std::move(cobj))), Fn const&&>);
    }
}

void test_constexpr() {
    constexpr ReturnFirst obj{};

    {
        using V = __RXX variant<int>;
        constexpr V v(42);

        static_assert(v.visit(obj) == 42);
    }
    {
        using V = __RXX variant<short, long, char>;
        constexpr V v(42L);

        static_assert(v.visit(obj) == 42);
    }
}

void test_exceptions() {
#if RXX_WITH_EXCEPTIONS
    ReturnArity obj{};

    auto test = [&](auto&& v) {
        try {
            v.visit(obj);
        } catch (__RXX bad_variant_access const&) {
            return true;
        } catch (...) {}
        return false;
    };

    {
        using V = __RXX variant<int, MakeEmptyT>;
        V v;
        makeEmpty(v);

        assert(test(v));
    }
#endif
}

// See https://llvm.org/PR31916
void test_caller_accepts_nonconst() {
    struct A {};
    struct Visitor {
        void operator()(A&) {}
    };
    __RXX variant<A> v;

    v.visit(Visitor{});
}

struct MyVariant : __RXX variant<short, long, float> {};

// FIXME: This is UB according to [namespace.std]
namespace std {
template <std::size_t Index>
void get(MyVariant const&) {
    assert(false);
}
} // namespace std

void test_derived_from_variant() {
    auto v1 = MyVariant{42};
    auto const cv1 = MyVariant{142};

    v1.visit([](auto x) { assert(x == 42); });
    cv1.visit([](auto x) { assert(x == 142); });
    MyVariant{-1.25f}.visit([](auto x) { assert(x == -1.25f); });
    std::move(v1).visit([](auto x) { assert(x == 42); });
    std::move(cv1).visit([](auto x) { assert(x == 142); });

    // Check that visit does not take index nor valueless_by_exception members
    // from the base class.
    struct EvilVariantBase {
        int index;
        char valueless_by_exception;
    };

    struct EvilVariant1 :
        __RXX variant<int, long, double>,
        std::tuple<int>,
        EvilVariantBase {
        using __RXX variant<int, long, double>::variant;
    };

    EvilVariant1{12}.visit([](auto x) { assert(x == 12); });
    EvilVariant1{12.3}.visit([](auto x) { assert(x == 12.3); });

    // Check that visit unambiguously picks the variant, even if the other base
    // has __impl member.
    struct ImplVariantBase {
        struct Callable {
            bool operator()() const {
                assert(false);
                return false;
            }
        };

        Callable __impl;
    };

    struct EvilVariant2 : __RXX variant<int, long, double>, ImplVariantBase {
        using __RXX variant<int, long, double>::variant;
    };

    EvilVariant2{12}.visit([](auto x) { assert(x == 12); });
    EvilVariant2{12.3}.visit([](auto x) { assert(x == 12.3); });
}

int main(int, char**) {
    test_call_operator_forwarding();
    test_argument_forwarding();
    test_return_type();
    test_constexpr();
    test_exceptions();
    test_caller_accepts_nonconst();
    test_derived_from_variant();

    return 0;
}
