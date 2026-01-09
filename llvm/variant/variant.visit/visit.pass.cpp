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
// template <class Visitor, class... Variants>
// constexpr see below visit(Visitor&& vis, Variants&&... vars);

#include "../../variant_test_helpers.h"
#include "rxx/variant.h"

#include <cassert>
#include <string>
#include <type_traits>
#include <utility>

void test_call_operator_forwarding() {
    using Fn = ForwardingCallObject;
    Fn obj{};
    Fn const& cobj = obj;
    { // test call operator forwarding - no variant
        __RXX visit(obj);
        assert(Fn::check_call<>(CT_NonConst | CT_LValue));
        __RXX visit(cobj);
        assert(Fn::check_call<>(CT_Const | CT_LValue));
        __RXX visit(std::move(obj));
        assert(Fn::check_call<>(CT_NonConst | CT_RValue));
        __RXX visit(std::move(cobj));
        assert(Fn::check_call<>(CT_Const | CT_RValue));
    }
    { // test call operator forwarding - single variant, single arg
        using V = __RXX variant<int>;
        V v(42);
        __RXX visit(obj, v);
        assert(Fn::check_call<int&>(CT_NonConst | CT_LValue));
        __RXX visit(cobj, v);
        assert(Fn::check_call<int&>(CT_Const | CT_LValue));
        __RXX visit(std::move(obj), v);
        assert(Fn::check_call<int&>(CT_NonConst | CT_RValue));
        __RXX visit(std::move(cobj), v);
        assert(Fn::check_call<int&>(CT_Const | CT_RValue));
    }
    { // test call operator forwarding - single variant, multi arg
        using V = __RXX variant<int, long, double>;
        V v(42l);
        __RXX visit(obj, v);
        assert(Fn::check_call<long&>(CT_NonConst | CT_LValue));
        __RXX visit(cobj, v);
        assert(Fn::check_call<long&>(CT_Const | CT_LValue));
        __RXX visit(std::move(obj), v);
        assert(Fn::check_call<long&>(CT_NonConst | CT_RValue));
        __RXX visit(std::move(cobj), v);
        assert(Fn::check_call<long&>(CT_Const | CT_RValue));
    }
    { // test call operator forwarding - multi variant, multi arg
        using V = __RXX variant<int, long, double>;
        using V2 = __RXX variant<int*, std::string>;
        V v(42l);
        V2 v2("hello");
        __RXX visit(obj, v, v2);
        assert((Fn::check_call<long&, std::string&>(CT_NonConst | CT_LValue)));
        __RXX visit(cobj, v, v2);
        assert((Fn::check_call<long&, std::string&>(CT_Const | CT_LValue)));
        __RXX visit(std::move(obj), v, v2);
        assert((Fn::check_call<long&, std::string&>(CT_NonConst | CT_RValue)));
        __RXX visit(std::move(cobj), v, v2);
        assert((Fn::check_call<long&, std::string&>(CT_Const | CT_RValue)));
    }
#ifndef RXX_TEST_LIGHTWEIGHT
    {
        using V = __RXX variant<int, long, double, std::string>;
        V v1(42l), v2("hello"), v3(101), v4(1.1);
        __RXX visit(obj, v1, v2, v3, v4);
        assert((Fn::check_call<long&, std::string&, int&, double&>(
            CT_NonConst | CT_LValue)));
        __RXX visit(cobj, v1, v2, v3, v4);
        assert((Fn::check_call<long&, std::string&, int&, double&>(
            CT_Const | CT_LValue)));
        __RXX visit(std::move(obj), v1, v2, v3, v4);
        assert((Fn::check_call<long&, std::string&, int&, double&>(
            CT_NonConst | CT_RValue)));
        __RXX visit(std::move(cobj), v1, v2, v3, v4);
        assert((Fn::check_call<long&, std::string&, int&, double&>(
            CT_Const | CT_RValue)));
    }
    {
        using V = __RXX variant<int, long, double, int*, std::string>;
        V v1(42l), v2("hello"), v3(nullptr), v4(1.1);
        __RXX visit(obj, v1, v2, v3, v4);
        assert((Fn::check_call<long&, std::string&, int*&, double&>(
            CT_NonConst | CT_LValue)));
        __RXX visit(cobj, v1, v2, v3, v4);
        assert((Fn::check_call<long&, std::string&, int*&, double&>(
            CT_Const | CT_LValue)));
        __RXX visit(std::move(obj), v1, v2, v3, v4);
        assert((Fn::check_call<long&, std::string&, int*&, double&>(
            CT_NonConst | CT_RValue)));
        __RXX visit(std::move(cobj), v1, v2, v3, v4);
        assert((Fn::check_call<long&, std::string&, int*&, double&>(
            CT_Const | CT_RValue)));
    }
#endif
}

void test_argument_forwarding() {
    using Fn = ForwardingCallObject;
    Fn obj{};
    auto const Val = CT_LValue | CT_NonConst;
    { // single argument - value type
        using V = __RXX variant<int>;
        V v(42);
        V const& cv = v;
        __RXX visit(obj, v);
        assert(Fn::check_call<int&>(Val));
        __RXX visit(obj, cv);
        assert(Fn::check_call<int const&>(Val));
        __RXX visit(obj, std::move(v));
        assert(Fn::check_call<int&&>(Val));
        __RXX visit(obj, std::move(cv));
        assert(Fn::check_call<int const&&>(Val));
    }
    { // multi argument - multi variant
        using V = __RXX variant<int, std::string, long>;
        V v1(42), v2("hello"), v3(43l);
        __RXX visit(obj, v1, v2, v3);
        assert((Fn::check_call<int&, std::string&, long&>(Val)));
        __RXX visit(obj, std::as_const(v1), std::as_const(v2), std::move(v3));
        assert((Fn::check_call<int const&, std::string const&, long&&>(Val)));
    }
#ifndef RXX_TEST_LIGHTWEIGHT
    {
        using V = __RXX variant<int, long, double, std::string>;
        V v1(42l), v2("hello"), v3(101), v4(1.1);
        __RXX visit(obj, v1, v2, v3, v4);
        assert((Fn::check_call<long&, std::string&, int&, double&>(Val)));
        __RXX visit(obj, std::as_const(v1), std::as_const(v2), std::move(v3),
            std::move(v4));
        assert(
            (Fn::check_call<long const&, std::string const&, int&&, double&&>(
                Val)));
    }
    {
        using V = __RXX variant<int, long, double, int*, std::string>;
        V v1(42l), v2("hello"), v3(nullptr), v4(1.1);
        __RXX visit(obj, v1, v2, v3, v4);
        assert((Fn::check_call<long&, std::string&, int*&, double&>(Val)));
        __RXX visit(obj, std::as_const(v1), std::as_const(v2), std::move(v3),
            std::move(v4));
        assert(
            (Fn::check_call<long const&, std::string const&, int*&&, double&&>(
                Val)));
    }
#endif
}

void test_return_type() {
    using Fn = ForwardingCallObject;
    Fn obj{};
    Fn const& cobj = obj;
    { // test call operator forwarding - no variant
        static_assert(std::is_same_v<decltype(__RXX visit(obj)), Fn&>);
        static_assert(std::is_same_v<decltype(__RXX visit(cobj)), Fn const&>);
        static_assert(
            std::is_same_v<decltype(__RXX visit(std::move(obj))), Fn&&>);
        static_assert(std::is_same_v<decltype(__RXX visit(std::move(cobj))),
            Fn const&&>);
    }
    { // test call operator forwarding - single variant, single arg
        using V = __RXX variant<int>;
        V v(42);
        static_assert(std::is_same_v<decltype(__RXX visit(obj, v)), Fn&>);
        static_assert(
            std::is_same_v<decltype(__RXX visit(cobj, v)), Fn const&>);
        static_assert(
            std::is_same_v<decltype(__RXX visit(std::move(obj), v)), Fn&&>);
        static_assert(
            std::is_same_v<decltype(__RXX visit(std::move(cobj), v)),
                Fn const&&>);
    }
    { // test call operator forwarding - single variant, multi arg
        using V = __RXX variant<int, long, double>;
        V v(42l);
        static_assert(std::is_same_v<decltype(__RXX visit(obj, v)), Fn&>);
        static_assert(
            std::is_same_v<decltype(__RXX visit(cobj, v)), Fn const&>);
        static_assert(
            std::is_same_v<decltype(__RXX visit(std::move(obj), v)), Fn&&>);
        static_assert(
            std::is_same_v<decltype(__RXX visit(std::move(cobj), v)),
                Fn const&&>);
    }
    { // test call operator forwarding - multi variant, multi arg
        using V = __RXX variant<int, long, double>;
        using V2 = __RXX variant<int*, std::string>;
        V v(42l);
        V2 v2("hello");
        static_assert(std::is_same_v<decltype(__RXX visit(obj, v, v2)), Fn&>);
        static_assert(
            std::is_same_v<decltype(__RXX visit(cobj, v, v2)), Fn const&>);
        static_assert(
            std::is_same_v<decltype(__RXX visit(std::move(obj), v, v2)),
                Fn&&>);
        static_assert(
            std::is_same_v<decltype(__RXX visit(std::move(cobj), v, v2)),
                Fn const&&>);
    }
#ifndef RXX_TEST_LIGHTWEIGHT
    {
        using V = __RXX variant<int, long, double, std::string>;
        V v1(42l), v2("hello"), v3(101), v4(1.1);
        static_assert(
            std::is_same_v<decltype(__RXX visit(obj, v1, v2, v3, v4)), Fn&>);
        static_assert(
            std::is_same_v<decltype(__RXX visit(cobj, v1, v2, v3, v4)),
                Fn const&>);
        static_assert(std::is_same_v<
            decltype(__RXX visit(std::move(obj), v1, v2, v3, v4)), Fn&&>);
        static_assert(std::is_same_v<decltype(__RXX visit(
                                         std::move(cobj), v1, v2, v3, v4)),
            Fn const&&>);
    }
    {
        using V = __RXX variant<int, long, double, int*, std::string>;
        V v1(42l), v2("hello"), v3(nullptr), v4(1.1);
        static_assert(
            std::is_same_v<decltype(__RXX visit(obj, v1, v2, v3, v4)), Fn&>);
        static_assert(
            std::is_same_v<decltype(__RXX visit(cobj, v1, v2, v3, v4)),
                Fn const&>);
        static_assert(std::is_same_v<
            decltype(__RXX visit(std::move(obj), v1, v2, v3, v4)), Fn&&>);
        static_assert(std::is_same_v<decltype(__RXX visit(
                                         std::move(cobj), v1, v2, v3, v4)),
            Fn const&&>);
    }
#endif
}

void test_constexpr() {
    constexpr ReturnFirst obj{};
    constexpr ReturnArity aobj{};
    {
        using V = __RXX variant<int>;
        constexpr V v(42);
        static_assert(__RXX visit(obj, v) == 42, "");
    }
    {
        using V = __RXX variant<short, long, char>;
        constexpr V v(42l);
        static_assert(__RXX visit(obj, v) == 42, "");
    }
    {
        using V1 = __RXX variant<int>;
        using V2 = __RXX variant<int, char*, long long>;
        using V3 = __RXX variant<bool, int, int>;
        constexpr V1 v1;
        constexpr V2 v2(nullptr);
        constexpr V3 v3;
        static_assert(__RXX visit(aobj, v1, v2, v3) == 3, "");
    }
    {
        using V1 = __RXX variant<int>;
        using V2 = __RXX variant<int, char*, long long>;
        using V3 = __RXX variant<void*, int, int>;
        constexpr V1 v1;
        constexpr V2 v2(nullptr);
        constexpr V3 v3;
        static_assert(__RXX visit(aobj, v1, v2, v3) == 3, "");
    }
#ifndef RXX_TEST_LIGHTWEIGHT
    {
        using V = __RXX variant<int, long, double, int*>;
        constexpr V v1(42l), v2(101), v3(nullptr), v4(1.1);
        static_assert(__RXX visit(aobj, v1, v2, v3, v4) == 4, "");
    }
    {
        using V = __RXX variant<int, long, double, long long, int*>;
        constexpr V v1(42l), v2(101), v3(nullptr), v4(1.1);
        static_assert(__RXX visit(aobj, v1, v2, v3, v4) == 4, "");
    }
#endif
}

void test_exceptions() {
#if RXX_WITH_EXCEPTIONS
    ReturnArity obj{};
    auto test = [&](auto&&... args) {
        try {
            __RXX visit(obj, args...);
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
    {
        using V = __RXX variant<int, MakeEmptyT>;
        using V2 = __RXX variant<long, std::string, void*>;
        V v;
        makeEmpty(v);
        V2 v2("hello");
        assert(test(v, v2));
    }
    {
        using V = __RXX variant<int, MakeEmptyT>;
        using V2 = __RXX variant<long, std::string, void*>;
        V v;
        makeEmpty(v);
        V2 v2("hello");
        assert(test(v2, v));
    }
    {
        using V = __RXX variant<int, MakeEmptyT>;
        using V2 = __RXX variant<long, std::string, void*, MakeEmptyT>;
        V v;
        makeEmpty(v);
        V2 v2;
        makeEmpty(v2);
        assert(test(v, v2));
    }
#  ifndef RXX_TEST_LIGHTWEIGHT
    {
        using V = __RXX variant<int, long, double, MakeEmptyT>;
        V v1(42l), v2(101), v3(202), v4(1.1);
        makeEmpty(v1);
        assert(test(v1, v2, v3, v4));
    }
    {
        using V = __RXX variant<int, long, double, long long, MakeEmptyT>;
        V v1(42l), v2(101), v3(202), v4(1.1);
        makeEmpty(v1);
        makeEmpty(v2);
        makeEmpty(v3);
        makeEmpty(v4);
        assert(test(v1, v2, v3, v4));
    }
#  endif
#endif
}

// See https://llvm.org/PR31916
void test_caller_accepts_nonconst() {
    struct A {};
    struct Visitor {
        void operator()(A&) {}
    };
    __RXX variant<A> v;
    __RXX visit(Visitor{}, v);
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
    __RXX visit([](auto x) { assert(x == 42); }, v1);
    __RXX visit([](auto x) { assert(x == 142); }, cv1);
    __RXX visit([](auto x) { assert(x == -1.25f); }, MyVariant{-1.25f});
    __RXX visit([](auto x) { assert(x == 42); }, std::move(v1));
    __RXX visit([](auto x) { assert(x == 142); }, std::move(cv1));

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

    __RXX visit([](auto x) { assert(x == 12); }, EvilVariant1{12});
    __RXX visit([](auto x) { assert(x == 12.3); }, EvilVariant1{12.3});

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

    __RXX visit([](auto x) { assert(x == 12); }, EvilVariant2{12});
    __RXX visit([](auto x) { assert(x == 12.3); }, EvilVariant2{12.3});
}

struct any_visitor {
    template <typename T>
    void operator()(T const&) const {}
};

template <typename T,
    typename = decltype(__RXX visit(
        std::declval<any_visitor&>(), std::declval<T>()))>
constexpr bool has_visit(int) {
    return true;
}

template <typename T>
constexpr bool has_visit(...) {
    return false;
}

void test_sfinae() {
    struct BadVariant : __RXX variant<short>, __RXX variant<long, float> {};
    struct BadVariant2 : private __RXX variant<long, float> {};
    struct GoodVariant : __RXX variant<long, float> {};
    struct GoodVariant2 : GoodVariant {};

    static_assert(!has_visit<int>(0));
    static_assert(!has_visit<BadVariant>(0));
    static_assert(!has_visit<BadVariant2>(0));
    static_assert(has_visit<__RXX variant<int>>(0));
    static_assert(has_visit<GoodVariant>(0));
    static_assert(has_visit<GoodVariant2>(0));
}

int main(int, char**) {
    test_call_operator_forwarding();
    test_argument_forwarding();
    test_return_type();
    test_constexpr();
    test_exceptions();
    test_caller_accepts_nonconst();
    test_derived_from_variant();
    test_sfinae();

    return 0;
}
