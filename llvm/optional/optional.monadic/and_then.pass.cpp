// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// <optional>

// template<class F> constexpr auto and_then(F&&) &;
// template<class F> constexpr auto and_then(F&&) &&;
// template<class F> constexpr auto and_then(F&&) const&;
// template<class F> constexpr auto and_then(F&&) const&&;

#include "../../static_asserts.h"
#include "rxx/optional.h"

#include <cassert>
#include <concepts>

struct LVal {
    constexpr __RXX optional<int> operator()(int&) { return 1; }
    __RXX optional<int> operator()(int const&) = delete;
    __RXX optional<int> operator()(int&&) = delete;
    __RXX optional<int> operator()(int const&&) = delete;
};

struct CLVal {
    __RXX optional<int> operator()(int&) = delete;
    constexpr __RXX optional<int> operator()(int const&) { return 1; }
    __RXX optional<int> operator()(int&&) = delete;
    __RXX optional<int> operator()(int const&&) = delete;
};

struct RVal {
    __RXX optional<int> operator()(int&) = delete;
    __RXX optional<int> operator()(int const&) = delete;
    constexpr __RXX optional<int> operator()(int&&) { return 1; }
    __RXX optional<int> operator()(int const&&) = delete;
};

struct CRVal {
    __RXX optional<int> operator()(int&) = delete;
    __RXX optional<int> operator()(int const&) = delete;
    __RXX optional<int> operator()(int&&) = delete;
    constexpr __RXX optional<int> operator()(int const&&) { return 1; }
};

struct RefQual {
    constexpr __RXX optional<int> operator()(int) & { return 1; }
    __RXX optional<int> operator()(int) const& = delete;
    __RXX optional<int> operator()(int) && = delete;
    __RXX optional<int> operator()(int) const&& = delete;
};

struct CRefQual {
    __RXX optional<int> operator()(int) & = delete;
    constexpr __RXX optional<int> operator()(int) const& { return 1; }
    __RXX optional<int> operator()(int) && = delete;
    __RXX optional<int> operator()(int) const&& = delete;
};

struct RVRefQual {
    __RXX optional<int> operator()(int) & = delete;
    __RXX optional<int> operator()(int) const& = delete;
    constexpr __RXX optional<int> operator()(int) && { return 1; }
    __RXX optional<int> operator()(int) const&& = delete;
};

struct RVCRefQual {
    __RXX optional<int> operator()(int) & = delete;
    __RXX optional<int> operator()(int) const& = delete;
    __RXX optional<int> operator()(int) && = delete;
    constexpr __RXX optional<int> operator()(int) const&& { return 1; }
};

struct NOLVal {
    constexpr __RXX optional<int> operator()(int&) { return __RXX nullopt; }
    __RXX optional<int> operator()(int const&) = delete;
    __RXX optional<int> operator()(int&&) = delete;
    __RXX optional<int> operator()(int const&&) = delete;
};

struct NOCLVal {
    __RXX optional<int> operator()(int&) = delete;
    constexpr __RXX optional<int> operator()(int const&) {
        return __RXX nullopt;
    }
    __RXX optional<int> operator()(int&&) = delete;
    __RXX optional<int> operator()(int const&&) = delete;
};

struct NORVal {
    __RXX optional<int> operator()(int&) = delete;
    __RXX optional<int> operator()(int const&) = delete;
    constexpr __RXX optional<int> operator()(int&&) { return __RXX nullopt; }
    __RXX optional<int> operator()(int const&&) = delete;
};

struct NOCRVal {
    __RXX optional<int> operator()(int&) = delete;
    __RXX optional<int> operator()(int const&) = delete;
    __RXX optional<int> operator()(int&&) = delete;
    constexpr __RXX optional<int> operator()(int const&&) {
        return __RXX nullopt;
    }
};

struct NORefQual {
    constexpr __RXX optional<int> operator()(int) & { return __RXX nullopt; }
    __RXX optional<int> operator()(int) const& = delete;
    __RXX optional<int> operator()(int) && = delete;
    __RXX optional<int> operator()(int) const&& = delete;
};

struct NOCRefQual {
    __RXX optional<int> operator()(int) & = delete;
    constexpr __RXX optional<int> operator()(int) const& {
        return __RXX nullopt;
    }
    __RXX optional<int> operator()(int) && = delete;
    __RXX optional<int> operator()(int) const&& = delete;
};

struct NORVRefQual {
    __RXX optional<int> operator()(int) & = delete;
    __RXX optional<int> operator()(int) const& = delete;
    constexpr __RXX optional<int> operator()(int) && { return __RXX nullopt; }
    __RXX optional<int> operator()(int) const&& = delete;
};

struct NORVCRefQual {
    __RXX optional<int> operator()(int) & = delete;
    __RXX optional<int> operator()(int) const& = delete;
    __RXX optional<int> operator()(int) && = delete;
    constexpr __RXX optional<int> operator()(int) const&& {
        return __RXX nullopt;
    }
};

struct NoCopy {
    NoCopy() = default;
    NoCopy(NoCopy const&) { assert(false); }
    __RXX optional<int> operator()(NoCopy const&&) { return 1; }
};

struct NonConst {
    __RXX optional<int> non_const() { return 1; }
};

constexpr void test_val_types() {
    // Test & overload
    {
        // Without & qualifier on F's operator()
        {
            __RXX optional<int> i{0};
            assert(i.and_then(LVal{}) == 1);
            assert(i.and_then(NOLVal{}) == __RXX nullopt);
            ASSERT_SAME_TYPE(
                decltype(i.and_then(LVal{})), __RXX optional<int>);
        }

        // With & qualifier on F's operator()
        {
            __RXX optional<int> i{0};
            RefQual l{};
            assert(i.and_then(l) == 1);
            NORefQual nl{};
            assert(i.and_then(nl) == __RXX nullopt);
            ASSERT_SAME_TYPE(decltype(i.and_then(l)), __RXX optional<int>);
        }
    }

    // Test const& overload
    {
        // Without & qualifier on F's operator()
        {
            __RXX optional<int> const i{0};
            assert(i.and_then(CLVal{}) == 1);
            assert(i.and_then(NOCLVal{}) == __RXX nullopt);
            ASSERT_SAME_TYPE(
                decltype(i.and_then(CLVal{})), __RXX optional<int>);
        }

        // With & qualifier on F's operator()
        {
            __RXX optional<int> const i{0};
            CRefQual const l{};
            assert(i.and_then(l) == 1);
            NOCRefQual const nl{};
            assert(i.and_then(nl) == __RXX nullopt);
            ASSERT_SAME_TYPE(decltype(i.and_then(l)), __RXX optional<int>);
        }
    }

    // Test && overload
    {
        // Without & qualifier on F's operator()
        {
            __RXX optional<int> i{0};
            assert(std::move(i).and_then(RVal{}) == 1);
            assert(std::move(i).and_then(NORVal{}) == __RXX nullopt);
            ASSERT_SAME_TYPE(
                decltype(std::move(i).and_then(RVal{})), __RXX optional<int>);
        }

        // With & qualifier on F's operator()
        {
            __RXX optional<int> i{0};
            assert(i.and_then(RVRefQual{}) == 1);
            assert(i.and_then(NORVRefQual{}) == __RXX nullopt);
            ASSERT_SAME_TYPE(
                decltype(i.and_then(RVRefQual{})), __RXX optional<int>);
        }
    }

    // Test const&& overload
    {
        // Without & qualifier on F's operator()
        {
            __RXX optional<int> const i{0};
            assert(std::move(i).and_then(CRVal{}) == 1);
            assert(std::move(i).and_then(NOCRVal{}) == __RXX nullopt);
            ASSERT_SAME_TYPE(
                decltype(std::move(i).and_then(CRVal{})), __RXX optional<int>);
        }

        // With & qualifier on F's operator()
        {
            __RXX optional<int> const i{0};
            RVCRefQual const l{};
            assert(i.and_then(std::move(l)) == 1);
            NORVCRefQual const nl{};
            assert(i.and_then(std::move(nl)) == __RXX nullopt);
            ASSERT_SAME_TYPE(
                decltype(i.and_then(std::move(l))), __RXX optional<int>);
        }
    }
}

// check that the lambda body is not instantiated during overload resolution
constexpr void test_sfinae() {
    __RXX optional<NonConst> opt{};
    auto l = [](auto&& x) { return x.non_const(); };
    (void)opt.and_then(l);
    (void)std::move(opt).and_then(l);
}

constexpr bool test() {
    test_val_types();
    __RXX optional<int> opt{};
    auto const& copt = opt;

    auto const never_called = [](int) {
        assert(false);
        return __RXX optional<int>{};
    };

    (void)opt.and_then(never_called);
    (void)std::move(opt).and_then(never_called);
    (void)copt.and_then(never_called);
    (void)std::move(copt).and_then(never_called);

    __RXX optional<NoCopy> nc;
    auto const& cnc = nc;
    (void)std::move(cnc).and_then(NoCopy{});
    (void)std::move(nc).and_then(NoCopy{});

    return true;
}

#if RXX_SUPPORTS_OPTIONAL_REFERENCES
constexpr bool test_ref() {
    // Test & overload
    {
        // Without & qualifier on F's operator()
        {
            int j = 42;
            __RXX optional<int&> i{j};
            std::same_as<__RXX optional<int>> decltype(auto) r =
                i.and_then(LVal{});

            assert(r == 1);
            assert(i.and_then(NOLVal{}) == __RXX nullopt);
        }

        // With & qualifier on F's operator()
        {
            int j = 42;
            __RXX optional<int&> i{j};
            RefQual l{};
            NORefQual nl{};
            std::same_as<__RXX optional<int>> decltype(auto) r =
                i.and_then(l);

            assert(r == 1);
            assert(i.and_then(nl) == __RXX nullopt);
        }
    }

    // Test const& overload
    {
        // Without & qualifier on F's operator()
        {
            int j = 42;
            __RXX optional<int const&> i{j};
            std::same_as<__RXX optional<int>> decltype(auto) r =
                i.and_then(CLVal{});

            assert(r == 1);
            assert(i.and_then(NOCLVal{}) == __RXX nullopt);
        }

        // With & qualifier on F's operator()
        {
            int j = 42;
            __RXX optional<int&> const i{j};
            CRefQual const l{};
            NOCRefQual const nl{};
            std::same_as<__RXX optional<int>> decltype(auto) r =
                i.and_then(l);

            assert(r == 1);
            assert(i.and_then(nl) == __RXX nullopt);
        }
    }
    // Test && overload
    {
        // With & qualifier on F's operator()
        {
            int j = 42;
            __RXX optional<int&> i{j};
            std::same_as<__RXX optional<int>> decltype(auto) r =
                i.and_then(RVRefQual{});

            assert(r == 1);
            assert(i.and_then(NORVRefQual{}) == __RXX nullopt);
        }
    }

    // Test const&& overload
    {
        // With & qualifier on F's operator()
        {
            int j = 42;
            __RXX optional<int&> const i{j};
            RVCRefQual const l{};
            NORVCRefQual const nl{};
            std::same_as<__RXX optional<int>> decltype(auto) r =
                i.and_then(std::move(l));

            assert(r == 1);
            assert(i.and_then(std::move(nl)) == __RXX nullopt);
        }
    }
    return true;
}
#endif

int main(int, char**) {
    test();
    static_assert(test());
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    test_ref();
    static_assert(test_ref());
#endif
    return 0;
}
