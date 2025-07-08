// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/to_input_view.h"

#include "test_iterators.h"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <vector>

namespace ranges = std::ranges;
namespace views = std::views;

void test01() {
    std::vector<int> r{1, 2, 3};
    auto v = r | rxx::views::to_input;
    using type = decltype(v);
    static_assert(ranges::input_range<type> && !ranges::forward_range<type>);

    assert(ranges::equal(v.base(), r));
    assert(v.size() == r.size());
    assert(v.end() == r.end());
    auto it = v.begin();
    assert(it != r.end());
    *it = 42;
    ++it;
    *it = 43;
    it++;
    ranges::iter_swap(v.begin(), it);
    assert(ranges::equal(r, (int[]){3, 43, 42}));
    *it = ranges::iter_move(it);
    assert(it == r.begin() + 2);
    assert(r.end() - it == 1);
    assert(it - r.end() == -1);
}

void test02() {
    int x[] = {1, 2, 3};
    rxx::tests::test_input_range<int> rx(x);
    static_assert(!ranges::common_range<decltype(rx)>);
    auto v = rx | rxx::views::to_input;
    static_assert(std::same_as<decltype(v), decltype(views::all(rx))>);
    static_assert(std::same_as<decltype(x | rxx::views::to_input),
        decltype(x | rxx::views::to_input | rxx::views::to_input)>);
}

int main() {
    test01();
    test02();
}
