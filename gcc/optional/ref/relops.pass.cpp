// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>
#include <functional>
#include <type_traits>

template <typename T, typename H = std::hash<T>>
constexpr bool has_disabled_hash = !std::is_default_constructible_v<H> &&
    !std::is_copy_constructible_v<H> && !std::is_move_constructible_v<H> &&
    !std::is_copy_assignable_v<H> && !std::is_move_assignable_v<H>;

static_assert(has_disabled_hash<__RXX optional<int&>>);
static_assert(has_disabled_hash<__RXX optional<int const&>>);

template <typename T, typename V>
constexpr void test_compare_val(V& l, V& h) {
    __RXX optional<T> t;

    assert(!(t == l));
    assert((t != l));
    assert((t < l));
    assert((t <= l));
    assert(!(t > l));
    assert(!(t >= l));
    assert((t <=> l) < 0);

    assert(!(l == t));
    assert((l != t));
    assert(!(l < t));
    assert(!(l <= t));
    assert((l > t));
    assert((l >= t));
    assert((l <=> t) > 0);

    t.emplace(l);
    assert((t == l));
    assert(!(t != l));
    assert(!(t < l));
    assert((t <= l));
    assert(!(t > l));
    assert((t >= l));
    assert((t <=> l) == 0);

    assert((l == t));
    assert(!(l != t));
    assert(!(l < t));
    assert((l <= t));
    assert(!(l > t));
    assert((l >= t));
    assert((t <=> l) == 0);

    t.emplace(h);
    assert(!(t == l));
    assert((t != l));
    assert(!(t < l));
    assert(!(t <= l));
    assert((t > l));
    assert((t >= l));
    assert((t <=> l) > 0);

    assert(!(l == t));
    assert((l != t));
    assert((l < t));
    assert((l <= t));
    assert(!(l > t));
    assert(!(l >= t));
    assert((l <=> t) < 0);
}

template <typename T, typename U, typename V>
constexpr void test_compare_opts(V& l, V& h) {
    __RXX optional<T> t;
    __RXX optional<U> u;

    assert((t == u));
    assert(!(t != u));
    assert(!(t < u));
    assert((t <= u));
    assert(!(t > u));
    assert((t >= u));
    assert((t <=> u) == 0);

    t.emplace(l);
    assert(!(t == u));
    assert((t != u));
    assert(!(t < u));
    assert(!(t <= u));
    assert((t > u));
    assert((t >= u));
    assert((t <=> u) > 0);

    u.emplace(l);
    assert((t == u));
    assert(!(t != u));
    assert(!(t < u));
    assert((t <= u));
    assert(!(t > u));
    assert((t <= u));
    assert((t <=> u) == 0);

    u.emplace(h);
    assert(!(t == u));
    assert((t != u));
    assert((t < u));
    assert((t <= u));
    assert(!(t > u));
    assert(!(t >= u));
    assert((t <=> u) < 0);

    t.reset();
    u.emplace(l);
    assert(!(t == u));
    assert((t != u));
    assert((t < u));
    assert((t <= u));
    assert(!(t > u));
    assert(!(t >= u));
    assert((t <=> u) < 0);

    t.emplace(h);
    assert(!(t == u));
    assert((t != u));
    assert(!(t < u));
    assert(!(t <= u));
    assert((t > u));
    assert((t >= u));
    assert((t <=> u) > 0);
}

template <typename V>
constexpr void test_compare(V l, V h) {
    test_compare_val<V&>(l, h);
    test_compare_val<V const&>(l, h);

    test_compare_opts<V&, V&>(l, h);
    test_compare_opts<V, V&>(l, h);
    test_compare_opts<V&, V>(l, h);

    test_compare_opts<V const&, V const&>(l, h);
    test_compare_opts<V, V const&>(l, h);
    test_compare_opts<V const&, V>(l, h);

    test_compare_opts<V&, V const&>(l, h);
    test_compare_opts<V const&, V&>(l, h);
}

struct TreeWay {
    int v;
    friend auto operator<=>(TreeWay, TreeWay) = default;
};

struct Other {
    int v;

    constexpr Other(int p) : v(p) {}
    constexpr Other(TreeWay p) : v(p.v) {}

    friend bool operator==(Other, Other) = default;
    friend auto operator<=>(Other, Other) = default;

    friend constexpr bool operator==(Other const& lhs, TreeWay const& rhs) {
        return lhs.v == rhs.v;
    }

    friend constexpr std::strong_ordering operator<=>(
        Other const& lhs, TreeWay const& rhs) {
        return lhs.v <=> rhs.v;
    }
};

constexpr void test_heterogeneus_cmp() {
    TreeWay l{10};
    Other h{20};

    __RXX optional<TreeWay&> t;
    __RXX optional<Other const&> u;

    assert((t == u));
    assert(!(t != u));
    assert(!(t < u));
    assert((t <= u));
    assert(!(t > u));
    assert((t >= u));
    assert((t <=> u) == 0);

    t.emplace(l);
    assert(!(t == u));
    assert((t != u));
    assert(!(t < u));
    assert(!(t <= u));
    assert((t > u));
    assert((t >= u));
    assert((t <=> u) > 0);

    u.emplace(h);
    assert(!(t == u));
    assert((t != u));
    assert((t < u));
    assert((t <= u));
    assert(!(t > u));
    assert(!(t >= u));
    assert((t <=> u) < 0);
}

int main() {
    auto test_all = [] {
        test_compare(2, 5);
        test_compare(TreeWay{11}, TreeWay{12});
        test_heterogeneus_cmp();
        return true;
    };

    test_all();
    static_assert(test_all());
}
