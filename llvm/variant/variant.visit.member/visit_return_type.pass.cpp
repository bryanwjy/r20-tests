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

// template<class R, class Self, class Visitor>
//   constexpr R visit(this Self&&, Visitor&&);              // since C++26

#include "../../variant_test_helpers.h"
#include "rxx/variant.h"

#include <cassert>
#include <string>
#include <type_traits>
#include <utility>

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void test_overload_ambiguity() {
    using V = __RXX variant<float, long, std::string>;
    using namespace std::string_literals;
    V v{"baba"s};

    v.visit(overloaded{[]([[maybe_unused]] auto x) { assert(false); },
        [](std::string const& x) { assert(x == "baba"s); }});
    assert(__RXX get<std::string>(v) == "baba"s);

    // Test the constraint.
    v = std::move(v).visit<V>(overloaded{[]([[maybe_unused]] auto x) {
                                             assert(false);
                                             return 0;
                                         },
        [](std::string const& x) {
            assert(x == "baba"s);
            return x + " zmt"s;
        }});
    assert(__RXX get<std::string>(v) == "baba zmt"s);
}

template <typename ReturnType>
void test_call_operator_forwarding() {
    using Fn = ForwardingCallObject;
    Fn obj{};
    Fn const& cobj = obj;

    { // test call operator forwarding - single variant, single arg
        using V = __RXX variant<int>;
        V v(42);

        v.visit<ReturnType>(obj);
        assert(Fn::check_call<int&>(CT_NonConst | CT_LValue));
        v.visit<ReturnType>(cobj);
        assert(Fn::check_call<int&>(CT_Const | CT_LValue));
        v.visit<ReturnType>(std::move(obj));
        assert(Fn::check_call<int&>(CT_NonConst | CT_RValue));
        v.visit<ReturnType>(std::move(cobj));
        assert(Fn::check_call<int&>(CT_Const | CT_RValue));
    }
    { // test call operator forwarding - single variant, multi arg
        using V = __RXX variant<int, long, double>;
        V v(42L);

        v.visit<ReturnType>(obj);
        assert(Fn::check_call<long&>(CT_NonConst | CT_LValue));
        v.visit<ReturnType>(cobj);
        assert(Fn::check_call<long&>(CT_Const | CT_LValue));
        v.visit<ReturnType>(std::move(obj));
        assert(Fn::check_call<long&>(CT_NonConst | CT_RValue));
        v.visit<ReturnType>(std::move(cobj));
        assert(Fn::check_call<long&>(CT_Const | CT_RValue));
    }
}

template <typename ReturnType>
void test_argument_forwarding() {
    using Fn = ForwardingCallObject;
    Fn obj{};
    auto const val = CT_LValue | CT_NonConst;

    { // single argument - value type
        using V = __RXX variant<int>;
        V v(42);
        V const& cv = v;

        v.visit<ReturnType>(obj);
        assert(Fn::check_call<int&>(val));
        cv.visit<ReturnType>(obj);
        assert(Fn::check_call<int const&>(val));
        std::move(v).visit<ReturnType>(obj);
        assert(Fn::check_call<int&&>(val));
        std::move(cv).visit<ReturnType>(obj);
        assert(Fn::check_call<int const&&>(val));
    }
}

template <typename ReturnType>
void test_return_type() {
    using Fn = ForwardingCallObject;
    Fn obj{};
    Fn const& cobj = obj;

    { // test call operator forwarding - no variant
        // non-member
        {
            static_assert(
                std::is_same_v<decltype(__RXX visit<ReturnType>(obj)),
                    ReturnType>);
            static_assert(
                std::is_same_v<decltype(__RXX visit<ReturnType>(cobj)),
                    ReturnType>);
            static_assert(std::is_same_v<decltype(__RXX visit<ReturnType>(
                                             std::move(obj))),
                ReturnType>);
            static_assert(std::is_same_v<decltype(__RXX visit<ReturnType>(
                                             std::move(cobj))),
                ReturnType>);
        }
    }
    { // test call operator forwarding - single variant, single arg
        using V = __RXX variant<int>;
        V v(42);

        static_assert(
            std::is_same_v<decltype(v.visit<ReturnType>(obj)), ReturnType>);
        static_assert(
            std::is_same_v<decltype(v.visit<ReturnType>(cobj)), ReturnType>);
        static_assert(
            std::is_same_v<decltype(v.visit<ReturnType>(std::move(obj))),
                ReturnType>);
        static_assert(
            std::is_same_v<decltype(v.visit<ReturnType>(std::move(cobj))),
                ReturnType>);
    }
    { // test call operator forwarding - single variant, multi arg
        using V = __RXX variant<int, long, double>;
        V v(42L);

        static_assert(
            std::is_same_v<decltype(v.visit<ReturnType>(obj)), ReturnType>);
        static_assert(
            std::is_same_v<decltype(v.visit<ReturnType>(cobj)), ReturnType>);
        static_assert(
            std::is_same_v<decltype(v.visit<ReturnType>(std::move(obj))),
                ReturnType>);
        static_assert(
            std::is_same_v<decltype(v.visit<ReturnType>(std::move(cobj))),
                ReturnType>);
    }
}

void test_constexpr_void() {
    constexpr ReturnFirst obj{};

    {
        using V = __RXX variant<int>;
        constexpr V v(42);

        static_assert((v.visit<void>(obj), 42) == 42);
    }
    {
        using V = __RXX variant<short, long, char>;
        constexpr V v(42L);

        static_assert((v.visit<void>(obj), 42) == 42);
    }
}

void test_constexpr_int() {
    constexpr ReturnFirst obj{};

    {
        using V = __RXX variant<int>;
        constexpr V v(42);

        static_assert(v.visit<int>(obj) == 42);
    }
    {
        using V = __RXX variant<short, long, char>;
        constexpr V v(42L);

        static_assert(v.visit<int>(obj) == 42);
    }
}

template <typename ReturnType>
void test_exceptions() {
#if RXX_WITH_EXCEPTIONS
    ReturnArity obj{};

    auto test = [&](auto&& v) {
        try {
            v.template visit<ReturnType>(obj);
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
template <typename ReturnType>
void test_caller_accepts_nonconst() {
    struct A {};
    struct Visitor {
        auto operator()(A&) {
            if constexpr (!std::is_void_v<ReturnType>) {
                return ReturnType{};
            }
        }
    };
    __RXX variant<A> v;

    v.template visit<ReturnType>(Visitor{});
}

void test_constexpr_explicit_side_effect() {
    auto test_lambda = [](int arg) constexpr {
        __RXX variant<int> v = 101;

        {
            v.template visit<void>([arg](int& x) constexpr { x = arg; });
        }

        return __RXX get<int>(v);
    };

    static_assert(test_lambda(202) == 202);
}

void test_derived_from_variant() {
    struct MyVariant : __RXX variant<short, long, float> {};

    MyVariant{42}.template visit<bool>([](auto x) {
        assert(x == 42);
        return true;
    });
    MyVariant{-1.3f}.template visit<bool>([](auto x) {
        assert(x == -1.3f);
        return true;
    });

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

    EvilVariant1{12}.template visit<bool>([](auto x) {
        assert(x == 12);
        return true;
    });
    EvilVariant1{12.3}.template visit<bool>([](auto x) {
        assert(x == 12.3);
        return true;
    });

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

    EvilVariant2{12}.template visit<bool>([](auto x) {
        assert(x == 12);
        return true;
    });
    EvilVariant2{12.3}.template visit<bool>([](auto x) {
        assert(x == 12.3);
        return true;
    });
}

int main(int, char**) {
    test_overload_ambiguity();
    test_call_operator_forwarding<void>();
    test_argument_forwarding<void>();
    test_return_type<void>();
    test_constexpr_void();
    test_exceptions<void>();
    test_caller_accepts_nonconst<void>();
    test_call_operator_forwarding<int>();
    test_argument_forwarding<int>();
    test_return_type<int>();
    test_constexpr_int();
    test_exceptions<int>();
    test_caller_accepts_nonconst<int>();
    test_constexpr_explicit_side_effect();
    test_derived_from_variant();

    return 0;
}
