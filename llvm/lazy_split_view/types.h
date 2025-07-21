// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef TEST_STD_RANGES_RANGE_ADAPTORS_RANGE_LAZY_SPLIT_TYPES_H
#define TEST_STD_RANGES_RANGE_ADAPTORS_RANGE_LAZY_SPLIT_TYPES_H

#include "../test_iterators.h"
#include "rxx/ranges.h"

#include <concepts>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

// CopyableView

struct CopyableView : xranges::view_base {
    std::string_view view_;
    constexpr explicit CopyableView() = default;
    constexpr CopyableView(char const* ptr) : view_(ptr) {}
    constexpr CopyableView(std::string_view v) : view_(v) {}
    constexpr forward_iterator<std::string_view::const_iterator> begin() const {
        return forward_iterator<std::string_view::const_iterator>(
            view_.begin());
    }
    constexpr forward_iterator<std::string_view::const_iterator> end() const {
        return forward_iterator<std::string_view::const_iterator>(view_.end());
    }
    constexpr bool operator==(CopyableView const& rhs) const {
        return view_ == rhs.view_;
    }
};
static_assert(xranges::forward_range<CopyableView>);
static_assert(xranges::forward_range<CopyableView const>);
static_assert(xranges::view<CopyableView>);
static_assert(std::is_copy_constructible_v<CopyableView>);

// ForwardView

struct ForwardView : xranges::view_base {
    std::string_view view_;
    constexpr explicit ForwardView() = default;
    constexpr ForwardView(char const* ptr) : view_(ptr) {}
    constexpr ForwardView(std::string_view v) : view_(v) {}
    constexpr ForwardView(ForwardView&&) = default;
    constexpr ForwardView& operator=(ForwardView&&) = default;
    constexpr forward_iterator<std::string_view::const_iterator> begin() const {
        return forward_iterator<std::string_view::const_iterator>(
            view_.begin());
    }
    constexpr forward_iterator<std::string_view::const_iterator> end() const {
        return forward_iterator<std::string_view::const_iterator>(view_.end());
    }
};
static_assert(xranges::forward_range<ForwardView>);
static_assert(xranges::forward_range<ForwardView const>);
static_assert(xranges::view<ForwardView>);
static_assert(!std::is_copy_constructible_v<ForwardView>);
static_assert(std::is_move_constructible_v<ForwardView>);

// ForwardDiffView

// Iterator types differ based on constness of this class.
struct ForwardDiffView : xranges::view_base {
    std::string buffer_;
    constexpr explicit ForwardDiffView() = default;
    constexpr ForwardDiffView(char const* ptr)
        : ForwardDiffView(std::string_view(ptr)) {}
    constexpr ForwardDiffView(std::string_view v) {
        // Workaround https://github.com/llvm/llvm-project/issues/55867
        buffer_ = v;
    }
    constexpr ForwardDiffView(ForwardDiffView&&) = default;
    constexpr ForwardDiffView& operator=(ForwardDiffView&&) = default;
    constexpr ForwardDiffView(ForwardDiffView const&) = default;
    constexpr ForwardDiffView& operator=(ForwardDiffView const&) = default;
    constexpr forward_iterator<char*> begin() {
        return forward_iterator<char*>(buffer_.data());
    }
    constexpr forward_iterator<char*> end() {
        return forward_iterator<char*>(buffer_.data() + buffer_.size());
    }
    constexpr forward_iterator<char const*> begin() const {
        return forward_iterator<char const*>(buffer_.data());
    }
    constexpr forward_iterator<char const*> end() const {
        return forward_iterator<char const*>(buffer_.data() + buffer_.size());
    }
};
static_assert(xranges::forward_range<ForwardView>);
static_assert(xranges::forward_range<ForwardView const>);
static_assert(xranges::view<ForwardView>);
static_assert(!std::same_as<xranges::iterator_t<ForwardDiffView>,
              xranges::iterator_t<ForwardDiffView const>>);

// ForwardOnlyIfNonConstView

template <class It>
class almost_forward_iterator {
    It it_;

    template <class U>
    friend class almost_forward_iterator;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename std::iterator_traits<It>::value_type;
    using difference_type = typename std::iterator_traits<It>::difference_type;
    using pointer = It;
    using reference = typename std::iterator_traits<It>::reference;

    constexpr almost_forward_iterator() : it_() {}
    constexpr explicit almost_forward_iterator(It it) : it_(it) {}
    template <class U>
    constexpr almost_forward_iterator(almost_forward_iterator<U> const& u)
        : it_(u.it_) {}

    constexpr reference operator*() const { return *it_; }
    constexpr pointer operator->() const { return it_; }

    constexpr almost_forward_iterator& operator++() {
        ++it_;
        return *this;
    }
    // Notice the slightly different return type.
    constexpr almost_forward_iterator const operator++(int) {
        return almost_forward_iterator(it_);
    }

    friend constexpr bool operator==(
        almost_forward_iterator const& x, almost_forward_iterator const& y) {
        return x.it_ == y.it_;
    }
    friend constexpr bool operator!=(
        almost_forward_iterator const& x, almost_forward_iterator const& y) {
        return x.it_ != y.it_;
    }
};
static_assert(!std::forward_iterator<almost_forward_iterator<int*>>);
static_assert(std::input_iterator<almost_forward_iterator<int*>>);

struct ForwardOnlyIfNonConstView : xranges::view_base {
    std::string_view view_;

    constexpr explicit ForwardOnlyIfNonConstView() = default;
    constexpr ForwardOnlyIfNonConstView(char const* ptr) : view_(ptr) {}
    constexpr ForwardOnlyIfNonConstView(std::string_view v) : view_(v) {}
    constexpr ForwardOnlyIfNonConstView(ForwardOnlyIfNonConstView&&) = default;
    constexpr ForwardOnlyIfNonConstView& operator=(
        ForwardOnlyIfNonConstView&&) = default;

    constexpr forward_iterator<std::string_view::const_iterator> begin() {
        return forward_iterator<std::string_view::const_iterator>(
            view_.begin());
    }
    constexpr forward_iterator<std::string_view::const_iterator> end() {
        return forward_iterator<std::string_view::const_iterator>(view_.end());
    }
    constexpr almost_forward_iterator<std::string_view::const_iterator>
    begin() const {
        return almost_forward_iterator<std::string_view::const_iterator>(
            view_.begin());
    }
    constexpr almost_forward_iterator<std::string_view::const_iterator>
    end() const {
        return almost_forward_iterator<std::string_view::const_iterator>(
            view_.end());
    }
};
static_assert(xranges::forward_range<ForwardOnlyIfNonConstView>);
static_assert(!xranges::forward_range<ForwardOnlyIfNonConstView const>);
static_assert(xranges::view<ForwardOnlyIfNonConstView>);

// InputView

struct InputView : xranges::view_base {
    std::string buffer_;

    constexpr InputView() = default;
    constexpr InputView(char const* s) : InputView(std::string_view(s)) {}
    constexpr InputView(std::string_view v) {
        // Workaround https://github.com/llvm/llvm-project/issues/55867
        buffer_ = v;
    }

    constexpr cpp20_input_iterator<char*> begin() {
        return cpp20_input_iterator<char*>(buffer_.data());
    }
    constexpr sentinel_wrapper<cpp20_input_iterator<char*>> end() {
        return sentinel_wrapper(
            cpp20_input_iterator<char*>(buffer_.data() + buffer_.size()));
    }
    constexpr cpp20_input_iterator<char const*> begin() const {
        return cpp20_input_iterator<char const*>(buffer_.data());
    }
    constexpr sentinel_wrapper<cpp20_input_iterator<char const*>> end() const {
        return sentinel_wrapper(
            cpp20_input_iterator<char const*>(buffer_.data() + buffer_.size()));
    }
    friend constexpr bool operator==(
        InputView const& lhs, InputView const& rhs) {
        return lhs.buffer_ == rhs.buffer_;
    }
};

static_assert(xranges::input_range<InputView>);
static_assert(xranges::input_range<InputView const>);
static_assert(xranges::view<InputView>);

// ForwardTinyView

struct ForwardTinyView : xranges::view_base {
    char c_[1] = {};
    constexpr ForwardTinyView() = default;
    constexpr ForwardTinyView(char c) { *c_ = c; }
    constexpr forward_iterator<char const*> begin() const {
        return forward_iterator<char const*>(c_);
    }
    constexpr forward_iterator<char const*> end() const {
        return forward_iterator<char const*>(c_ + 1);
    }
    constexpr static std::size_t size() { return 1; }
};
static_assert(xranges::forward_range<ForwardTinyView>);
static_assert(xranges::view<ForwardTinyView>);
static_assert(xranges::details::tiny_range<ForwardTinyView>);

// Aliases

using SplitViewCopyable = xranges::lazy_split_view<CopyableView, CopyableView>;
using OuterIterCopyable = xranges::iterator_t<SplitViewCopyable>;
using ValueTypeCopyable = OuterIterCopyable::value_type;
using InnerIterCopyable = xranges::iterator_t<ValueTypeCopyable>;
using BaseIterCopyable = xranges::iterator_t<CopyableView>;

using SplitViewForward = xranges::lazy_split_view<ForwardView, ForwardView>;
using OuterIterForward = xranges::iterator_t<SplitViewForward>;
using ValueTypeForward = OuterIterForward::value_type;
using InnerIterForward = xranges::iterator_t<ValueTypeForward>;
using BaseIterForward = xranges::iterator_t<ForwardView>;

using SplitViewInput = xranges::lazy_split_view<InputView, ForwardTinyView>;
using OuterIterInput = xranges::iterator_t<SplitViewInput>;
using ValueTypeInput = OuterIterInput::value_type;
using InnerIterInput = xranges::iterator_t<ValueTypeInput>;
using BaseIterInput = xranges::iterator_t<InputView>;

using SplitViewDiff =
    xranges::lazy_split_view<ForwardDiffView, ForwardDiffView>;
using OuterIterConst = decltype(std::declval<SplitViewDiff const>().begin());
using OuterIterNonConst = decltype(std::declval<SplitViewDiff>().begin());
static_assert(!std::same_as<OuterIterConst, OuterIterNonConst>);
using InnerIterConst = decltype((*std::declval<OuterIterConst>()).begin());
using InnerIterNonConst =
    decltype((*std::declval<OuterIterNonConst>()).begin());
static_assert(!std::same_as<InnerIterConst, InnerIterNonConst>);

#endif // TEST_STD_RANGES_RANGE_ADAPTORS_RANGE_LAZY_SPLIT_TYPES_H
