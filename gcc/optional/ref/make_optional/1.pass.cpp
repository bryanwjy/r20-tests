// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>
#include <utility>

struct NonTrivial {
    constexpr NonTrivial() {}
    constexpr NonTrivial(NonTrivial const&) {};
    constexpr ~NonTrivial() {};
};

template <typename T>
struct Conv {
    T t;

    constexpr operator T() const noexcept { return t; }
};

constexpr bool test() {
    NonTrivial t;
    NonTrivial const& ct = t;

#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    auto o1 = __RXX make_optional<NonTrivial&>(t);
    assert(o1.has_value());
    assert(&o1.value() == &t);

    auto o2 = __RXX make_optional<NonTrivial const&>(t);
    assert(o2.has_value());
    assert(&o2.value() == &t);

    auto o3 = __RXX make_optional<NonTrivial const&>(ct);
    assert(o3.has_value());
    assert(&o3.value() == &t);

    Conv<NonTrivial&> rw(t);
    auto o4 = __RXX make_optional<NonTrivial&>(rw);
    assert(o4.has_value());
    assert(&o4.value() == &t);

    auto o5 = __RXX make_optional<NonTrivial&>(std::as_const(rw));
    assert(o5.has_value());
    assert(&o5.value() == &t);

    auto o6 = __RXX make_optional<NonTrivial&>(Conv<NonTrivial&>(t));
    assert(o6.has_value());
    assert(&o6.value() == &t);
#endif

    return true;
}

int main() {
    test();
    static_assert(test());
}
