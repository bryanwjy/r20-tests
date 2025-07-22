// Copyright 2025 Bryan Wong

#include "rxx/tuple.h"

#include <tuple>

#define STRIP(Value) std::remove_cvref_t<decltype(Value)>

static constexpr rxx::tuple<int, float> t(73, 0.2f);
static_assert(rxx::get<0>(t) == 73, "");
static_assert(rxx::get<int>(t) == 73, "");
static_assert(rxx::ranges::get_element<0>(t) == 73, "");

static_assert(std::tuple_size<STRIP(t)>::value == 2,
    "tuple_size should return the expected size");
static_assert(std::is_same<std::tuple_element_t<0, STRIP(t)>, int>::value,
    "tuple_element should return the indexed element");
static_assert(std::is_same<std::tuple_element_t<1, STRIP(t)>, float>::value,
    "tuple_element should return the indexed element");

template <typename T, size_t I>
concept gettable =
    requires(T&& val) { rxx::ranges::get_element<I>(std::forward<T>(val)); };

static_assert(!gettable<decltype((t)), 2>, "Should not be gettable");

static constexpr rxx::tuple<long long, double> u(t);
static constexpr rxx::tuple<int, short, char, long long> v(73, 2, 22, 2652547);
static constexpr rxx::tuple<int, short, char, long long, short> w(
    73, 2, 22, 2652547, 7231);
static constexpr rxx::tuple<int, float> t1(2, 0.354);
static constexpr rxx::tuple<long long, double> t2(2, 0.354);

static_assert(sizeof(decltype(rxx::tuple<int, float, long long, double>{
    std::declval<rxx::tuple<int const&, float const&, long long const&,
        double const&>>()})));
static constexpr rxx::tuple<int, float, long long, double> t3(
    rxx::tuple_cat(t1, t2));

static_assert(std::same_as<decltype(rxx::tuple_cat(t1, t2)),
    rxx::tuple<int, float, long long, double>>);

static_assert(
    rxx::get<char>(v) == 22, "Should be gettable by type if target exists");
static_assert(rxx::get<char>(w) == 22,
    "Should be gettable by type if target type is unique");

template <int>
struct f {};

static_assert(
    std::is_same_v<decltype(rxx::forward_as_tuple(f<0>{}, f<1>{}, f<2>{})),
        rxx::tuple<f<0>&&, f<1>&&, f<2>&&>>,
    "rxx::forward_as_tuple must be lvalue if lvalue, rvalue otherwise");

static_assert(std::is_same_v<decltype(rxx::forward_as_tuple(
                                 std::declval<f<0>&>(), f<1>{}, f<2>{})),
                  rxx::tuple<f<0>&, f<1>&&, f<2>&&>>,
    "rxx::forward_as_tuple must be lvalue if lvalue, rvalue otherwise");

static_assert(
    std::is_same_v<decltype(rxx::forward_as_tuple(std::declval<f<0> const&>(),
                       std::declval<f<0>&>(),
                       std::declval<f<0> const volatile&>())),
        rxx::tuple<f<0> const&, f<0>&, f<0> const volatile&>>,
    "rxx::forward_as_tuple must conserve cv qualifiers");

static_assert(
    std::is_same_v<decltype(rxx::forward_as_tuple(std::declval<f<0> const>(),
                       std::declval<f<0>>(),
                       std::declval<f<0> const volatile>())),
        rxx::tuple<f<0> const&&, f<0>&&, f<0> const volatile&&>>,
    "rxx::forward_as_tuple must conserve cv qualifiers");

//

static_assert(std::is_same_v<decltype(rxx::forward_as<std::tuple>(
                                 f<0>{}, f<1>{}, f<2>{})),
                  std::tuple<f<0>&&, f<1>&&, f<2>&&>>,
    "rxx::forward_as<std::tuple> must be lvalue if lvalue, rvalue otherwise");

static_assert(std::is_same_v<decltype(rxx::forward_as<std::tuple>(
                                 std::declval<f<0>&>(), f<1>{}, f<2>{})),
                  std::tuple<f<0>&, f<1>&&, f<2>&&>>,
    "rxx::forward_as<std::tuple> must be lvalue if lvalue, rvalue otherwise");

static_assert(
    std::is_same_v<decltype(rxx::forward_as<std::tuple>(
                       std::declval<f<0> const&>(), std::declval<f<0>&>(),
                       std::declval<f<0> const volatile&>())),
        std::tuple<f<0> const&, f<0>&, f<0> const volatile&>>,
    "rxx::forward_as<std::tuple> must conserve cv qualifiers");

static_assert(
    std::is_same_v<decltype(rxx::forward_as<std::tuple>(
                       std::declval<f<0> const>(), std::declval<f<0>>(),
                       std::declval<f<0> const volatile>())),
        std::tuple<f<0> const&&, f<0>&&, f<0> const volatile&&>>,
    "rxx::forward_as<std::tuple> must conserve cv qualifiers");

struct explicit_obj {
    explicit explicit_obj() noexcept = default;
};
struct implicit_obj {
    implicit_obj() noexcept = default;
};

static_assert(
    !rxx::is_explicit_constructible_v<rxx::tuple<implicit_obj, int, float>>,
    "tuples containing non explicit constructible elements should not be "
    "explicitly constructible");
static_assert(
    rxx::is_explicit_constructible_v<rxx::tuple<explicit_obj, int, float>>,
    "tuples containing non implicit constructible elements should not be "
    "implicitly constructible");

template <int>
struct move_only {
public:
    constexpr move_only() noexcept = default;
    constexpr move_only(move_only&&) noexcept = default;
    constexpr move_only& operator=(move_only&&) noexcept = default;
    constexpr move_only const& operator=(move_only&&) const noexcept {
        return *this;
    }

private:
    constexpr move_only(move_only const&) noexcept = default;
    constexpr move_only& operator=(move_only const&) noexcept = default;
};

static_assert(!std::is_copy_constructible_v<move_only<0>>,
    "move_only should be copy constructible");
static_assert(!std::is_copy_assignable_v<move_only<0>>,
    "move_only should be copy assignable");
static_assert(std::is_move_constructible_v<move_only<0>>,
    "move_only should be move constructible");
static_assert(std::is_move_assignable_v<move_only<0>>,
    "move_only should be move assignable");

template <int>
struct copy_only {
public:
    constexpr copy_only() noexcept = default;
    constexpr copy_only(copy_only const&) noexcept = default;
    constexpr copy_only& operator=(copy_only const&) noexcept = default;

    constexpr copy_only(copy_only&&) noexcept = delete;
    constexpr copy_only& operator=(copy_only&&) noexcept = delete;
};

static_assert(std::is_copy_constructible_v<copy_only<0>>,
    "copy_only should be copy constructible");
static_assert(std::is_copy_assignable_v<copy_only<0>>,
    "copy_only should be copy assignable");
static_assert(!std::is_move_constructible_v<copy_only<0>>,
    "copy_only should be move constructible");
static_assert(!std::is_move_assignable_v<copy_only<0>>,
    "copy_only should be move assignable");

static_assert(
    !std::is_copy_constructible_v<rxx::tuple<move_only<0>, copy_only<0>>>,
    "if non-copy_only element exists, tuple should not be copy_only");
static_assert(
    !std::is_copy_assignable_v<rxx::tuple<move_only<0>, copy_only<0>>>,
    "if non-copy_only element exists, tuple should not be copy_only");
static_assert(!std::is_copy_constructible_v<rxx::tuple<move_only<0>, int>>,
    "if non-copy_only element exists, tuple should not be copy_only");
static_assert(!std::is_copy_assignable_v<rxx::tuple<move_only<0>, int>>,
    "if non-copy_only element exists, tuple should not be copy_only");
static_assert(
    std::is_copy_constructible_v<rxx::tuple<copy_only<0>, copy_only<1>>>,
    "if all elements are copy_only, tuple should be copy_only");
static_assert(std::is_copy_assignable_v<rxx::tuple<copy_only<0>, copy_only<1>>>,
    "if all elements are copy_only, tuple should be copy_only");

static_assert(std::is_move_constructible_v<rxx::tuple<int, int>>,
    "fundamental tuple is move constructible");

static_assert(!std::is_constructible_v<rxx::tuple<move_only<0>, move_only<1>>,
                  rxx::tuple<move_only<0>&&, move_only<1>&&> const&>,
    "Move only tuple should not be constructible with an lvalue tuple of "
    "rvalue references of the "
    "same element types");
static_assert(std::is_constructible_v<rxx::tuple<move_only<0>, move_only<1>>,
                  rxx::tuple<move_only<0>&&, move_only<1>&&>>,
    "Move only tuple should be constructible with an rvalue tuple of rvalue "
    "references of the same "
    "element types");

static_assert(std::is_constructible_v<rxx::tuple<copy_only<0>, copy_only<1>>,
                  rxx::tuple<copy_only<0>&&, copy_only<1>&&> const&>,
    "Copy only tuple should be constructible with an lvalue tuple of rvalue "
    "references of the same "
    "element types");

static_assert(
    std::is_move_constructible_v<rxx::tuple<copy_only<0>, copy_only<1>>>,
    "if non-move_only element exists, tuple should still be movable");

static_assert(
    std::is_move_constructible_v<rxx::tuple<move_only<0>, copy_only<0>>>,
    "if non-move_only element exists, tuple should still be movable");
static_assert(std::is_move_assignable_v<rxx::tuple<move_only<0>, copy_only<0>>>,
    "if non-move_only element exists, tuple should still be movable");
static_assert(std::is_move_assignable_v<rxx::tuple<copy_only<0>, copy_only<1>>>,
    "if non-move_only element exists, tuple should still be movable");

static_assert(
    std::is_move_constructible_v<rxx::tuple<move_only<0>, move_only<1>>>,
    "if all elements are move_only, tuple should be move_only");
static_assert(std::is_move_assignable_v<rxx::tuple<move_only<0>, move_only<1>>>,
    "if all elements are move_only, tuple should be move_only");

template <int J>
struct defaultable {
    constexpr defaultable() noexcept = default;
    template <int I>
    requires (I != J)
    constexpr defaultable(defaultable<I>) noexcept {}
};

static_assert(
    std::is_default_constructible_v<rxx::tuple<defaultable<0>, int, float>>,
    "If all element are default constructible, tuple is default constructible");

static_assert(std::is_constructible_v<rxx::tuple<int, defaultable<0>, float>,
                  int, defaultable<0>, float>,
    "If all element are default constructible, tuple is default constructible");

static_assert(std::is_nothrow_default_constructible_v<
                  rxx::tuple<defaultable<0>, defaultable<1>, defaultable<3>>>,
    "If all element are nothrow constructible, tuple is nothrow constructible");

static_assert(std::is_nothrow_constructible_v<rxx::tuple<long long, double>,
                  rxx::tuple<int, float>>,
    "If all element are nothrow constructible, tuple is nothrow constructible");

static_assert(std::is_constructible_v<rxx::tuple<long long, double>,
                  rxx::tuple<int, float>&>,
    "If all element are constructible, tuple is constructible from reference");
static_assert(std::is_constructible_v<rxx::tuple<long long, double>,
                  rxx::tuple<int, float> const&>,
    "If all element are constructible, tuple is constructible from const "
    "reference");

static_assert(
    std::is_nothrow_constructible_v<rxx::tuple<long long, double>, int, float>,
    "If all element are nothrow constructible, tuple is nothrow constructible");

static_assert(std::is_nothrow_constructible_v<
                  rxx::tuple<defaultable<0>, defaultable<1>, defaultable<3>>,
                  defaultable<2>, defaultable<3>, defaultable<1>>,
    "If all element are nothrow constructible, tuple is nothrow constructible");

static_assert(std::is_constructible_v<defaultable<0>, defaultable<2>>,
    "If all element are nothrow constructible, tuple is nothrow constructible");

static_assert(std::is_nothrow_copy_constructible_v<
                  rxx::tuple<defaultable<0>, defaultable<1>, defaultable<3>>>,
    "If all element are nothrow constructible, tuple is nothrow constructible");

static_assert(std::is_nothrow_move_constructible_v<
                  rxx::tuple<defaultable<0>, defaultable<1>, defaultable<3>>>,
    "If all element are nothrow constructible, tuple is nothrow constructible");

static_assert(std::is_nothrow_copy_assignable_v<
                  rxx::tuple<defaultable<0>, defaultable<1>, defaultable<3>>>,
    "If all element are nothrow assignable, tuple is nothrow assignable");

static_assert(std::is_nothrow_move_assignable_v<
                  rxx::tuple<defaultable<0>, defaultable<1>, defaultable<3>>>,
    "If all element are nothrow assignable, tuple is nothrow assignable");

template <int>
struct non_defaultable {
    non_defaultable() = delete;
    non_defaultable(int) {}
};

static_assert(!std::is_default_constructible_v<
                  rxx::tuple<non_defaultable<0>, int, float>>,
    "If non-default constructible element exists, tuple is not default "
    "constructible");
