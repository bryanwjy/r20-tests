// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

#pragma once

#define ASSERT_SAME_TYPE(...) static_assert(std::same_as<__VA_ARGS__>)

#define ASSERT_NOEXCEPT(...) static_assert(noexcept(__VA_ARGS__));

#define ASSERT_NOT_NOEXCEPT(...) static_assert(!noexcept(__VA_ARGS__));
