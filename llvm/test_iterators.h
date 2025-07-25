// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef SUPPORT_TEST_ITERATORS_H
#define SUPPORT_TEST_ITERATORS_H

#include "rxx/iterator.h"
#include "rxx/ranges.h"
#include "type_algorithms.h"

#include <cassert>
#include <concepts>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

namespace details {
namespace constructible {
template <typename T>
void implicit_conv(T) noexcept;

template <typename T, typename THead, typename TMid, typename... TTail>
auto explicit_test(int) noexcept
    -> decltype((implicit_conv<T>({std::declval<THead>(), std::declval<TMid>(),
                     std::declval<TTail>()...}),
        std::false_type{}));
template <typename T, typename THead, typename TMid, typename... TTail>
auto explicit_test(float) noexcept
    -> std::is_constructible<T, THead, TMid, TTail...>;
template <typename T, typename THead>
auto explicit_test(int) noexcept
    -> std::conjunction<std::negation<std::is_convertible<THead, T>>,
        std::is_constructible<T, THead>>;
template <typename T>
auto explicit_test(int) noexcept
    -> std::conjunction<std::negation<decltype(implicit_test<T>(0))>,
        std::is_default_constructible<T>>;
template <typename T, typename...>
auto explicit_test(...) noexcept -> std::false_type;

template <typename TTarget, typename... TArgs>
using is_explicit = decltype(explicit_test<TTarget, TArgs...>(0));
} // namespace constructible
} // namespace details

template <typename TTarget, typename... TArgs>
inline constexpr bool is_explicit_constructible_v =
    details::constructible::is_explicit<TTarget, TArgs...>::value;

namespace support {

struct double_move_tracker {
    constexpr double_move_tracker() : moved_from_(false) {}

    double_move_tracker(double_move_tracker const&) = default;

    constexpr double_move_tracker(double_move_tracker&& other)
        : moved_from_(false) {
        assert(!other.moved_from_);
        other.moved_from_ = true;
    }

    double_move_tracker& operator=(double_move_tracker const&) = default;

    constexpr double_move_tracker& operator=(double_move_tracker&& other) {
        assert(!other.moved_from_);
        other.moved_from_ = true;
        moved_from_ = false;
        return *this;
    }

private:
    bool moved_from_;
};

} // namespace support

// This iterator meets C++20's Cpp17OutputIterator requirements, as described
// in Table 90 ([output.iterators]).
template <class It>
class cpp17_output_iterator {
    It it_;
    support::double_move_tracker tracker_;

    template <class U>
    friend class cpp17_output_iterator;

public:
    typedef std::output_iterator_tag iterator_category;
    typedef void value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It pointer;
    typedef typename std::iterator_traits<It>::reference reference;

    constexpr explicit cpp17_output_iterator(It it) : it_(std::move(it)) {}

    template <class U>
    constexpr cpp17_output_iterator(cpp17_output_iterator<U> const& u)
        : it_(u.it_)
        , tracker_(u.tracker_) {}

    template <class U,
        class = typename std::enable_if<
            std::is_default_constructible<U>::value>::type>
    constexpr cpp17_output_iterator(cpp17_output_iterator<U>&& u)
        : it_(std::move(u.it_))
        , tracker_(std::move(u.tracker_)) {
        u.it_ = U();
    }

    constexpr reference operator*() const { return *it_; }

    constexpr cpp17_output_iterator& operator++() {
        ++it_;
        return *this;
    }
    constexpr cpp17_output_iterator operator++(int) {
        return cpp17_output_iterator(it_++);
    }

    friend constexpr It base(cpp17_output_iterator const& i) { return i.it_; }

    template <class T>
    void operator,(T const&) = delete;
};

template <class It>
cpp17_output_iterator(It) -> cpp17_output_iterator<It>;

static_assert(std::output_iterator<cpp17_output_iterator<int*>, int>);

// This iterator meets C++20's Cpp17InputIterator requirements, as described
// in Table 89 ([input.iterators]).
template <class It, class ItTraits = It>
class cpp17_input_iterator {
    typedef std::iterator_traits<ItTraits> Traits;
    It it_;
    support::double_move_tracker tracker_;

    template <class U, class T>
    friend class cpp17_input_iterator;

public:
    typedef std::input_iterator_tag iterator_category;
    typedef typename Traits::value_type value_type;
    typedef typename Traits::difference_type difference_type;
    typedef It pointer;
    typedef typename Traits::reference reference;

    constexpr explicit cpp17_input_iterator(It it) : it_(it) {}

    template <class U, class T>
    constexpr cpp17_input_iterator(cpp17_input_iterator<U, T> const& u)
        : it_(u.it_)
        , tracker_(u.tracker_) {}

    template <class U, class T,
        class = typename std::enable_if<
            std::is_default_constructible<U>::value>::type>
    constexpr cpp17_input_iterator(cpp17_input_iterator<U, T>&& u)
        : it_(std::move(u.it_))
        , tracker_(std::move(u.tracker_)) {
        u.it_ = U();
    }

    constexpr reference operator*() const { return *it_; }

    constexpr cpp17_input_iterator& operator++() {
        ++it_;
        return *this;
    }
    constexpr cpp17_input_iterator operator++(int) {
        return cpp17_input_iterator(it_++);
    }

    friend constexpr bool operator==(
        cpp17_input_iterator const& x, cpp17_input_iterator const& y) {
        return x.it_ == y.it_;
    }
    friend constexpr bool operator!=(
        cpp17_input_iterator const& x, cpp17_input_iterator const& y) {
        return x.it_ != y.it_;
    }

    friend constexpr It base(cpp17_input_iterator const& i) { return i.it_; }

    template <class T>
    void operator,(T const&) = delete;
};

template <class It>
cpp17_input_iterator(It) -> cpp17_input_iterator<It>;

static_assert(std::input_iterator<cpp17_input_iterator<int*>>);

template <class It>
class forward_iterator {
    It it_;
    support::double_move_tracker tracker_;

    template <class U>
    friend class forward_iterator;

public:
    typedef std::forward_iterator_tag iterator_category;
    typedef typename std::iterator_traits<It>::value_type value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It pointer;
    typedef typename std::iterator_traits<It>::reference reference;

    constexpr forward_iterator() : it_() {}
    constexpr explicit forward_iterator(It it) : it_(it) {}

    template <class U>
    constexpr forward_iterator(forward_iterator<U> const& u)
        : it_(u.it_)
        , tracker_(u.tracker_) {}

    template <class U,
        class = typename std::enable_if<
            std::is_default_constructible<U>::value>::type>
    constexpr forward_iterator(forward_iterator<U>&& other)
        : it_(std::move(other.it_))
        , tracker_(std::move(other.tracker_)) {
        other.it_ = U();
    }

    constexpr reference operator*() const { return *it_; }

    constexpr forward_iterator& operator++() {
        ++it_;
        return *this;
    }
    constexpr forward_iterator operator++(int) {
        return forward_iterator(it_++);
    }

    friend constexpr bool operator==(
        forward_iterator const& x, forward_iterator const& y) {
        return x.it_ == y.it_;
    }
    friend constexpr bool operator!=(
        forward_iterator const& x, forward_iterator const& y) {
        return x.it_ != y.it_;
    }

    friend constexpr It base(forward_iterator const& i) { return i.it_; }

    template <class T>
    void operator,(T const&) = delete;
};

template <class It>
forward_iterator(It) -> forward_iterator<It>;

template <class It>
class bidirectional_iterator {
    It it_;
    support::double_move_tracker tracker_;

    template <class U>
    friend class bidirectional_iterator;

public:
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef typename std::iterator_traits<It>::value_type value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It pointer;
    typedef typename std::iterator_traits<It>::reference reference;

    constexpr bidirectional_iterator() : it_() {}
    constexpr explicit bidirectional_iterator(It it) : it_(it) {}

    template <class U>
    constexpr bidirectional_iterator(bidirectional_iterator<U> const& u)
        : it_(u.it_)
        , tracker_(u.tracker_) {}

    template <class U,
        class = typename std::enable_if<
            std::is_default_constructible<U>::value>::type>
    constexpr bidirectional_iterator(bidirectional_iterator<U>&& u)
        : it_(std::move(u.it_))
        , tracker_(std::move(u.tracker_)) {
        u.it_ = U();
    }

    constexpr reference operator*() const { return *it_; }

    constexpr bidirectional_iterator& operator++() {
        ++it_;
        return *this;
    }
    constexpr bidirectional_iterator& operator--() {
        --it_;
        return *this;
    }
    constexpr bidirectional_iterator operator++(int) {
        return bidirectional_iterator(it_++);
    }
    constexpr bidirectional_iterator operator--(int) {
        return bidirectional_iterator(it_--);
    }

    friend constexpr bool operator==(
        bidirectional_iterator const& x, bidirectional_iterator const& y) {
        return x.it_ == y.it_;
    }
    friend constexpr bool operator!=(
        bidirectional_iterator const& x, bidirectional_iterator const& y) {
        return x.it_ != y.it_;
    }

    friend constexpr It base(bidirectional_iterator const& i) { return i.it_; }

    template <class T>
    void operator,(T const&) = delete;
};

template <class It>
bidirectional_iterator(It) -> bidirectional_iterator<It>;

template <class It>
class random_access_iterator {
    It it_;
    support::double_move_tracker tracker_;

    template <class U>
    friend class random_access_iterator;

public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef typename std::iterator_traits<It>::value_type value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It pointer;
    typedef typename std::iterator_traits<It>::reference reference;

    constexpr random_access_iterator() : it_() {}
    constexpr explicit random_access_iterator(It it) : it_(it) {}

    template <class U>
    constexpr random_access_iterator(random_access_iterator<U> const& u)
        : it_(u.it_)
        , tracker_(u.tracker_) {}

    template <class U,
        class = typename std::enable_if<
            std::is_default_constructible<U>::value>::type>
    constexpr random_access_iterator(random_access_iterator<U>&& u)
        : it_(std::move(u.it_))
        , tracker_(std::move(u.tracker_)) {
        u.it_ = U();
    }

    constexpr reference operator*() const { return *it_; }
    constexpr reference operator[](difference_type n) const { return it_[n]; }

    constexpr random_access_iterator& operator++() {
        ++it_;
        return *this;
    }
    constexpr random_access_iterator& operator--() {
        --it_;
        return *this;
    }
    constexpr random_access_iterator operator++(int) {
        return random_access_iterator(it_++);
    }
    constexpr random_access_iterator operator--(int) {
        return random_access_iterator(it_--);
    }

    constexpr random_access_iterator& operator+=(difference_type n) {
        it_ += n;
        return *this;
    }
    constexpr random_access_iterator& operator-=(difference_type n) {
        it_ -= n;
        return *this;
    }
    friend constexpr random_access_iterator operator+(
        random_access_iterator x, difference_type n) {
        x += n;
        return x;
    }
    friend constexpr random_access_iterator operator+(
        difference_type n, random_access_iterator x) {
        x += n;
        return x;
    }
    friend constexpr random_access_iterator operator-(
        random_access_iterator x, difference_type n) {
        x -= n;
        return x;
    }
    friend constexpr difference_type operator-(
        random_access_iterator x, random_access_iterator y) {
        return x.it_ - y.it_;
    }

    friend constexpr bool operator==(
        random_access_iterator const& x, random_access_iterator const& y) {
        return x.it_ == y.it_;
    }
    friend constexpr bool operator!=(
        random_access_iterator const& x, random_access_iterator const& y) {
        return x.it_ != y.it_;
    }
    friend constexpr bool operator<(
        random_access_iterator const& x, random_access_iterator const& y) {
        return x.it_ < y.it_;
    }
    friend constexpr bool operator<=(
        random_access_iterator const& x, random_access_iterator const& y) {
        return x.it_ <= y.it_;
    }
    friend constexpr bool operator>(
        random_access_iterator const& x, random_access_iterator const& y) {
        return x.it_ > y.it_;
    }
    friend constexpr bool operator>=(
        random_access_iterator const& x, random_access_iterator const& y) {
        return x.it_ >= y.it_;
    }

    friend constexpr It base(random_access_iterator const& i) { return i.it_; }

    template <class T>
    void operator,(T const&) = delete;
};

template <class It>
random_access_iterator(It) -> random_access_iterator<It>;

// Since C++20, a container iterator type that is random access is also required
// to support three-way comparison. See C++20 [tab:container.req], C++23
// [container.reqmts]/39 - /41.
template <class It>
class three_way_random_access_iterator {
    It it_;
    support::double_move_tracker tracker_;

    template <class U>
    friend class three_way_random_access_iterator;

public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef typename std::iterator_traits<It>::value_type value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It pointer;
    typedef typename std::iterator_traits<It>::reference reference;

    constexpr three_way_random_access_iterator() : it_() {}
    constexpr explicit three_way_random_access_iterator(It it) : it_(it) {}

    template <class U>
    constexpr three_way_random_access_iterator(
        three_way_random_access_iterator<U> const& u)
        : it_(u.it_)
        , tracker_(u.tracker_) {}

    template <class U,
        class = typename std::enable_if<
            std::is_default_constructible<U>::value>::type>
    constexpr three_way_random_access_iterator(
        three_way_random_access_iterator<U>&& u)
        : it_(std::move(u.it_))
        , tracker_(std::move(u.tracker_)) {
        u.it_ = U();
    }

    constexpr reference operator*() const { return *it_; }
    constexpr reference operator[](difference_type n) const { return it_[n]; }

    constexpr three_way_random_access_iterator& operator++() {
        ++it_;
        return *this;
    }
    constexpr three_way_random_access_iterator& operator--() {
        --it_;
        return *this;
    }
    constexpr three_way_random_access_iterator operator++(int) {
        return three_way_random_access_iterator(it_++);
    }
    constexpr three_way_random_access_iterator operator--(int) {
        return three_way_random_access_iterator(it_--);
    }

    constexpr three_way_random_access_iterator& operator+=(difference_type n) {
        it_ += n;
        return *this;
    }
    constexpr three_way_random_access_iterator& operator-=(difference_type n) {
        it_ -= n;
        return *this;
    }
    friend constexpr three_way_random_access_iterator operator+(
        three_way_random_access_iterator x, difference_type n) {
        x += n;
        return x;
    }
    friend constexpr three_way_random_access_iterator operator+(
        difference_type n, three_way_random_access_iterator x) {
        x += n;
        return x;
    }
    friend constexpr three_way_random_access_iterator operator-(
        three_way_random_access_iterator x, difference_type n) {
        x -= n;
        return x;
    }
    friend constexpr difference_type operator-(
        three_way_random_access_iterator x,
        three_way_random_access_iterator y) {
        return x.it_ - y.it_;
    }

    friend constexpr bool operator==(three_way_random_access_iterator const& x,
        three_way_random_access_iterator const& y) {
        return x.it_ == y.it_;
    }

    friend constexpr bool operator<(three_way_random_access_iterator const& x,
        three_way_random_access_iterator const& y) {
        return x.it_ < y.it_;
    }
    friend constexpr bool operator<=(three_way_random_access_iterator const& x,
        three_way_random_access_iterator const& y) {
        return x.it_ <= y.it_;
    }
    friend constexpr bool operator>(three_way_random_access_iterator const& x,
        three_way_random_access_iterator const& y) {
        return x.it_ > y.it_;
    }
    friend constexpr bool operator>=(three_way_random_access_iterator const& x,
        three_way_random_access_iterator const& y) {
        return x.it_ >= y.it_;
    }

    friend constexpr std::strong_ordering operator<=>(
        three_way_random_access_iterator const& x,
        three_way_random_access_iterator const& y) {
        if constexpr (std::three_way_comparable<It>) {
            return x.it_ <=> y.it_;
        } else {
            if (x.it_ < y.it_) {
                return std::strong_ordering::less;
            } else if (y.it_ < x.it_) {
                return std::strong_ordering::greater;
            } else {
                return std::strong_ordering::equal;
            }
        }
    }

    friend constexpr It base(three_way_random_access_iterator const& i) {
        return i.it_;
    }

    template <class T>
    void operator,(T const&) = delete;
};

template <class It>
three_way_random_access_iterator(It) -> three_way_random_access_iterator<It>;

template <std::random_access_iterator It>
class cpp20_random_access_iterator {
    It it_;
    support::double_move_tracker tracker_;

    template <std::random_access_iterator>
    friend class cpp20_random_access_iterator;

public:
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = std::random_access_iterator_tag;
    using value_type = typename std::iterator_traits<It>::value_type;
    using difference_type = typename std::iterator_traits<It>::difference_type;

    constexpr cpp20_random_access_iterator() : it_() {}
    constexpr explicit cpp20_random_access_iterator(It it) : it_(it) {}

    template <class U>
    constexpr cpp20_random_access_iterator(
        cpp20_random_access_iterator<U> const& u)
        : it_(u.it_)
        , tracker_(u.tracker_) {}

    template <class U>
    constexpr cpp20_random_access_iterator(cpp20_random_access_iterator<U>&& u)
        : it_(std::move(u.it_))
        , tracker_(std::move(u.tracker_)) {
        u.it_ = U();
    }

    constexpr decltype(auto) operator*() const { return *it_; }
    constexpr decltype(auto) operator[](difference_type n) const {
        return it_[n];
    }

    constexpr cpp20_random_access_iterator& operator++() {
        ++it_;
        return *this;
    }
    constexpr cpp20_random_access_iterator& operator--() {
        --it_;
        return *this;
    }
    constexpr cpp20_random_access_iterator operator++(int) {
        return cpp20_random_access_iterator(it_++);
    }
    constexpr cpp20_random_access_iterator operator--(int) {
        return cpp20_random_access_iterator(it_--);
    }

    constexpr cpp20_random_access_iterator& operator+=(difference_type n) {
        it_ += n;
        return *this;
    }
    constexpr cpp20_random_access_iterator& operator-=(difference_type n) {
        it_ -= n;
        return *this;
    }
    friend constexpr cpp20_random_access_iterator operator+(
        cpp20_random_access_iterator x, difference_type n) {
        x += n;
        return x;
    }
    friend constexpr cpp20_random_access_iterator operator+(
        difference_type n, cpp20_random_access_iterator x) {
        x += n;
        return x;
    }
    friend constexpr cpp20_random_access_iterator operator-(
        cpp20_random_access_iterator x, difference_type n) {
        x -= n;
        return x;
    }
    friend constexpr difference_type operator-(
        cpp20_random_access_iterator x, cpp20_random_access_iterator y) {
        return x.it_ - y.it_;
    }

    friend constexpr bool operator==(cpp20_random_access_iterator const& x,
        cpp20_random_access_iterator const& y) {
        return x.it_ == y.it_;
    }
    friend constexpr bool operator!=(cpp20_random_access_iterator const& x,
        cpp20_random_access_iterator const& y) {
        return x.it_ != y.it_;
    }
    friend constexpr bool operator<(cpp20_random_access_iterator const& x,
        cpp20_random_access_iterator const& y) {
        return x.it_ < y.it_;
    }
    friend constexpr bool operator<=(cpp20_random_access_iterator const& x,
        cpp20_random_access_iterator const& y) {
        return x.it_ <= y.it_;
    }
    friend constexpr bool operator>(cpp20_random_access_iterator const& x,
        cpp20_random_access_iterator const& y) {
        return x.it_ > y.it_;
    }
    friend constexpr bool operator>=(cpp20_random_access_iterator const& x,
        cpp20_random_access_iterator const& y) {
        return x.it_ >= y.it_;
    }

    friend constexpr It base(cpp20_random_access_iterator const& i) {
        return i.it_;
    }

    template <class T>
    void operator,(T const&) = delete;
};
template <class It>
cpp20_random_access_iterator(It) -> cpp20_random_access_iterator<It>;

static_assert(std::random_access_iterator<cpp20_random_access_iterator<int*>>);

template <std::contiguous_iterator It>
class contiguous_iterator {
    It it_;
    support::double_move_tracker tracker_;

    template <std::contiguous_iterator U>
    friend class contiguous_iterator;

public:
    using iterator_category = std::contiguous_iterator_tag;
    using value_type = typename std::iterator_traits<It>::value_type;
    using difference_type = typename std::iterator_traits<It>::difference_type;
    using pointer = typename std::iterator_traits<It>::pointer;
    using reference = typename std::iterator_traits<It>::reference;
    using element_type = value_type;

    constexpr It base() const { return it_; }

    constexpr contiguous_iterator() : it_() {}
    constexpr explicit contiguous_iterator(It it) : it_(it) {}

    template <class U>
    constexpr contiguous_iterator(contiguous_iterator<U> const& u)
        : it_(u.it_)
        , tracker_(u.tracker_) {}

    template <class U,
        class = typename std::enable_if<
            std::is_default_constructible<U>::value>::type>
    constexpr contiguous_iterator(contiguous_iterator<U>&& u)
        : it_(std::move(u.it_))
        , tracker_(std::move(u.tracker_)) {
        u.it_ = U();
    }

    constexpr reference operator*() const { return *it_; }
    constexpr pointer operator->() const { return it_; }
    constexpr reference operator[](difference_type n) const { return it_[n]; }

    constexpr contiguous_iterator& operator++() {
        ++it_;
        return *this;
    }
    constexpr contiguous_iterator& operator--() {
        --it_;
        return *this;
    }
    constexpr contiguous_iterator operator++(int) {
        return contiguous_iterator(it_++);
    }
    constexpr contiguous_iterator operator--(int) {
        return contiguous_iterator(it_--);
    }

    constexpr contiguous_iterator& operator+=(difference_type n) {
        it_ += n;
        return *this;
    }
    constexpr contiguous_iterator& operator-=(difference_type n) {
        it_ -= n;
        return *this;
    }
    friend constexpr contiguous_iterator operator+(
        contiguous_iterator x, difference_type n) {
        x += n;
        return x;
    }
    friend constexpr contiguous_iterator operator+(
        difference_type n, contiguous_iterator x) {
        x += n;
        return x;
    }
    friend constexpr contiguous_iterator operator-(
        contiguous_iterator x, difference_type n) {
        x -= n;
        return x;
    }
    friend constexpr difference_type operator-(
        contiguous_iterator x, contiguous_iterator y) {
        return x.it_ - y.it_;
    }

    friend constexpr bool operator==(
        contiguous_iterator const& x, contiguous_iterator const& y) {
        return x.it_ == y.it_;
    }
    friend constexpr bool operator!=(
        contiguous_iterator const& x, contiguous_iterator const& y) {
        return x.it_ != y.it_;
    }
    friend constexpr bool operator<(
        contiguous_iterator const& x, contiguous_iterator const& y) {
        return x.it_ < y.it_;
    }
    friend constexpr bool operator<=(
        contiguous_iterator const& x, contiguous_iterator const& y) {
        return x.it_ <= y.it_;
    }
    friend constexpr bool operator>(
        contiguous_iterator const& x, contiguous_iterator const& y) {
        return x.it_ > y.it_;
    }
    friend constexpr bool operator>=(
        contiguous_iterator const& x, contiguous_iterator const& y) {
        return x.it_ >= y.it_;
    }

    // Note no operator<=>, use three_way_contiguous_iterator for testing
    // operator<=>

    friend constexpr It base(contiguous_iterator const& i) { return i.it_; }

    template <class T>
    void operator,(T const&) = delete;
};
template <class It>
contiguous_iterator(It) -> contiguous_iterator<It>;

template <class It>
class three_way_contiguous_iterator {
    static_assert(std::is_pointer_v<It>, "Things probably break in this case");

    It it_;
    support::double_move_tracker tracker_;

    template <class U>
    friend class three_way_contiguous_iterator;

public:
    typedef std::contiguous_iterator_tag iterator_category;
    typedef typename std::iterator_traits<It>::value_type value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It pointer;
    typedef typename std::iterator_traits<It>::reference reference;
    typedef typename std::remove_pointer<It>::type element_type;

    constexpr It base() const { return it_; }

    constexpr three_way_contiguous_iterator() : it_() {}
    constexpr explicit three_way_contiguous_iterator(It it) : it_(it) {}

    template <class U>
    constexpr three_way_contiguous_iterator(
        three_way_contiguous_iterator<U> const& u)
        : it_(u.it_)
        , tracker_(u.tracker_) {}

    template <class U,
        class = typename std::enable_if<
            std::is_default_constructible<U>::value>::type>
    constexpr three_way_contiguous_iterator(
        three_way_contiguous_iterator<U>&& u)
        : it_(std::move(u.it_))
        , tracker_(std::move(u.tracker_)) {
        u.it_ = U();
    }

    constexpr reference operator*() const { return *it_; }
    constexpr pointer operator->() const { return it_; }
    constexpr reference operator[](difference_type n) const { return it_[n]; }

    constexpr three_way_contiguous_iterator& operator++() {
        ++it_;
        return *this;
    }
    constexpr three_way_contiguous_iterator& operator--() {
        --it_;
        return *this;
    }
    constexpr three_way_contiguous_iterator operator++(int) {
        return three_way_contiguous_iterator(it_++);
    }
    constexpr three_way_contiguous_iterator operator--(int) {
        return three_way_contiguous_iterator(it_--);
    }

    constexpr three_way_contiguous_iterator& operator+=(difference_type n) {
        it_ += n;
        return *this;
    }
    constexpr three_way_contiguous_iterator& operator-=(difference_type n) {
        it_ -= n;
        return *this;
    }
    friend constexpr three_way_contiguous_iterator operator+(
        three_way_contiguous_iterator x, difference_type n) {
        x += n;
        return x;
    }
    friend constexpr three_way_contiguous_iterator operator+(
        difference_type n, three_way_contiguous_iterator x) {
        x += n;
        return x;
    }
    friend constexpr three_way_contiguous_iterator operator-(
        three_way_contiguous_iterator x, difference_type n) {
        x -= n;
        return x;
    }
    friend constexpr difference_type operator-(
        three_way_contiguous_iterator x, three_way_contiguous_iterator y) {
        return x.it_ - y.it_;
    }

    friend constexpr auto operator<=>(three_way_contiguous_iterator const& x,
        three_way_contiguous_iterator const& y) {
        return x.it_ <=> y.it_;
    }
    friend constexpr bool operator==(three_way_contiguous_iterator const& x,
        three_way_contiguous_iterator const& y) {
        return x.it_ == y.it_;
    }

    template <class T>
    void operator,(T const&) = delete;
};
template <class It>
three_way_contiguous_iterator(It) -> three_way_contiguous_iterator<It>;

template <class Iter> // ADL base() for everything else (including pointers)
constexpr Iter base(Iter i) {
    return i;
}

template <typename T>
struct ThrowingIterator {
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type;
    typedef T const value_type;
    typedef T const* pointer;
    typedef T const& reference;

    enum ThrowingAction {
        TAIncrement,
        TADecrement,
        TADereference,
        TAAssignment,
        TAComparison
    };

    constexpr ThrowingIterator()
        : begin_(nullptr)
        , end_(nullptr)
        , current_(nullptr)
        , action_(TADereference)
        , index_(0) {}
    constexpr explicit ThrowingIterator(T const* first, T const* last,
        int index = 0, ThrowingAction action = TADereference)
        : begin_(first)
        , end_(last)
        , current_(first)
        , action_(action)
        , index_(index) {}
    constexpr ThrowingIterator(ThrowingIterator const& rhs)
        : begin_(rhs.begin_)
        , end_(rhs.end_)
        , current_(rhs.current_)
        , action_(rhs.action_)
        , index_(rhs.index_) {}

    constexpr ThrowingIterator& operator=(ThrowingIterator const& rhs) {
        if (action_ == TAAssignment && --index_ < 0) {
#if RXX_WITH_EXCEPTIONS
            throw std::runtime_error("throw from iterator assignment");
#else
            assert(false);
#endif
        }
        begin_ = rhs.begin_;
        end_ = rhs.end_;
        current_ = rhs.current_;
        action_ = rhs.action_;
        index_ = rhs.index_;
        return *this;
    }

    constexpr reference operator*() const {
        if (action_ == TADereference && --index_ < 0) {
#if RXX_WITH_EXCEPTIONS
            throw std::runtime_error("throw from iterator dereference");
#else
            assert(false);
#endif
        }
        return *current_;
    }

    constexpr ThrowingIterator& operator++() {
        if (action_ == TAIncrement && --index_ < 0) {
#if RXX_WITH_EXCEPTIONS
            throw std::runtime_error("throw from iterator increment");
#else
            assert(false);
#endif
        }
        ++current_;
        return *this;
    }

    constexpr ThrowingIterator operator++(int) {
        ThrowingIterator temp = *this;
        ++(*this);
        return temp;
    }

    constexpr ThrowingIterator& operator--() {
        if (action_ == TADecrement && --index_ < 0) {
#if RXX_WITH_EXCEPTIONS
            throw std::runtime_error("throw from iterator decrement");
#else
            assert(false);
#endif
        }
        --current_;
        return *this;
    }

    constexpr ThrowingIterator operator--(int) {
        ThrowingIterator temp = *this;
        --(*this);
        return temp;
    }

    constexpr friend bool operator==(
        ThrowingIterator const& a, ThrowingIterator const& b) {
        if (a.action_ == TAComparison && --a.index_ < 0) {
#if RXX_WITH_EXCEPTIONS
            throw std::runtime_error("throw from iterator comparison");
#else
            assert(false);
#endif
        }
        bool atEndL = a.current_ == a.end_;
        bool atEndR = b.current_ == b.end_;
        if (atEndL != atEndR)
            return false; // one is at the end (or empty), the other is not.
        if (atEndL)
            return true; // both are at the end (or empty)
        return a.current_ == b.current_;
    }

    constexpr friend bool operator!=(
        ThrowingIterator const& a, ThrowingIterator const& b) {
        return !(a == b);
    }

    template <class T2>
    void operator,(T2 const&) = delete;

private:
    T const* begin_;
    T const* end_;
    T const* current_;
    ThrowingAction action_;
    mutable int index_;
};

template <typename T>
struct NonThrowingIterator {
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type;
    typedef T const value_type;
    typedef T const* pointer;
    typedef T const& reference;

    NonThrowingIterator() : begin_(nullptr), end_(nullptr), current_(nullptr) {}
    explicit NonThrowingIterator(T const* first, T const* last)
        : begin_(first)
        , end_(last)
        , current_(first) {}
    NonThrowingIterator(NonThrowingIterator const& rhs)
        : begin_(rhs.begin_)
        , end_(rhs.end_)
        , current_(rhs.current_) {}

    NonThrowingIterator& operator=(NonThrowingIterator const& rhs) noexcept {
        begin_ = rhs.begin_;
        end_ = rhs.end_;
        current_ = rhs.current_;
        return *this;
    }

    reference operator*() const noexcept { return *current_; }

    NonThrowingIterator& operator++() noexcept {
        ++current_;
        return *this;
    }

    NonThrowingIterator operator++(int) noexcept {
        NonThrowingIterator temp = *this;
        ++(*this);
        return temp;
    }

    NonThrowingIterator& operator--() noexcept {
        --current_;
        return *this;
    }

    NonThrowingIterator operator--(int) noexcept {
        NonThrowingIterator temp = *this;
        --(*this);
        return temp;
    }

    friend bool operator==(
        NonThrowingIterator const& a, NonThrowingIterator const& b) noexcept {
        bool atEndL = a.current_ == a.end_;
        bool atEndR = b.current_ == b.end_;
        if (atEndL != atEndR)
            return false; // one is at the end (or empty), the other is not.
        if (atEndL)
            return true; // both are at the end (or empty)
        return a.current_ == b.current_;
    }

    friend bool operator!=(
        NonThrowingIterator const& a, NonThrowingIterator const& b) noexcept {
        return !(a == b);
    }

    template <class T2>
    void operator,(T2 const&) = delete;

private:
    T const* begin_;
    T const* end_;
    T const* current_;
};

template <class It>
class cpp20_input_iterator {
    It it_;
    support::double_move_tracker tracker_;

public:
    using value_type = std::iter_value_t<It>;
    using difference_type = std::iter_difference_t<It>;
    using iterator_concept = std::input_iterator_tag;

    constexpr explicit cpp20_input_iterator(It it) : it_(it) {}
    cpp20_input_iterator(cpp20_input_iterator&&) = default;
    cpp20_input_iterator& operator=(cpp20_input_iterator&&) = default;
    constexpr decltype(auto) operator*() const { return *it_; }
    constexpr cpp20_input_iterator& operator++() {
        ++it_;
        return *this;
    }
    constexpr void operator++(int) { ++it_; }

    friend constexpr It base(cpp20_input_iterator const& i) { return i.it_; }

    template <class T>
    void operator,(T const&) = delete;
};
template <class It>
cpp20_input_iterator(It) -> cpp20_input_iterator<It>;

static_assert(std::input_iterator<cpp20_input_iterator<int*>>);

template <std::input_or_output_iterator>
struct iter_value_or_void {
    using type = void;
};

template <std::input_iterator I>
struct iter_value_or_void<I> {
    using type = std::iter_value_t<I>;
};

template <class It>
class cpp20_output_iterator {
    It it_;
    support::double_move_tracker tracker_;

public:
    using difference_type = std::iter_difference_t<It>;

    constexpr explicit cpp20_output_iterator(It it) : it_(it) {}
    cpp20_output_iterator(cpp20_output_iterator&&) = default;
    cpp20_output_iterator& operator=(cpp20_output_iterator&&) = default;

    constexpr decltype(auto) operator*() const { return *it_; }
    constexpr cpp20_output_iterator& operator++() {
        ++it_;
        return *this;
    }
    constexpr cpp20_output_iterator operator++(int) {
        return cpp20_output_iterator(it_++);
    }

    friend constexpr It base(cpp20_output_iterator const& i) { return i.it_; }

    template <class T>
    void operator,(T const&) = delete;
};
template <class It>
cpp20_output_iterator(It) -> cpp20_output_iterator<It>;

static_assert(std::output_iterator<cpp20_output_iterator<int*>, int>);

// An `input_iterator` that can be used in a `xranges::common_range`
template <class Base>
struct common_input_iterator {
    Base it_;

    using value_type = std::iter_value_t<Base>;
    using difference_type = std::intptr_t;
    using iterator_concept = std::input_iterator_tag;

    constexpr common_input_iterator() = default;
    constexpr explicit common_input_iterator(Base it) : it_(it) {}

    constexpr common_input_iterator& operator++() {
        ++it_;
        return *this;
    }
    constexpr void operator++(int) { ++it_; }

    constexpr decltype(auto) operator*() const { return *it_; }

    friend constexpr bool operator==(
        common_input_iterator const&, common_input_iterator const&) = default;
};

struct IteratorOpCounts {
    std::size_t increments = 0; ///< Number of times the iterator moved forward
                                ///< (++it, it++, it+=positive, it-=negative).
    std::size_t decrements = 0; ///< Number of times the iterator moved backward
                                ///< (--it, it--, it-=positive, it+=negative).
    std::size_t zero_moves = 0; ///< Number of times a call was made to move the
                                ///< iterator by 0 positions (it+=0, it-=0).
    std::size_t equal_cmps =
        0; ///< Total number of calls to op== or op!=. If compared against a
           ///< sentinel object, that
           ///  sentinel object must call the `record_equality_comparison`
           ///  function so that the comparison is counted correctly.
};

// Iterator adaptor that records its operation counts in a IteratorOpCounts
template <class It>
class operation_counting_iterator {
public:
    using value_type = typename iter_value_or_void<It>::type;
    using difference_type = std::iter_difference_t<It>;
    using iterator_concept = std::conditional_t<std::contiguous_iterator<It>,
        std::contiguous_iterator_tag,
        std::conditional_t<std::random_access_iterator<It>,
            std::random_access_iterator_tag,
            std::conditional_t<std::bidirectional_iterator<It>,
                std::bidirectional_iterator_tag,
                std::conditional_t<std::forward_iterator<It>,
                    std::forward_iterator_tag,
                    std::conditional_t<std::input_iterator<It>,
                        std::input_iterator_tag,
                        /* else */ std::output_iterator_tag>>>>>;
    using iterator_category = iterator_concept;

    operation_counting_iterator()
    requires std::default_initializable<It>
    = default;

    constexpr explicit operation_counting_iterator(
        It const& it, IteratorOpCounts* counts = nullptr)
        : base_(base(it))
        , counts_(counts) {}

    constexpr operation_counting_iterator(
        operation_counting_iterator const& o) {
        *this = o;
    }
    constexpr operation_counting_iterator(operation_counting_iterator&& o) {
        *this = o;
    }

    constexpr operation_counting_iterator& operator=(
        operation_counting_iterator const& o) = default;
    constexpr operation_counting_iterator& operator=(
        operation_counting_iterator&& o) {
        return *this = o;
    }

    friend constexpr It base(operation_counting_iterator const& it) {
        return It(it.base_);
    }

    constexpr decltype(auto) operator*() const { return *It(base_); }

    constexpr decltype(auto) operator[](difference_type n) const {
        return It(base_)[n];
    }

    constexpr operation_counting_iterator& operator++() {
        It tmp(base_);
        base_ = base(++tmp);
        moved_by(1);
        return *this;
    }

    constexpr void operator++(int) { ++*this; }

    constexpr operation_counting_iterator operator++(int)
    requires std::forward_iterator<It>
    {
        auto temp = *this;
        ++*this;
        return temp;
    }

    constexpr operation_counting_iterator& operator--()
    requires std::bidirectional_iterator<It>
    {
        It tmp(base_);
        base_ = base(--tmp);
        moved_by(-1);
        return *this;
    }

    constexpr operation_counting_iterator operator--(int)
    requires std::bidirectional_iterator<It>
    {
        auto temp = *this;
        --*this;
        return temp;
    }

    constexpr operation_counting_iterator& operator+=(difference_type const n)
    requires std::random_access_iterator<It>
    {
        It tmp(base_);
        base_ = base(tmp += n);
        moved_by(n);
        return *this;
    }

    constexpr operation_counting_iterator& operator-=(difference_type const n)
    requires std::random_access_iterator<It>
    {
        It tmp(base_);
        base_ = base(tmp -= n);
        moved_by(-n);
        return *this;
    }

    friend constexpr operation_counting_iterator operator+(
        operation_counting_iterator it, difference_type n)
    requires std::random_access_iterator<It>
    {
        return it += n;
    }

    friend constexpr operation_counting_iterator operator+(
        difference_type n, operation_counting_iterator it)
    requires std::random_access_iterator<It>
    {
        return it += n;
    }

    friend constexpr operation_counting_iterator operator-(
        operation_counting_iterator it, difference_type n)
    requires std::random_access_iterator<It>
    {
        return it -= n;
    }

    friend constexpr difference_type operator-(
        operation_counting_iterator const& x,
        operation_counting_iterator const& y)
    requires std::sized_sentinel_for<It, It>
    {
        return base(x) - base(y);
    }

    constexpr void record_equality_comparison() const {
        if (counts_ != nullptr)
            ++counts_->equal_cmps;
    }

    constexpr bool operator==(operation_counting_iterator const& other) const
    requires std::sentinel_for<It, It>
    {
        record_equality_comparison();
        return It(base_) == It(other.base_);
    }

    friend constexpr bool operator<(operation_counting_iterator const& x,
        operation_counting_iterator const& y)
    requires std::random_access_iterator<It>
    {
        return It(x.base_) < It(y.base_);
    }

    friend constexpr bool operator>(operation_counting_iterator const& x,
        operation_counting_iterator const& y)
    requires std::random_access_iterator<It>
    {
        return It(x.base_) > It(y.base_);
    }

    friend constexpr bool operator<=(operation_counting_iterator const& x,
        operation_counting_iterator const& y)
    requires std::random_access_iterator<It>
    {
        return It(x.base_) <= It(y.base_);
    }

    friend constexpr bool operator>=(operation_counting_iterator const& x,
        operation_counting_iterator const& y)
    requires std::random_access_iterator<It>
    {
        return It(x.base_) >= It(y.base_);
    }

    template <class T>
    void operator,(T const&) = delete;

private:
    constexpr void moved_by(difference_type n) {
        if (counts_ == nullptr)
            return;
        if (n > 0)
            ++counts_->increments;
        else if (n < 0)
            ++counts_->decrements;
        else
            ++counts_->zero_moves;
    }

    decltype(base(std::declval<It>())) base_;
    IteratorOpCounts* counts_ = nullptr;
};
template <class It>
operation_counting_iterator(It) -> operation_counting_iterator<It>;

template <class It>
class sentinel_wrapper {
public:
    explicit sentinel_wrapper() = default;
    constexpr explicit sentinel_wrapper(It const& it) : base_(base(it)) {}
    constexpr bool operator==(It const& other) const {
        // If supported, record statistics about the equality operator call
        // inside `other`.
        if constexpr (requires { other.record_equality_comparison(); }) {
            other.record_equality_comparison();
        }
        return base_ == base(other);
    }
    friend constexpr It base(sentinel_wrapper const& s) { return It(s.base_); }

private:
    decltype(base(std::declval<It>())) base_;
};
template <class It>
sentinel_wrapper(It) -> sentinel_wrapper<It>;

template <class It>
class sized_sentinel {
public:
    explicit sized_sentinel() = default;
    constexpr explicit sized_sentinel(It const& it) : base_(base(it)) {}
    constexpr bool operator==(It const& other) const {
        return base_ == base(other);
    }
    friend constexpr auto operator-(sized_sentinel const& s, It const& i) {
        return s.base_ - base(i);
    }
    friend constexpr auto operator-(It const& i, sized_sentinel const& s) {
        return base(i) - s.base_;
    }
    friend constexpr It base(sized_sentinel const& s) { return It(s.base_); }

private:
    decltype(base(std::declval<It>())) base_;
};
template <class It>
sized_sentinel(It) -> sized_sentinel<It>;

namespace adl {

class Iterator {
public:
    using value_type = int;
    using reference = int&;
    using difference_type = std::ptrdiff_t;

private:
    value_type* ptr_ = nullptr;
    int* iter_moves_ = nullptr;
    int* iter_swaps_ = nullptr;

    constexpr Iterator(int* p, int* iter_moves, int* iter_swaps)
        : ptr_(p)
        , iter_moves_(iter_moves)
        , iter_swaps_(iter_swaps) {}

public:
    constexpr Iterator() = default;
    static constexpr Iterator TrackMoves(int* p, int& iter_moves) {
        return Iterator(p, &iter_moves, /*iter_swaps=*/nullptr);
    }
    static constexpr Iterator TrackSwaps(int& iter_swaps) {
        return Iterator(/*p=*/nullptr, /*iter_moves=*/nullptr, &iter_swaps);
    }
    static constexpr Iterator TrackSwaps(int* p, int& iter_swaps) {
        return Iterator(p, /*iter_moves=*/nullptr, &iter_swaps);
    }

    constexpr int iter_moves() const {
        assert(iter_moves_);
        return *iter_moves_;
    }
    constexpr int iter_swaps() const {
        assert(iter_swaps_);
        return *iter_swaps_;
    }

    constexpr value_type& operator*() const { return *ptr_; }
    constexpr reference operator[](difference_type n) const { return ptr_[n]; }

    friend constexpr Iterator operator+(Iterator i, difference_type n) {
        return Iterator(i.ptr_ + n, i.iter_moves_, i.iter_swaps_);
    }
    friend constexpr Iterator operator+(difference_type n, Iterator i) {
        return i + n;
    }
    constexpr Iterator operator-(difference_type n) const {
        return Iterator(ptr_ - n, iter_moves_, iter_swaps_);
    }
    constexpr difference_type operator-(Iterator rhs) const {
        return ptr_ - rhs.ptr_;
    }
    constexpr Iterator& operator+=(difference_type n) {
        ptr_ += n;
        return *this;
    }
    constexpr Iterator& operator-=(difference_type n) {
        ptr_ -= n;
        return *this;
    }

    constexpr Iterator& operator++() {
        ++ptr_;
        return *this;
    }
    constexpr Iterator operator++(int) {
        Iterator prev = *this;
        ++ptr_;
        return prev;
    }

    constexpr Iterator& operator--() {
        --ptr_;
        return *this;
    }
    constexpr Iterator operator--(int) {
        Iterator prev = *this;
        --ptr_;
        return prev;
    }

    constexpr friend void iter_swap(Iterator a, Iterator b) {
        std::swap(a.ptr_, b.ptr_);
        if (a.iter_swaps_) {
            ++(*a.iter_swaps_);
        }
    }

    constexpr friend value_type&& iter_move(Iterator iter) {
        if (iter.iter_moves_) {
            ++(*iter.iter_moves_);
        }
        return std::move(*iter);
    }

    constexpr friend bool operator==(Iterator const& lhs, Iterator const& rhs) {
        return lhs.ptr_ == rhs.ptr_;
    }
    constexpr friend auto operator<=>(
        Iterator const& lhs, Iterator const& rhs) {
        return lhs.ptr_ <=> rhs.ptr_;
    }
};

} // namespace adl

template <class T>
class rvalue_iterator {
public:
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = T&&;
    using value_type = T;

    rvalue_iterator() = default;
    constexpr rvalue_iterator(T* it) : it_(it) {}

    constexpr reference operator*() const { return std::move(*it_); }

    constexpr rvalue_iterator& operator++() {
        ++it_;
        return *this;
    }

    constexpr rvalue_iterator operator++(int) {
        auto tmp = *this;
        ++it_;
        return tmp;
    }

    constexpr rvalue_iterator& operator--() {
        --it_;
        return *this;
    }

    constexpr rvalue_iterator operator--(int) {
        auto tmp = *this;
        --it_;
        return tmp;
    }

    constexpr rvalue_iterator operator+(difference_type n) const {
        auto tmp = *this;
        tmp.it += n;
        return tmp;
    }

    constexpr friend rvalue_iterator operator+(
        difference_type n, rvalue_iterator iter) {
        iter += n;
        return iter;
    }

    constexpr rvalue_iterator operator-(difference_type n) const {
        auto tmp = *this;
        tmp.it -= n;
        return tmp;
    }

    constexpr difference_type operator-(rvalue_iterator const& other) const {
        return it_ - other.it_;
    }

    constexpr rvalue_iterator& operator+=(difference_type n) {
        it_ += n;
        return *this;
    }

    constexpr rvalue_iterator& operator-=(difference_type n) {
        it_ -= n;
        return *this;
    }

    constexpr reference operator[](difference_type n) const {
        return std::move(it_[n]);
    }

    auto operator<=>(rvalue_iterator const&) const noexcept = default;

private:
    T* it_;
};

template <class T>
rvalue_iterator(T*) -> rvalue_iterator<T>;

static_assert(std::random_access_iterator<rvalue_iterator<int*>>);

// The ProxyDiffTBase allows us to conditionally specify
// Proxy<T>::difference_type which we need in certain situations. For example
// when we want std::weakly_incrementable<Proxy<T>> to be true.
template <class T>
struct ProxyDiffTBase {
    // Add default `operator<=>` so that the derived type, Proxy, can also use
    // the default `operator<=>`
    friend constexpr auto operator<=>(
        ProxyDiffTBase const&, ProxyDiffTBase const&) = default;
};

template <class T>
requires requires { std::iter_difference_t<T>{}; }
struct ProxyDiffTBase<T> {
    using difference_type = std::iter_difference_t<T>;
    // Add default `operator<=>` so that the derived type, Proxy, can also use
    // the default `operator<=>`
    friend constexpr auto operator<=>(
        ProxyDiffTBase const&, ProxyDiffTBase const&) = default;
};

// Proxy
// ======================================================================
// Proxy that can wrap a value or a reference. It simulates C++23's tuple
// but simplified to just hold one argument.
// Note that unlike tuple, this class deliberately doesn't have special handling
// of swap to cause a compilation error if it's used in an algorithm that relies
// on plain swap instead of ranges::iter_swap.
// This class is useful for testing that if algorithms support proxy iterator
// properly, i.e. calling ranges::iter_swap and ranges::iter_move instead of
// plain swap and std::move.

template <class T>
struct Proxy;

template <class T>
inline constexpr bool IsProxy = false;

template <class T>
inline constexpr bool IsProxy<Proxy<T>> = true;

template <class T>
struct Proxy : ProxyDiffTBase<T> {
    T data;

    constexpr T& getData() & { return data; }

    constexpr T const& getData() const& { return data; }

    constexpr T&& getData() && { return static_cast<T&&>(data); }

    constexpr T const&& getData() const&& {
        return static_cast<T const&&>(data);
    }

    // Explicitly declare the copy constructor as defaulted to avoid deprecation
    // of the implicitly declared one because of the user-provided copy
    // assignment operator.
    Proxy(Proxy const&) = default;

    template <class U>
    requires std::constructible_from<T, U&&>
    constexpr Proxy(U&& u) : data{std::forward<U>(u)} {}

    // This constructor covers conversion from cvref of Proxy<U>, including
    // non-const/const versions of copy/move constructor
    template <class Other>
    requires (IsProxy<std::decay_t<Other>> &&
        std::constructible_from<T, decltype(std::declval<Other>().getData())>)
    constexpr Proxy(Other&& other)
        : data{std::forward<Other>(other).getData()} {}

    template <class Other>
    requires (IsProxy<std::decay_t<Other>> &&
        std::assignable_from<T&, decltype(std::declval<Other>().getData())>)
    constexpr Proxy& operator=(Other&& other) {
        data = std::forward<Other>(other).getData();
        return *this;
    }

    // const assignment required to make ProxyIterator model
    // std::indirectly_writable
    template <class Other>
    requires (IsProxy<std::decay_t<Other>> &&
        std::assignable_from<T const&,
            decltype(std::declval<Other>().getData())>)
    constexpr const Proxy& operator=(Other&& other) const {
        data = std::forward<Other>(other).getData();
        return *this;
    }

    // If `T` is a reference type, the implicitly-generated assignment operator
    // will be deleted (and would take precedence over the templated `operator=`
    // above because it's a better match).
    constexpr Proxy& operator=(Proxy const& rhs) {
        data = rhs.data;
        return *this;
    }

    // no specialised swap function that takes const Proxy& and no specialised
    // const member swap Calling swap(Proxy<T>{}, Proxy<T>{}) would fail (pass
    // prvalues)

    // Compare operators are defined for the convenience of the tests
    friend constexpr bool operator==(Proxy const&, Proxy const&)
    requires (std::equality_comparable<T> && !std::is_reference_v<T>)
    = default;

    // Helps compare e.g. `Proxy<int>` and `Proxy<int&>`. Note that the default
    // equality comparison operator is deleted when `T` is a reference type.
    template <class U>
    friend constexpr bool operator==(Proxy const& lhs, Proxy<U> const& rhs)
    requires std::equality_comparable_with<std::decay_t<T>, std::decay_t<U>>
    {
        return lhs.data == rhs.data;
    }

    friend constexpr auto operator<=>(Proxy const&, Proxy const&)
    requires (std::three_way_comparable<T> && !std::is_reference_v<T>)
    = default;

    // Helps compare e.g. `Proxy<int>` and `Proxy<int&>`. Note that the default
    // 3-way comparison operator is deleted when `T` is a reference type.
    template <class U>
    friend constexpr auto operator<=>(Proxy const& lhs, Proxy<U> const& rhs)
    requires std::three_way_comparable_with<std::decay_t<T>, std::decay_t<U>>
    {
        return lhs.data <=> rhs.data;
    }

    // Needed to allow certain types to be weakly_incrementable
    constexpr Proxy& operator++()
    requires (std::weakly_incrementable<std::remove_reference_t<T>>)
    {
        ++data;
        return *this;
    }

    constexpr Proxy operator++(int)
    requires (std::incrementable<std::remove_reference_t<T>>)
    {
        Proxy tmp = *this;
        operator++();
        return tmp;
    }
};

// This is to make ProxyIterator model `std::indirectly_readable`
template <class T, class U, template <class> class TQual,
    template <class> class UQual>
requires requires { typename std::common_reference_t<TQual<T>, UQual<U>>; }
struct std::basic_common_reference<Proxy<T>, Proxy<U>, TQual, UQual> {
    using type = Proxy<std::common_reference_t<TQual<T>, UQual<U>>>;
};

template <class T, class U>
requires requires { typename std::common_type_t<T, U>; }
struct std::common_type<Proxy<T>, Proxy<U>> {
    using type = Proxy<std::common_type_t<T, U>>;
};

// ProxyIterator
// ======================================================================
// It wraps `Base` iterator and when dereferenced it returns a Proxy<ref>
// It simulates C++23's zip_view::iterator but simplified to just wrap
// one base iterator.
// Note it forwards value_type, iter_move, iter_swap. e.g if the base
// iterator is int*,
// operator*    -> Proxy<int&>
// iter_value_t -> Proxy<int>
// iter_move    -> Proxy<int&&>
template <class Base>
struct ProxyIteratorBase {};

template <class Base>
requires std::derived_from<
    typename std::iterator_traits<Base>::iterator_category,
    std::input_iterator_tag>
struct ProxyIteratorBase<Base> {
    using iterator_category = std::input_iterator_tag;
};

template <std::input_iterator Base>
consteval auto get_iterator_concept() {
    if constexpr (std::random_access_iterator<Base>) {
        return std::random_access_iterator_tag{};
    } else if constexpr (std::bidirectional_iterator<Base>) {
        return std::bidirectional_iterator_tag{};
    } else if constexpr (std::forward_iterator<Base>) {
        return std::forward_iterator_tag{};
    } else {
        return std::input_iterator_tag{};
    }
}

template <std::input_iterator Base>
struct ProxyIterator : ProxyIteratorBase<Base> {
    Base base_;

    using iterator_concept = decltype(get_iterator_concept<Base>());
    using value_type = Proxy<std::iter_value_t<Base>>;
    using difference_type = std::iter_difference_t<Base>;

    ProxyIterator()
    requires std::default_initializable<Base>
    = default;

    constexpr ProxyIterator(Base base) : base_{std::move(base)} {}

    template <class T>
    requires std::constructible_from<Base, T&&>
    constexpr ProxyIterator(T&& t) : base_{std::forward<T>(t)} {}

    friend constexpr decltype(auto) base(ProxyIterator const& p) {
        return base(p.base_);
    }

    // Specialization of iter_move
    // If operator* returns Proxy<Foo&>, iter_move will return Proxy<Foo&&>
    // Note std::move(*it) returns Proxy<Foo&>&&, which is not what we want as
    // it will likely result in a copy rather than a move
    friend constexpr Proxy<std::iter_rvalue_reference_t<Base>> iter_move(
        ProxyIterator const& p) noexcept {
        return {xranges::iter_move(p.base_)};
    }

    // Specialization of iter_swap
    // Note std::swap(*x, *y) would fail to compile as operator* returns
    // prvalues and std::swap takes non-const lvalue references
    friend constexpr void iter_swap(
        ProxyIterator const& x, ProxyIterator const& y) noexcept {
        xranges::iter_swap(x.base_, y.base_);
    }

    // to satisfy input_iterator
    constexpr Proxy<std::iter_reference_t<Base>> operator*() const {
        return {*base_};
    }

    constexpr ProxyIterator& operator++() {
        ++base_;
        return *this;
    }

    constexpr void operator++(int) { ++*this; }

    friend constexpr bool operator==(
        ProxyIterator const& x, ProxyIterator const& y)
    requires std::equality_comparable<Base>
    {
        return x.base_ == y.base_;
    }

    // to satisfy forward_iterator
    constexpr ProxyIterator operator++(int)
    requires std::forward_iterator<Base>
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    // to satisfy bidirectional_iterator
    constexpr ProxyIterator& operator--()
    requires std::bidirectional_iterator<Base>
    {
        --base_;
        return *this;
    }

    constexpr ProxyIterator operator--(int)
    requires std::bidirectional_iterator<Base>
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    // to satisfy random_access_iterator
    constexpr ProxyIterator& operator+=(difference_type n)
    requires std::random_access_iterator<Base>
    {
        base_ += n;
        return *this;
    }

    constexpr ProxyIterator& operator-=(difference_type n)
    requires std::random_access_iterator<Base>
    {
        base_ -= n;
        return *this;
    }

    constexpr Proxy<std::iter_reference_t<Base>> operator[](
        difference_type n) const
    requires std::random_access_iterator<Base>
    {
        return {base_[n]};
    }

    friend constexpr bool operator<(
        ProxyIterator const& x, ProxyIterator const& y)
    requires std::random_access_iterator<Base>
    {
        return x.base_ < y.base_;
    }

    friend constexpr bool operator>(
        ProxyIterator const& x, ProxyIterator const& y)
    requires std::random_access_iterator<Base>
    {
        return x.base_ > y.base_;
    }

    friend constexpr bool operator<=(
        ProxyIterator const& x, ProxyIterator const& y)
    requires std::random_access_iterator<Base>
    {
        return x.base_ <= y.base_;
    }

    friend constexpr bool operator>=(
        ProxyIterator const& x, ProxyIterator const& y)
    requires std::random_access_iterator<Base>
    {
        return x.base_ >= y.base_;
    }

    friend constexpr auto operator<=>(
        ProxyIterator const& x, ProxyIterator const& y)
    requires (
        std::random_access_iterator<Base> && std::three_way_comparable<Base>)
    {
        return x.base_ <=> y.base_;
    }

    friend constexpr ProxyIterator operator+(
        ProxyIterator const& x, difference_type n)
    requires std::random_access_iterator<Base>
    {
        return ProxyIterator{x.base_ + n};
    }

    friend constexpr ProxyIterator operator+(
        difference_type n, ProxyIterator const& x)
    requires std::random_access_iterator<Base>
    {
        return ProxyIterator{n + x.base_};
    }

    friend constexpr ProxyIterator operator-(
        ProxyIterator const& x, difference_type n)
    requires std::random_access_iterator<Base>
    {
        return ProxyIterator{x.base_ - n};
    }

    friend constexpr difference_type operator-(
        ProxyIterator const& x, ProxyIterator const& y)
    requires std::random_access_iterator<Base>
    {
        return x.base_ - y.base_;
    }
};
template <class Base>
ProxyIterator(Base) -> ProxyIterator<Base>;

static_assert(std::indirectly_readable<ProxyIterator<int*>>);
static_assert(std::indirectly_writable<ProxyIterator<int*>, Proxy<int>>);
static_assert(std::indirectly_writable<ProxyIterator<int*>, Proxy<int&>>);

template <class Iter>
using Cpp20InputProxyIterator = ProxyIterator<cpp20_input_iterator<Iter>>;

template <class Iter>
using ForwardProxyIterator = ProxyIterator<forward_iterator<Iter>>;

template <class Iter>
using BidirectionalProxyIterator = ProxyIterator<bidirectional_iterator<Iter>>;

template <class Iter>
using RandomAccessProxyIterator = ProxyIterator<random_access_iterator<Iter>>;

template <class Iter>
using ContiguousProxyIterator = ProxyIterator<contiguous_iterator<Iter>>;

template <class BaseSent>
struct ProxySentinel {
    BaseSent base_;

    ProxySentinel() = default;
    constexpr ProxySentinel(BaseSent base) : base_{std::move(base)} {}

    template <class Base>
    requires std::equality_comparable_with<Base, BaseSent>
    friend constexpr bool operator==(
        ProxyIterator<Base> const& p, ProxySentinel const& sent) {
        return p.base_ == sent.base_;
    }
};
template <class BaseSent>
ProxySentinel(BaseSent) -> ProxySentinel<BaseSent>;

template <xranges::input_range Base>
requires xranges::view<Base>
struct ProxyRange {
    Base base_;

    constexpr auto begin() { return ProxyIterator{xranges::begin(base_)}; }

    constexpr auto end() { return ProxySentinel{xranges::end(base_)}; }

    constexpr auto begin() const
    requires xranges::input_range<Base const>
    {
        return ProxyIterator{xranges::begin(base_)};
    }

    constexpr auto end() const
    requires xranges::input_range<Base const>
    {
        return ProxySentinel{xranges::end(base_)};
    }
};

template <xranges::input_range R>
requires xranges::viewable_range<R&&>
ProxyRange(R&&) -> ProxyRange<std::views::all_t<R&&>>;

namespace util {
template <class Derived, class Iter>
class iterator_wrapper {
    Iter iter_;

    using iter_traits = std::iterator_traits<Iter>;

public:
    using iterator_category = typename iter_traits::iterator_category;
    using value_type = typename iter_traits::value_type;
    using difference_type = typename iter_traits::difference_type;
    using pointer = typename iter_traits::pointer;
    using reference = typename iter_traits::reference;

    constexpr iterator_wrapper() : iter_() {}
    constexpr explicit iterator_wrapper(Iter iter) : iter_(iter) {}

    decltype(*iter_) operator*() { return *iter_; }
    decltype(*iter_) operator*() const { return *iter_; }

    decltype(iter_[0]) operator[](difference_type v) const { return iter_[v]; }

    Derived& operator++() {
        ++iter_;
        return static_cast<Derived&>(*this);
    }

    Derived operator++(int) {
        auto tmp = static_cast<Derived&>(*this);
        ++(*this);
        return tmp;
    }

    Derived& operator--() {
        --iter_;
        return static_cast<Derived&>(*this);
    }

    Derived operator--(int) {
        auto tmp = static_cast<Derived&>(*this);
        --(*this);
        return tmp;
    }

    Derived& operator+=(difference_type i) {
        iter_ += i;
        return static_cast<Derived&>(*this);
    }

    Derived& operator-=(difference_type i) {
        iter_ -= i;
        return static_cast<Derived&>(*this);
    }

    friend decltype(iter_ - iter_) operator-(
        iterator_wrapper const& lhs, iterator_wrapper const& rhs) {
        return lhs.iter_ - rhs.iter_;
    }

    friend Derived operator-(Derived iter, difference_type i) {
        iter.iter_ -= i;
        return iter;
    }

    friend Derived operator+(Derived iter, difference_type i) {
        iter.iter_ += i;
        return iter;
    }

    friend Derived operator+(difference_type i, Derived iter) {
        return iter + i;
    }

    friend bool operator==(
        iterator_wrapper const& lhs, iterator_wrapper const& rhs) {
        return lhs.iter_ == rhs.iter_;
    }
    friend bool operator!=(
        iterator_wrapper const& lhs, iterator_wrapper const& rhs) {
        return lhs.iter_ != rhs.iter_;
    }

    friend bool operator>(
        iterator_wrapper const& lhs, iterator_wrapper const& rhs) {
        return lhs.iter_ > rhs.iter_;
    }
    friend bool operator<(
        iterator_wrapper const& lhs, iterator_wrapper const& rhs) {
        return lhs.iter_ < rhs.iter_;
    }
    friend bool operator<=(
        iterator_wrapper const& lhs, iterator_wrapper const& rhs) {
        return lhs.iter_ <= rhs.iter_;
    }
    friend bool operator>=(
        iterator_wrapper const& lhs, iterator_wrapper const& rhs) {
        return lhs.iter_ >= rhs.iter_;
    }
};

class iterator_error : std::runtime_error {
public:
    iterator_error(char const* what) : std::runtime_error(what) {}
};

#if RXX_WITH_EXCEPTIONS
template <class Iter>
class throw_on_move_iterator :
    public iterator_wrapper<throw_on_move_iterator<Iter>, Iter> {
    using base = iterator_wrapper<throw_on_move_iterator<Iter>, Iter>;

    int moves_until_throw_ = 0;

public:
    using difference_type = typename base::difference_type;
    using value_type = typename base::value_type;
    using iterator_category = typename base::iterator_category;

    throw_on_move_iterator() = default;
    throw_on_move_iterator(Iter iter, int moves_until_throw)
        : base(std::move(iter))
        , moves_until_throw_(moves_until_throw) {}

    throw_on_move_iterator(throw_on_move_iterator const& other) : base(other) {}
    throw_on_move_iterator& operator=(throw_on_move_iterator const& other) {
        static_cast<base&>(*this) = other;
        return *this;
    }

    throw_on_move_iterator(throw_on_move_iterator&& other)
        : base(std::move(other))
        , moves_until_throw_(other.moves_until_throw_ - 1) {
        if (moves_until_throw_ == -1)
            throw iterator_error("throw_on_move_iterator");
    }

    throw_on_move_iterator& operator=(throw_on_move_iterator&& other) {
        moves_until_throw_ = other.moves_until_throw_ - 1;
        if (moves_until_throw_ == -1)
            throw iterator_error("throw_on_move_iterator");
        return *this;
    }
};

template <class Iter>
throw_on_move_iterator(Iter) -> throw_on_move_iterator<Iter>;
#endif // TEST_HAS_NO_EXCEPTIONS
} // namespace util

namespace types {
template <class Ptr>
using random_access_iterator_list =
    type_list<Ptr, contiguous_iterator<Ptr>, random_access_iterator<Ptr>>;

template <class Ptr>
using bidirectional_iterator_list =
    concatenate_t<random_access_iterator_list<Ptr>,
        type_list<bidirectional_iterator<Ptr>>>;

template <class Ptr>
using forward_iterator_list = concatenate_t<bidirectional_iterator_list<Ptr>,
    type_list<forward_iterator<Ptr>>>;

template <class Ptr>
using cpp17_input_iterator_list = concatenate_t<forward_iterator_list<Ptr>,
    type_list<cpp17_input_iterator<Ptr>>>;

template <class Ptr>
using cpp20_input_iterator_list = concatenate_t<forward_iterator_list<Ptr>,
    type_list<cpp20_input_iterator<Ptr>, cpp17_input_iterator<Ptr>>>;
} // namespace types

#endif // SUPPORT_TEST_ITERATORS_H
