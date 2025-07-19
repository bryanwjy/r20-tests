// { dg-do run { target c++23 } }

#include "rxx/numeric/iota.h"

#include "../test_iterators.h"
#include "rxx/algorithm/equal.h"

#include <cassert>

namespace xranges = rxx::ranges;

void test01() {
    int x[3] = {};
    rxx::tests::test_output_range<int> rx(x);
    auto r0 = xranges::iota(rx, 0);
    assert(r0.out.ptr == x + 3);
    assert(r0.value == 3);
    assert(xranges::equal(x, (int[]){0, 1, 2}));
    auto r1 = xranges::iota(x, x + 2, 5);
    assert(r1.out == x + 2);
    assert(r1.value == 7);
    assert(xranges::equal(x, (int[]){5, 6, 2}));
}

int main() {
    test01();
}
