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

// template<class F> constexpr auto transform(F&&) &;
// template<class F> constexpr auto transform(F&&) &&;
// template<class F> constexpr auto transform(F&&) const&;
// template<class F> constexpr auto transform(F&&) const&&;

#include "../../static_asserts.h"
#include "rxx/optional.h"

#include <cassert>
#include <concepts>
#include <utility>

struct LVal {
    constexpr int operator()(int&) { return 1; }
    int operator()(int const&) = delete;
    int operator()(int&&) = delete;
    int operator()(int const&&) = delete;
};

struct CLVal {
    int operator()(int&) = delete;
    constexpr int operator()(int const&) { return 1; }
    int operator()(int&&) = delete;
    int operator()(int const&&) = delete;
};

struct RVal {
    int operator()(int&) = delete;
    int operator()(int const&) = delete;
    constexpr int operator()(int&&) { return 1; }
    int operator()(int const&&) = delete;
};

struct CRVal {
    int operator()(int&) = delete;
    int operator()(int const&) = delete;
    int operator()(int&&) = delete;
    constexpr int operator()(int const&&) { return 1; }
};

struct RefQual {
    constexpr int operator()(int) & { return 1; }
    int operator()(int) const& = delete;
    int operator()(int) && = delete;
    int operator()(int) const&& = delete;
};

struct CRefQual {
    int operator()(int) & = delete;
    constexpr int operator()(int) const& { return 1; }
    int operator()(int) && = delete;
    int operator()(int) const&& = delete;
};

struct RVRefQual {
    int operator()(int) & = delete;
    int operator()(int) const& = delete;
    constexpr int operator()(int) && { return 1; }
    int operator()(int) const&& = delete;
};

struct RVCRefQual {
    int operator()(int) & = delete;
    int operator()(int) const& = delete;
    int operator()(int) && = delete;
    constexpr int operator()(int) const&& { return 1; }
};

struct NoCopy {
    NoCopy() = default;
    NoCopy(NoCopy const&) { assert(false); }
    int operator()(NoCopy const&&) { return 1; }
};

struct NoMove {
    NoMove() = default;
    NoMove(NoMove&&) = delete;
    NoMove operator()(NoCopy const&&) { return NoMove{}; }
};

constexpr void test_val_types() {
    // Test & overload
    {
        // Without & qualifier on F's operator()
        {
            __RXX optional<int> i{0};
            assert(i.transform(LVal{}) == 1);
            ASSERT_SAME_TYPE(
                decltype(i.transform(LVal{})), __RXX optional<int>);
        }

        // With & qualifier on F's operator()
        {
            __RXX optional<int> i{0};
            RefQual l{};
            assert(i.transform(l) == 1);
            ASSERT_SAME_TYPE(decltype(i.transform(l)), __RXX optional<int>);
        }
    }

    // Test const& overload
    {
        // Without & qualifier on F's operator()
        {
            __RXX optional<int> const i{0};
            assert(i.transform(CLVal{}) == 1);
            ASSERT_SAME_TYPE(
                decltype(i.transform(CLVal{})), __RXX optional<int>);
        }

        // With & qualifier on F's operator()
        {
            __RXX optional<int> const i{0};
            CRefQual const l{};
            assert(i.transform(l) == 1);
            ASSERT_SAME_TYPE(decltype(i.transform(l)), __RXX optional<int>);
        }
    }

    // Test && overload
    {
        // Without & qualifier on F's operator()
        {
            __RXX optional<int> i{0};
            assert(std::move(i).transform(RVal{}) == 1);
            ASSERT_SAME_TYPE(
                decltype(std::move(i).transform(RVal{})), __RXX optional<int>);
        }

        // With & qualifier on F's operator()
        {
            __RXX optional<int> i{0};
            assert(i.transform(RVRefQual{}) == 1);
            ASSERT_SAME_TYPE(
                decltype(i.transform(RVRefQual{})), __RXX optional<int>);
        }
    }

    // Test const&& overload
    {
        // Without & qualifier on F's operator()
        {
            __RXX optional<int> const i{0};
            assert(std::move(i).transform(CRVal{}) == 1);
            ASSERT_SAME_TYPE(decltype(std::move(i).transform(CRVal{})),
                __RXX optional<int>);
        }

        // With & qualifier on F's operator()
        {
            __RXX optional<int> const i{0};
            RVCRefQual const l{};
            assert(i.transform(std::move(l)) == 1);
            ASSERT_SAME_TYPE(
                decltype(i.transform(std::move(l))), __RXX optional<int>);
        }
    }
}

struct NonConst {
    int non_const() { return 1; }
};

// check that the lambda body is not instantiated during overload resolution
constexpr void test_sfinae() {
    __RXX optional<NonConst> opt{};
    auto l = [](auto&& x) { return x.non_const(); };
    (void)opt.transform(l);
    (void)std::move(opt).transform(l);
}

constexpr bool test() {
    test_sfinae();
    test_val_types();
    __RXX optional<int> opt;
    auto const& copt = opt;

    auto const never_called = [](int) {
        assert(false);
        return 0;
    };

    (void)opt.transform(never_called);
    (void)std::move(opt).transform(never_called);
    (void)copt.transform(never_called);
    (void)std::move(copt).transform(never_called);

    __RXX optional<NoCopy> nc;
    auto const& cnc = nc;
    (void)std::move(nc).transform(NoCopy{});
    (void)std::move(cnc).transform(NoCopy{});

    (void)std::move(nc).transform(NoMove{});
    (void)std::move(cnc).transform(NoMove{});

    return true;
}

#if RXX_SUPPORTS_OPTIONAL_REFERENCES
constexpr bool test_ref() {
    {
        __RXX optional<int&> opt1;
        std::same_as<__RXX optional<int>> decltype(auto) opt1r =
            opt1.transform([](int i) { return i + 2; });
        assert(!opt1);
        assert(!opt1r);
    }

    {
        int i = 42;
        __RXX optional<int&> opt{i};
        std::same_as<__RXX optional<int>> decltype(auto) o2 =
            opt.transform([](int j) { return j + 2; });

        assert(*o2 == 44);
    }
    // Test & overload
    {
        // Without & qualifier on F's operator()
        {
            int i = 42;
            __RXX optional<int&> opt{i};
            std::same_as<__RXX optional<int>> decltype(auto) o3 =
                opt.transform(LVal{});

            assert(*o3 == 1);
        }

        // With & qualifier on F's operator()
        {
            int i = 42;
            __RXX optional<int&> opt{i};
            RefQual l{};
            std::same_as<__RXX optional<int>> decltype(auto) o3 =
                opt.transform(l);

            assert(*o3 == 1);
        }
    }
    // const& overload
    {
        // Without & qualifier on F's operator()
        {
            int i = 42;
            __RXX optional<int const&> opt{i};
            std::same_as<__RXX optional<int>> decltype(auto) o3 =
                std::as_const(opt).transform(CLVal{});

            assert(*o3 == 1);
        }

        // With & qualifier on F's operator()
        {
            int i = 42;
            __RXX optional<int&> const opt{i};
            CRefQual const l{};
            std::same_as<__RXX optional<int>> decltype(auto) o3 =
                opt.transform(l);

            assert(*o3 == 1);
        }
    }

    // Test && overload
    {
        // Without & qualifier on F's operator()
        {
            int i = 42;
            __RXX optional<int> opt{i};
            std::same_as<__RXX optional<int>> decltype(auto) o3 =
                std::move(opt).transform(RVal{});

            assert(*o3 == 1);
        }

        // With & qualifier on F's operator()
        {
            int i = 42;
            __RXX optional<int&> opt{i};
            std::same_as<__RXX optional<int>> decltype(auto) o3 =
                std::move(opt).transform(RVRefQual{});
            assert(*o3 == 1);
        }
    }

    // const&& overload
    {
        // With & qualifier on F's operator()
        {
            int i = 42;
            __RXX optional<int&> opt{i};
            RVCRefQual const rvc{};
            std::same_as<__RXX optional<int>> decltype(auto) o3 =
                opt.transform(std::move(rvc));
            assert(*o3 == 1);
        }
    }
    {
        __RXX optional<int&> o6 = __RXX nullopt;
        auto o6r = o6.transform([](int) { return 42; });
        assert(!o6r);
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
