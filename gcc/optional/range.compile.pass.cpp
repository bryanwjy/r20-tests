// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional/fwd.h"

#include "rxx/optional.h"
#include "rxx/ranges.h"

#include <cassert>
#include <concepts>
#include <type_traits>
#if RXX_SUPPORTS_RANGE_FORMAT
#  include <format> // IWYU pragma: keep
#endif
#include <iterator>
#include <string_view>
#include <vector>

namespace xranges = __RXX ranges;
namespace xviews = __RXX views;

struct NonMovable {
    constexpr NonMovable() {}
    constexpr NonMovable(int) {}

    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;

    friend bool operator==(NonMovable const&, NonMovable const&) = default;
};

struct NonAssignable {
    NonAssignable() = default;
    NonAssignable(NonAssignable&&) = default;
    NonAssignable& operator=(NonAssignable&&) = delete;

    friend bool operator==(
        NonAssignable const&, NonAssignable const&) = default;
};

template <typename T>
constexpr void test_range_concepts() {
    using O = __RXX optional<T>;
    static_assert(xranges::contiguous_range<O>);
    static_assert(xranges::sized_range<O>);
    static_assert(xranges::common_range<O>);

#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    // an optional<T&> is borrowed range
    constexpr bool is_ref_opt = std::is_reference_v<T>;
    static_assert(xranges::borrowed_range<O> == is_ref_opt);
#endif

    // for any T (including const U) such that optional<T> is not assignable,
    // it does not satisfy ranges::view
    constexpr bool is_opt_view = std::is_reference_v<T> || std::movable<T>;
    static_assert(xranges::view<O> == is_opt_view);
    static_assert(xranges::viewable_range<O> == is_opt_view);
}

template <typename O>
constexpr void test_iterator_concepts() {
    using T = typename O::value_type;
    using iterator = typename O::iterator;
    static_assert(std::contiguous_iterator<iterator>);
    static_assert(
        std::is_same_v<typename std::iterator_traits<iterator>::value_type,
            std::remove_cv_t<T>>);
    static_assert(
        std::is_same_v<std::iter_value_t<iterator>, std::remove_cv_t<T>>);
    static_assert(
        std::is_same_v<typename std::iterator_traits<iterator>::reference, T&>);
    static_assert(std::is_same_v<std::iter_reference_t<iterator>, T&>);
}

template <typename O>
constexpr void test_const_iterator_concepts() {
    using T = typename O::value_type;
    using const_iterator = typename O::const_iterator;
    static_assert(std::contiguous_iterator<const_iterator>);
    static_assert(std::is_same_v<
        typename std::iterator_traits<const_iterator>::value_type,
        std::remove_cv_t<T>>);
    static_assert(
        std::is_same_v<std::iter_value_t<const_iterator>, std::remove_cv_t<T>>);
    static_assert(
        std::is_same_v<typename std::iterator_traits<const_iterator>::reference,
            T const&>);
    static_assert(
        std::is_same_v<std::iter_reference_t<const_iterator>, T const&>);
}

template <typename T>
constexpr void test_empty() {
    using O = __RXX optional<T>;
    O empty;
    assert(!empty);
    assert(empty.begin() == empty.end());
    assert(std::as_const(empty).begin() == std::as_const(empty).end());
    assert(xranges::empty(empty));
    assert(xranges::empty(std::as_const(empty)));
    assert(xranges::empty(empty | xviews::as_const));
    assert(xranges::size(empty) == 0);
    assert(xranges::size(std::as_const(empty)) == 0);

    size_t count = 0;
    for (auto const& x : empty)
        ++count;
    assert(count == 0);
}

template <typename T>
constexpr void test_non_empty(T const& value) {
    using O = __RXX optional<T>;
    using V = typename O::value_type;
    O non_empty(std::in_place, value);
    assert(non_empty);
    if constexpr (!std::is_array_v<V>)
        assert(*non_empty == value);

    assert(non_empty.begin() != non_empty.end());
    assert(non_empty.begin() < non_empty.end());
    assert(std::as_const(non_empty).begin() != std::as_const(non_empty).end());
    assert(std::as_const(non_empty).begin() < std::as_const(non_empty).end());
    assert(!xranges::empty(non_empty));
    assert(!xranges::empty(std::as_const(non_empty)));
    assert(!xranges::empty(non_empty | xviews::as_const));
    assert(xranges::size(non_empty) == 1);
    assert(xranges::size(std::as_const(non_empty)) == 1);

    size_t count = 0;
    for (auto const& x : non_empty)
        ++count;
    assert(count == 1);

    if constexpr (std::is_move_assignable_v<V>) {
        for (auto& x : non_empty)
            x = V{};
        assert(non_empty);
        assert(*non_empty == V{});
    }
}

template <typename T>
constexpr void test(T const& value) {
    using O = __RXX optional<T>;
    test_range_concepts<T>();
    test_iterator_concepts<O>();
    if constexpr (!std::is_reference_v<T>)
        test_const_iterator_concepts<O>();
    test_empty<T>();
    test_non_empty<T>(value);
#if RXX_SUPPORTS_RANGE_FORMAT
    static_assert(!std::formattable<O, char>);
    static_assert(!std::formattable<O, wchar_t>);
    static_assert(std::format_kind<O> == std::range_format::disabled);
#endif
}

constexpr void range_chain_example() // from P3168
{
    std::vector<int> v{2, 3, 4, 5, 6, 7, 8, 9, 1};
    auto test = [](int i) -> __RXX optional<int> {
        switch (i) {
        case 1:
        case 3:
        case 7:
        case 9:
            return i * 2;
        default:
            return {};
        }
    };

    auto result = v | xviews::transform(test) |
        xviews::filter([](auto x) { return bool(x); }) |
        xviews::transform([](auto x) { return *x; }) |
        xranges::to<std::vector>();

    bool ok = result == std::vector<int>{6, 14, 18, 2};
    assert(ok);
}

template <typename T>
constexpr void test_not_range() {
    static_assert(!requires { typename __RXX optional<T>::iterator; });
    static_assert(!requires(__RXX optional<T> o) { o.begin(); });
    static_assert(!requires(__RXX optional<T> o) { o.end(); });
};

template <typename T>
constexpr bool is_optional = false;

template <typename T>
constexpr bool is_optional<__RXX optional<T>> = true;

template <bool usesOptional, typename T, typename U = std::remove_cv_t<T>>
constexpr void test_as_const(std::type_identity_t<U> u) {
    __RXX optional<T> o(std::in_place, std::forward<U>(u));
    auto cv = xviews::as_const(o);
    static_assert(is_optional<decltype(cv)> == usesOptional);
    static_assert(std::is_same_v<decltype(*cv.begin()),
        std::remove_reference_t<T> const&>);
    assert(!xranges::empty(cv));

    __RXX optional<T> e;
    auto cve = xviews::as_const(e);
    static_assert(is_optional<decltype(cve)> == usesOptional);
    static_assert(std::is_same_v<decltype(*cve.begin()),
        std::remove_reference_t<T> const&>);
    assert(xranges::empty(cve));
}

template <bool usesOptional, typename T, typename U = std::remove_cv_t<T>>
constexpr void test_reverse(std::type_identity_t<U> u) {
    __RXX optional<T> o(std::in_place, std::forward<U>(u));
    auto rv = xviews::reverse(o);
    static_assert(is_optional<decltype(rv)> == usesOptional);
    static_assert(std::is_same_v<decltype(*rv.begin()), T&>);
    assert(!xranges::empty(rv));

    __RXX optional<T> e;
    auto rve = xviews::reverse(e);
    static_assert(is_optional<decltype(rve)> == usesOptional);
    static_assert(std::is_same_v<decltype(*rve.begin()), T&>);
    assert(xranges::empty(rve));
}

template <bool usesOptional, typename T, typename U = std::remove_cv_t<T>>
constexpr void test_take(std::type_identity_t<U> u) {
    __RXX optional<T> o(std::in_place, std::forward<U>(u));
    auto tvp = xviews::take(o, 3);
    static_assert(is_optional<decltype(tvp)> == usesOptional);
    static_assert(std::is_same_v<decltype(*tvp.begin()), T&>);
    assert(!xranges::empty(tvp));

    auto tvz = xviews::take(o, 0);
    static_assert(is_optional<decltype(tvz)> == usesOptional);
    static_assert(std::is_same_v<decltype(*tvz.begin()), T&>);
    assert(xranges::empty(tvz));

    __RXX optional<T> e;
    auto tvep = xviews::take(e, 5);
    static_assert(is_optional<decltype(tvep)> == usesOptional);
    static_assert(std::is_same_v<decltype(*tvep.begin()), T&>);
    assert(xranges::empty(tvep));

    auto tvez = xviews::take(e, 0);
    static_assert(is_optional<decltype(tvez)> == usesOptional);
    static_assert(std::is_same_v<decltype(*tvez.begin()), T&>);
    assert(xranges::empty(tvez));
}

template <bool usesOptional, typename T, typename U = std::remove_cv_t<T>>
constexpr void test_drop(std::type_identity_t<U> u) {
    __RXX optional<T> o(std::in_place, std::forward<U>(u));
    auto dvp = xviews::drop(o, 3);
    static_assert(is_optional<decltype(dvp)> == usesOptional);
    static_assert(std::is_same_v<decltype(*dvp.begin()), T&>);
    assert(xranges::empty(dvp));

    auto dvz = xviews::drop(o, 0);
    static_assert(is_optional<decltype(dvz)> == usesOptional);
    static_assert(std::is_same_v<decltype(*dvz.begin()), T&>);
    assert(!xranges::empty(dvz));

    __RXX optional<T> e;
    auto dvep = xviews::drop(e, 5);
    static_assert(is_optional<decltype(dvep)> == usesOptional);
    static_assert(std::is_same_v<decltype(*dvep.begin()), T&>);
    assert(xranges::empty(dvep));

    auto dvez = xviews::drop(e, 0);
    static_assert(is_optional<decltype(dvez)> == usesOptional);
    static_assert(std::is_same_v<decltype(*dvez.begin()), T&>);
    assert(xranges::empty(dvez));
}

constexpr bool all_tests() {
    test(42);
    int i = 42;
    int arr[10]{};
    NonMovable nm;
    NonAssignable na;
    test(&i);
    test(std::string_view("test"));
    test(std::vector<int>{1, 2, 3, 4});
    test(__RXX optional<int>(42));
    test<int const>(42);
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    test<int&>(i);
    test<int const&>(i);
    test<int (&)[10]>(arr);
    test<int const(&)[10]>(arr);
    test<NonMovable&>(nm);
    test<NonMovable const&>(nm);
    test<NonAssignable&>(na);
    test<NonAssignable const&>(na);
    test_not_range<void (&)()>();
    test_not_range<void (&)(int)>();
    test_not_range<int (&)[]>();
    test_not_range<int const(&)[]>();
#endif

    range_chain_example();

    test_as_const<false, int>(i);
    test_as_const<false, int const>(i);
    test_as_const<false, NonMovable, int>(10);
    test_as_const<false, NonMovable const, int>(10);
    test_as_const<false, NonAssignable>({});
    test_as_const<false, NonAssignable const>({});

#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    test_as_const<true, int&>(i);
    test_as_const<true, int const&>(i);
    test_as_const<true, NonMovable&>(nm);
    test_as_const<true, NonMovable const&>(nm);
    test_as_const<true, NonAssignable&>(na);
    test_as_const<true, NonAssignable const&>(na);
#endif

#if RXX_SUPPORTS_OPTIONAL_REFERENCES
#  define TEST_ADAPTOR(name)                         \
      test_##name<true, int>(i);                     \
      test_##name<false, const int>(i);              \
      test_##name<true, int&>(i);                    \
      test_##name<true, const int&>(i);              \
      test_##name<false, NonMovable, int>(10);       \
      test_##name<false, const NonMovable, int>(10); \
      test_##name<true, NonMovable&>(nm);            \
      test_##name<true, const NonMovable&>(nm);      \
      test_##name<false, NonAssignable>({});         \
      test_##name<false, const NonAssignable>({});   \
      test_##name<true, NonAssignable&>(na);         \
      test_##name<true, const NonAssignable&>(na)
#else
#  define TEST_ADAPTOR(name)                         \
      test_##name<true, int>(i);                     \
      test_##name<false, const int>(i);              \
      test_##name<false, NonMovable, int>(10);       \
      test_##name<false, const NonMovable, int>(10); \
      test_##name<false, NonAssignable>({});         \
      test_##name<false, const NonAssignable>({})
#endif

    TEST_ADAPTOR(reverse);
    TEST_ADAPTOR(take);
    TEST_ADAPTOR(drop);
#undef TEST_ADAPTOR
    return true;
}

static_assert(all_tests());
