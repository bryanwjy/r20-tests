// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// type_traits
// common_reference

#include "rxx/type_traits/common_reference.h"

#include "rxx/tuple.h"

#include <utility>

template <class T>
constexpr bool has_type = requires { typename T::type; };

// A slightly simplified variation of std::tuple
template <class...>
struct UserTuple {};

template <class, class, class>
struct Tuple_helper {};
template <class... Ts, class... Us>
struct Tuple_helper<std::void_t<rxx::common_reference_t<Ts, Us>...>,
    UserTuple<Ts...>, UserTuple<Us...>> {
    using type = UserTuple<rxx::common_reference_t<Ts, Us>...>;
};

template <class... Ts, class... Us, template <class> class TQual,
    template <class> class UQual>
struct std::basic_common_reference<::UserTuple<Ts...>, ::UserTuple<Us...>,
    TQual, UQual> :
    ::Tuple_helper<void, UserTuple<TQual<Ts>...>, UserTuple<UQual<Us>...>> {};

struct X2 {};
struct Y2 {};
struct Z2 {};

template <>
struct std::common_type<X2, Y2> {
    using type = Z2;
};
template <>
struct std::common_type<Y2, X2> {
    using type = Z2;
};

// (6.1)
//  -- If sizeof...(T) is zero, there shall be no member type.
static_assert(!has_type<rxx::common_reference<>>);

// (6.2)
//  -- Otherwise, if sizeof...(T) is one, let T0 denote the sole type in the
//     pack T. The member typedef type shall denote the same type as T0.
static_assert(std::is_same_v<rxx::common_reference_t<void>, void>);
static_assert(std::is_same_v<rxx::common_reference_t<int>, int>);
static_assert(std::is_same_v<rxx::common_reference_t<int&>, int&>);
static_assert(std::is_same_v<rxx::common_reference_t<int&&>, int&&>);
static_assert(std::is_same_v<rxx::common_reference_t<int const>, int const>);
static_assert(std::is_same_v<rxx::common_reference_t<int const&>, int const&>);
static_assert(
    std::is_same_v<rxx::common_reference_t<int const&&>, int const&&>);
static_assert(
    std::is_same_v<rxx::common_reference_t<int volatile[]>, int volatile[]>);
static_assert(std::is_same_v<rxx::common_reference_t<int volatile (&)[]>,
    int volatile (&)[]>);
static_assert(std::is_same_v<rxx::common_reference_t<int volatile (&&)[]>,
    int volatile (&&)[]>);
static_assert(std::is_same_v<rxx::common_reference_t<void (&)()>, void (&)()>);
static_assert(
    std::is_same_v<rxx::common_reference_t<void (&&)()>, void (&&)()>);

// (6.3)
//  -- Otherwise, if sizeof...(T) is two, let T1 and T2 denote the two types in
//     the pack T. Then
// (6.3.1)
//    -- Let R be COMMON-REF(T1, T2). If T1 and T2 are reference types, R is
//    well-formed,
//       and is_convertible_v<add_pointer_t<T1>, add_pointer_t<R>> &&
//       is_convertible_v<add_pointer_t<T2>, add_pointer_t<R>> is true, then the
//       member typedef type denotes R.

struct B {};
struct D : B {};
static_assert(std::is_same_v<rxx::common_reference_t<B&, D&>, B&>);
static_assert(std::is_same_v<rxx::common_reference_t<B const&, D&>, B const&>);
static_assert(std::is_same_v<rxx::common_reference_t<B&, D const&>, B const&>);
static_assert(
    std::is_same_v<rxx::common_reference_t<B&, D const&, D&>, B const&>);
static_assert(std::is_same_v<rxx::common_reference_t<B&, D&, B&, D&>, B&>);

static_assert(std::is_same_v<rxx::common_reference_t<B&&, D&&>, B&&>);
static_assert(
    std::is_same_v<rxx::common_reference_t<B const&&, D&&>, B const&&>);
static_assert(
    std::is_same_v<rxx::common_reference_t<B&&, D const&&>, B const&&>);
static_assert(std::is_same_v<rxx::common_reference_t<B&, D&&>, B const&>);
static_assert(std::is_same_v<rxx::common_reference_t<B&, D const&&>, B const&>);
static_assert(std::is_same_v<rxx::common_reference_t<B const&, D&&>, B const&>);

static_assert(std::is_same_v<rxx::common_reference_t<B&&, D&>, B const&>);
static_assert(std::is_same_v<rxx::common_reference_t<B&&, D const&>, B const&>);
static_assert(std::is_same_v<rxx::common_reference_t<B const&&, D&>, B const&>);

static_assert(std::is_same_v<rxx::common_reference_t<int const&, int volatile&>,
    int const volatile&>);
static_assert(std::is_same_v<
    rxx::common_reference_t<int const volatile&&, int volatile&&>,
    int const volatile&&>);

static_assert(std::is_same_v<rxx::common_reference_t<int (&)[10], int (&&)[10]>,
    int const (&)[10]>);
static_assert(std::is_same_v<
    rxx::common_reference_t<int const (&)[10], int volatile (&)[10]>,
    int const volatile (&)[10]>);

// when conversion from pointers are not true
struct E {};
struct F {
    operator E&() const;
};

static_assert(!std::is_convertible_v<F*, E*>);

// The following should not use 6.3.1, but fallback to 6.3.3
static_assert(std::is_same_v<rxx::common_reference_t<E&, F>, E&>);

// (6.3.2)
//    -- Otherwise, if basic_common_reference<remove_cvref_t<T1>,
//       remove_cvref_t<T2>, XREF(T1), XREF(T2)>::type is well-formed, then the
//       member typedef type denotes that type.
static_assert(
    std::is_same_v<rxx::common_reference_t<UserTuple<int, short> const&,
                       UserTuple<int&, short volatile&>>,
        UserTuple<int const&, short const volatile&>>);

static_assert(
    std::is_same_v<rxx::common_reference_t<UserTuple<int, short> volatile&,
                       UserTuple<int, short> const&>,
        UserTuple<int, short> const volatile&>);

// (6.3.3)
//    -- Otherwise, if COND_RES(T1, T2) is well-formed, then the member typedef
//       type denotes that type.
static_assert(std::is_same_v<rxx::common_reference_t<void, void>, void>);
static_assert(std::is_same_v<rxx::common_reference_t<int, short>, int>);
static_assert(std::is_same_v<rxx::common_reference_t<int, short&>, int>);
static_assert(std::is_same_v<rxx::common_reference_t<int&, short&>, int>);
static_assert(std::is_same_v<rxx::common_reference_t<int&, short>, int>);

// tricky volatile reference case
static_assert(
    std::is_same_v<rxx::common_reference_t<int&&, int volatile&>, int>);
static_assert(
    std::is_same_v<rxx::common_reference_t<int volatile&, int&&>, int>);

static_assert(
    std::is_same_v<rxx::common_reference_t<int (&)[10], int (&)[11]>, int*>);

// https://github.com/ericniebler/stl2/issues/338
struct MyIntRef {
    MyIntRef(int&);
};
static_assert(
    std::is_same_v<rxx::common_reference_t<int&, MyIntRef>, MyIntRef>);

// (6.3.4)
//    -- Otherwise, if common_type_t<T1, T2> is well-formed, then the member
//       typedef type denotes that type.
struct moveonly {
    moveonly() = default;
    moveonly(moveonly&&) = default;
    moveonly& operator=(moveonly&&) = default;
};
struct moveonly2 : moveonly {};

static_assert(std::is_same_v<rxx::common_reference_t<moveonly const&, moveonly>,
    moveonly>);
static_assert(
    std::is_same_v<rxx::common_reference_t<moveonly2 const&, moveonly>,
        moveonly>);
static_assert(
    std::is_same_v<rxx::common_reference_t<moveonly const&, moveonly2>,
        moveonly>);

static_assert(std::is_same_v<rxx::common_reference_t<X2&, Y2 const&>, Z2>);

// (6.3.5)
//    -- Otherwise, there shall be no member type.
static_assert(!has_type<rxx::common_reference<UserTuple<short> volatile&,
                  UserTuple<int, short> const&>>);

// (6.4)
//  -- Otherwise, if sizeof...(T) is greater than two, let T1, T2, and Rest,
//     respectively, denote the first, second, and (pack of) remaining types
//     comprising T. Let C be the type common_reference_t<T1, T2>. Then:
// (6.4.1)
//    -- If there is such a type C, the member typedef type shall denote the
//       same type, if any, as common_reference_t<C, Rest...>.
static_assert(std::is_same_v<rxx::common_reference_t<int, int, int>, int>);
static_assert(
    std::is_same_v<rxx::common_reference_t<int&&, int const&, int volatile&>,
        int const volatile&>);
static_assert(
    std::is_same_v<rxx::common_reference_t<int&&, int const&, float&>, float>);

// (6.4.2)
//    -- Otherwise, there shall be no member type.
static_assert(!has_type<rxx::common_reference<int, short, int, char*>>);

#if RXX_CXX23
static_assert(std::is_same_v<rxx::common_reference_t<std::tuple<int, int>>,
    std::tuple<int, int>>);
static_assert(std::is_same_v<
    rxx::common_reference_t<std::tuple<int, long>, std::tuple<long, int>>,
    std::tuple<long, long>>);
static_assert(
    std::is_same_v<rxx::common_reference_t<std::tuple<int&, int const&>,
                       std::tuple<int const&, int>>,
        std::tuple<int const&, int>>);
static_assert(
    std::is_same_v<rxx::common_reference_t<std::tuple<int&, int volatile&>,
                       std::tuple<int volatile&, int>>,
        std::tuple<int volatile&, int>>);
static_assert(std::is_same_v<
    rxx::common_reference_t<std::tuple<int&, int const volatile&>,
        std::tuple<int const volatile&, int>>,
    std::tuple<int const volatile&, int>>);
static_assert(
    !has_type<rxx::common_reference_t<std::tuple<int const&, int volatile&>,
        std::tuple<int volatile&, int const&>>>);

static_assert(std::is_same_v<
    rxx::common_reference_t<std::tuple<int, X2>, std::tuple<int, Y2>>,
    std::tuple<int, Z2>>);
static_assert(std::is_same_v<
    rxx::common_reference_t<std::tuple<int, X2>, std::tuple<int, Y2>>,
    std::tuple<int, Z2>>);
static_assert(!has_type<rxx::common_reference<std::tuple<int, const X2>,
                  std::tuple<float, const Z2>>>);
static_assert(
    !has_type<
        rxx::common_reference<std::tuple<int, X2>, std::tuple<float, Z2>>>);
static_assert(!has_type<rxx::common_reference<std::tuple<int, X2>, int, X2>>);
#endif
// rxx::tuple

static_assert(std::is_same_v<rxx::common_reference_t<rxx::tuple<int, int>>,
    rxx::tuple<int, int>>);
static_assert(std::is_same_v<
    rxx::common_reference_t<rxx::tuple<int, long>, rxx::tuple<long, int>>,
    rxx::tuple<long, long>>);
static_assert(
    std::is_same_v<rxx::common_reference_t<rxx::tuple<int&, int const&>,
                       rxx::tuple<int const&, int>>,
        rxx::tuple<int const&, int>>);
static_assert(
    std::is_same_v<rxx::common_reference_t<rxx::tuple<int&, int volatile&>,
                       rxx::tuple<int volatile&, int>>,
        rxx::tuple<int volatile&, int>>);
static_assert(std::is_same_v<
    rxx::common_reference_t<rxx::tuple<int&, int const volatile&>,
        rxx::tuple<int const volatile&, int>>,
    rxx::tuple<int const volatile&, int>>);
static_assert(
    !has_type<rxx::common_reference_t<rxx::tuple<int const&, int volatile&>,
        rxx::tuple<int volatile&, int const&>>>);

static_assert(std::is_same_v<
    rxx::common_reference_t<rxx::tuple<int, X2>, rxx::tuple<int, Y2>>,
    rxx::tuple<int, Z2>>);
static_assert(std::is_same_v<
    rxx::common_reference_t<rxx::tuple<int, X2>, rxx::tuple<int, Y2>>,
    rxx::tuple<int, Z2>>);
static_assert(!has_type<rxx::common_reference<rxx::tuple<int, const X2>,
                  rxx::tuple<float, const Z2>>>);
static_assert(
    !has_type<
        rxx::common_reference<rxx::tuple<int, X2>, rxx::tuple<float, Z2>>>);
static_assert(!has_type<rxx::common_reference<rxx::tuple<int, X2>, int, X2>>);

//

struct A {};
template <template <class> class TQual, template <class> class UQual>
struct std::basic_common_reference<A, rxx::tuple<B>, TQual, UQual> {
    using type = rxx::tuple<UQual<B>>;
};

#if RXX_CXX23
template <template <class> class TQual, template <class> class UQual>
struct std::basic_common_reference<A, std::tuple<B>, TQual, UQual> {
    using type = std::tuple<UQual<B>>;
};

static_assert(
    std::is_same_v<rxx::common_reference_t<A, std::tuple<B>, std::tuple<D>>,
        std::tuple<B>>);
#endif

static_assert(std::is_same_v<rxx::common_reference_t<std::pair<int, int>>,
    std::pair<int, int>>);
static_assert(std::is_same_v<
    rxx::common_reference_t<std::pair<int, long>, std::pair<long, int>>,
    std::pair<long, long>>);
static_assert(
    std::is_same_v<rxx::common_reference_t<std::pair<int&, int const&>,
                       std::pair<int const&, int>>,
        std::pair<int const&, int>>);
static_assert(
    std::is_same_v<rxx::common_reference_t<std::pair<int&, int volatile&>,
                       std::pair<int volatile&, int>>,
        std::pair<int volatile&, int>>);
static_assert(
    std::is_same_v<rxx::common_reference_t<std::pair<int&, int const volatile&>,
                       std::pair<int const volatile&, int>>,
        std::pair<int const volatile&, int>>);
static_assert(
    !has_type<rxx::common_reference_t<std::pair<int const&, int volatile&>,
        std::pair<int volatile&, int const&>>>);

static_assert(std::is_same_v<
    rxx::common_reference_t<std::pair<int, X2>, std::pair<int, Y2>>,
    std::pair<int, Z2>>);
static_assert(std::is_same_v<
    rxx::common_reference_t<std::pair<int, X2>, std::pair<int, Y2>>,
    std::pair<int, Z2>>);
static_assert(!has_type<rxx::common_reference<std::pair<int, const X2>,
                  std::pair<float, const Z2>>>);
static_assert(
    !has_type<rxx::common_reference<std::pair<int, X2>, std::pair<float, Z2>>>);
static_assert(!has_type<rxx::common_reference<std::pair<int, X2>, int, X2>>);
