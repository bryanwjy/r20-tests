// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// REQUIRES: std-at-least-c++23

// <ranges>

// friend constexpr decltype(auto) iter_move(const iterator& x);

#include "../types.h"
#include "rxx/algorithm.h"
#include "rxx/ranges.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

namespace xranges = __RXX ranges;
namespace xviews = __RXX views;

class MoveOnlyInt {
public:
    enum Status {
        constructed,
        move_constructed,
        moved_from_this
    };

    MoveOnlyInt() : ptr_{&data_} {}
    constexpr MoveOnlyInt(int val)
        : data_{
              .val = val,
              .status = constructed,
          },
          ptr_{&data_} {}

    constexpr MoveOnlyInt(MoveOnlyInt&& other) noexcept
        : data_{
              .val = other.data_.val,
              .status = move_constructed,
          }, ptr_{&data_} {
        other.data_.val = -1;
        other.data_.status = moved_from_this;
    }

    constexpr MoveOnlyInt(MoveOnlyInt const&& other) noexcept
        : data_{
              .val = other.data_.val,
              .status = move_constructed,
          }, ptr_{&data_} {
        other.ptr_->val = -1;
        other.ptr_->status = moved_from_this;
    }

    MoveOnlyInt(MoveOnlyInt const&) {
        assert(false);
    } // Should never be called in this test.

    MoveOnlyInt& operator=(
        MoveOnlyInt&&) { // Should never be called in this test.
        assert(false);
        return *this;
    }

    constexpr bool was_normally_constructed() const {
        return data_.status == constructed;
    }
    constexpr bool was_move_constructed() const {
        return data_.status == move_constructed;
    }
    constexpr bool was_moved_from() const {
        return data_.status == moved_from_this;
    }

    friend constexpr bool operator==(MoveOnlyInt const& left, int right) {
        return left.data_.val == right;
    }
    friend constexpr bool operator==(
        MoveOnlyInt const& left, MoveOnlyInt const& right) {
        return left.data_.val == right.data_.val;
    }

private:
    struct data {
        int val = -1;
        Status status = constructed;
    };
    data data_;
    data* ptr_;
};

static_assert(std::movable<MoveOnlyInt>);

struct ProxyRvalueRef {
    MoveOnlyInt&& val;
};

class CommonProxyRvalueRef {
public:
    constexpr CommonProxyRvalueRef(ProxyRvalueRef i) : val_(std::move(i.val)) {}
    constexpr CommonProxyRvalueRef(MoveOnlyInt i) : val_(std::move(i)) {}

    constexpr MoveOnlyInt&& get() { return std::move(val_); }

private:
    MoveOnlyInt val_;
};

template <template <class> class TQual, template <class> class UQual>
struct std::basic_common_reference<ProxyRvalueRef, MoveOnlyInt, TQual, UQual> {
    using type = CommonProxyRvalueRef;
};

template <template <class> class TQual, template <class> class UQual>
struct std::basic_common_reference<MoveOnlyInt, ProxyRvalueRef, TQual, UQual> {
    using type = CommonProxyRvalueRef;
};

static_assert(std::common_reference_with<MoveOnlyInt&&, ProxyRvalueRef>);
static_assert(std::common_reference_with<MoveOnlyInt&&, CommonProxyRvalueRef>);

class ProxyIter {
public:
    using value_type = MoveOnlyInt;
    using difference_type = std::ptrdiff_t;

    constexpr ProxyIter() : ptr_(nullptr) {}
    constexpr explicit ProxyIter(MoveOnlyInt* it) : ptr_(std::move(it)) {}

    constexpr decltype(auto) operator*() const { return *ptr_; }

    constexpr ProxyIter& operator++() {
        ++ptr_;
        return *this;
    }

    constexpr ProxyIter operator++(int) {
        ProxyIter copy = *this;
        ++ptr_;
        return copy;
    }

    constexpr ProxyIter& operator--() {
        --ptr_;
        return *this;
    }

    constexpr ProxyIter operator--(int) {
        ProxyIter copy = *this;
        --ptr_;
        return copy;
    }

    friend bool operator==(ProxyIter const&, ProxyIter const&) = default;

    friend constexpr ProxyRvalueRef iter_move(ProxyIter const iter) {
        return ProxyRvalueRef{xranges::iter_move(iter.ptr_)};
    }

private:
    MoveOnlyInt* ptr_;
};

static_assert(std::forward_iterator<ProxyIter>);

template <std::forward_iterator Iter>
class IterMoveTrackingIterator {
public:
    using value_type = std::iter_value_t<Iter>;
    using difference_type = std::iter_difference_t<Iter>;

    IterMoveTrackingIterator() = default;
    constexpr explicit IterMoveTrackingIterator(Iter iter, bool* flag = nullptr)
        : iter_(std::move(iter))
        , flag_(flag) {}

    constexpr IterMoveTrackingIterator& operator++() {
        ++iter_;
        return *this;
    }

    constexpr IterMoveTrackingIterator operator++(int) {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr decltype(auto) operator*() const { return *iter_; }

    constexpr bool operator==(IterMoveTrackingIterator const& other) const {
        return iter_ == other.iter_;
    }

    friend constexpr decltype(auto) iter_move(
        IterMoveTrackingIterator const& iter) {
        assert(iter.flag_ != nullptr);
        *iter.flag_ = true;
        return xranges::iter_move(iter.iter_);
    }

private:
    Iter iter_ = Iter();
    bool* flag_ = nullptr;
};

static_assert(std::forward_iterator<IterMoveTrackingIterator<int*>> &&
    !std::bidirectional_iterator<IterMoveTrackingIterator<int*>>);

#if !RXX_SUPPORTS_FROM_RANGE

template <typename C, typename R>
constexpr C reserve_and_emplace(R&& arg) {
    C container;
    container.reserve(xranges::size(arg));
    for (auto const val : arg) {
        container.emplace_back(val);
    }
    return container;
}

#endif

constexpr bool test() {
    { // Test `iter_move` when result is true rvalue reference. Test return
      // types.
        using V = std::array<std::array<char, 1>, 2>;
        using Pattern = std::array<char, 1>;
        using JWV = xranges::join_with_view<xranges::owning_view<V>,
            xranges::owning_view<Pattern>>;

        JWV jwv(
            V{
                {{'0'}, {'1'}}
        },
            Pattern{','});

        {
            auto it = jwv.begin();
            std::same_as<char&&> decltype(auto) v_rref1 = iter_move(it);
            std::same_as<char&&> decltype(auto) v_rref2 =
                iter_move(std::as_const(it));
            std::same_as<char&&> decltype(auto) v_rref3 =
                xranges::iter_move(it);
            std::same_as<char&&> decltype(auto) v_rref4 =
                xranges::iter_move(std::as_const(it));
            assert(
                xranges::equal(std::array{v_rref1, v_rref2, v_rref3, v_rref4},
                    xviews::repeat('0', 4)));

            ++it; // `it` points to element of `Pattern` from here
            std::same_as<char&&> decltype(auto) pattern_rref1 = iter_move(it);
            std::same_as<char&&> decltype(auto) pattern_rref2 =
                iter_move(std::as_const(it));
            std::same_as<char&&> decltype(auto) pattern_rref3 =
                xranges::iter_move(it);
            std::same_as<char&&> decltype(auto) pattern_rref4 =
                xranges::iter_move(std::as_const(it));
            assert(xranges::equal(std::array{pattern_rref1, pattern_rref2,
                                      pattern_rref3, pattern_rref4},
                xviews::repeat(',', 4)));
        }

        {
            auto cit = std::prev(std::as_const(jwv).end());
            std::same_as<char const&&> decltype(auto) cv_rref1 = iter_move(cit);
            std::same_as<char const&&> decltype(auto) cv_rref2 =
                iter_move(std::as_const(cit));
            std::same_as<char const&&> decltype(auto) cv_rref3 =
                xranges::iter_move(cit);
            std::same_as<char const&&> decltype(auto) cv_rref4 =
                xranges::iter_move(std::as_const(cit));
            assert(xranges::equal(
                std::array{cv_rref1, cv_rref2, cv_rref3, cv_rref4},
                xviews::repeat('1', 4)));

            cit--; // `cit` points to element of `Pattern` from here
            std::same_as<char const&&> decltype(auto) cpattern_rref1 =
                iter_move(cit);
            std::same_as<char const&&> decltype(auto) cpattern_rref2 =
                iter_move(std::as_const(cit));
            std::same_as<char const&&> decltype(auto) cpattern_rref3 =
                xranges::iter_move(cit);
            std::same_as<char const&&> decltype(auto) cpattern_rref4 =
                xranges::iter_move(std::as_const(cit));
            assert(xranges::equal(std::array{cpattern_rref1, cpattern_rref2,
                                      cpattern_rref3, cpattern_rref4},
                xviews::repeat(',', 4)));
        }
    }

    { // Test `iter_move` when result is true rvalue reference. Test moving.
        using Inner = std::vector<MoveOnlyInt>;
        using V = std::vector<Inner>;
        using Pattern = std::vector<MoveOnlyInt>;
        using JWV = xranges::join_with_view<xranges::owning_view<V>,
            xranges::owning_view<Pattern>>;

#if RXX_SUPPORTS_FROM_RANGE
        // requires from_ranges ctor to work
        V v;
        v.reserve(2);
        v.emplace_back(xranges::to<Inner>(xviews::iota(0, 4)));
        v.emplace_back(xranges::to<Inner>(xviews::iota(12, 16)));

        JWV jwv(std::move(v), xranges::to<Pattern>(xviews::iota(4, 12)));
        assert(xranges::all_of(jwv, &MoveOnlyInt::was_normally_constructed));
#else
        V v;
        v.reserve(2);
        v.emplace_back(reserve_and_emplace<Inner>(xviews::iota(0, 4)));
        v.emplace_back(reserve_and_emplace<Inner>(xviews::iota(12, 16)));
        JWV jwv(
            std::move(v), reserve_and_emplace<Pattern>(xviews::iota(4, 12)));
        assert(xranges::all_of(jwv, &MoveOnlyInt::was_normally_constructed));
#endif

        {
            std::vector<MoveOnlyInt> values;
            values.reserve(8);

            auto it = jwv.begin();
            values.emplace_back(iter_move(it));
            ++it;
            values.emplace_back(iter_move(std::as_const(it)));
            it++;
            values.emplace_back(xranges::iter_move(it));
            ++it;
            values.emplace_back(xranges::iter_move(std::as_const(it)));
            it++; // `it` points to element of `Pattern` from here
            values.emplace_back(iter_move(it));
            ++it;
            values.emplace_back(iter_move(std::as_const(it)));
            it++;
            values.emplace_back(xranges::iter_move(it));
            ++it;
            values.emplace_back(xranges::iter_move(std::as_const(it)));

            assert(xranges::equal(values, xviews::iota(0, 8)));
            assert(xranges::all_of(values, &MoveOnlyInt::was_move_constructed));
        }

        {
            std::vector<MoveOnlyInt> values;
            values.reserve(8);

            auto cit = std::prev(std::as_const(jwv).end());
            values.emplace_back(iter_move(cit));
            cit--;
            values.emplace_back(iter_move(std::as_const(cit)));
            --cit;
            values.emplace_back(xranges::iter_move(cit));
            cit--;
            values.emplace_back(xranges::iter_move(std::as_const(cit)));
            --cit; // `it` points to element of `Pattern` from here
            values.emplace_back(iter_move(cit));
            cit--;
            values.emplace_back(iter_move(std::as_const(cit)));
            --cit;
            values.emplace_back(xranges::iter_move(cit));
            cit--;
            values.emplace_back(xranges::iter_move(std::as_const(cit)));

            assert(
                xranges::equal(xviews::reverse(values), xviews::iota(8, 16)));
            assert(xranges::all_of(values, &MoveOnlyInt::was_move_constructed));
        }

        assert(xranges::all_of(jwv, &MoveOnlyInt::was_moved_from));
    }

    { // Test `iter_move` when result is proxy rvalue reference type, which is
      // different from
        // range_rvalue_reference_t<InnerRng> and
        // range_rvalue_reference_t<Pattern>.
        using Inner = std::vector<MoveOnlyInt>;
        using V = std::vector<Inner>;
        using Pattern = xranges::subrange<ProxyIter, ProxyIter>;
        using JWV = xranges::join_with_view<xranges::owning_view<V>, Pattern>;

        static_assert(!std::same_as<xranges::range_rvalue_reference_t<V>,
            xranges::range_rvalue_reference_t<JWV>>);
        static_assert(!std::same_as<xranges::range_rvalue_reference_t<Pattern>,
            xranges::range_rvalue_reference_t<JWV>>);
        static_assert(std::same_as<CommonProxyRvalueRef,
            xranges::range_rvalue_reference_t<JWV>>);

#if RXX_SUPPORTS_FROM_RANGE
        V v;
        v.reserve(2);
        v.emplace_back(xranges::to<Inner>(xviews::iota(0, 4)));
        v.emplace_back(xranges::to<Inner>(xviews::iota(12, 16)));
        auto pattern =
            xranges::to<std::vector<MoveOnlyInt>>(xviews::iota(4, 12));
#else
        V v;
        v.reserve(2);
        v.emplace_back(reserve_and_emplace<Inner>(xviews::iota(0, 4)));
        v.emplace_back(reserve_and_emplace<Inner>(xviews::iota(12, 16)));
        auto pattern =
            reserve_and_emplace<std::vector<MoveOnlyInt>>(xviews::iota(4, 12));

#endif

        Pattern pattern_as_subrange(ProxyIter{pattern.data()},
            ProxyIter{pattern.data() + pattern.size()});

        JWV jwv(std::move(v), pattern_as_subrange);
        assert(xranges::all_of(jwv, &MoveOnlyInt::was_normally_constructed));

        {
            std::vector<MoveOnlyInt> values;
            values.reserve(8);

            auto it = jwv.begin();
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref1 =
                iter_move(it);
            values.emplace_back(rref1.get());
            ++it;
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref2 =
                iter_move(std::as_const(it));
            values.emplace_back(rref2.get());
            it++;
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref3 =
                xranges::iter_move(it);
            values.emplace_back(rref3.get());
            ++it;
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref4 =
                xranges::iter_move(std::as_const(it));
            values.emplace_back(rref4.get());
            it++; // `it` points to element of `Pattern` from here
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref5 =
                iter_move(it);
            values.emplace_back(rref5.get());
            ++it;
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref6 =
                iter_move(std::as_const(it));
            values.emplace_back(rref6.get());
            it++;
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref7 =
                xranges::iter_move(it);
            values.emplace_back(rref7.get());
            ++it;
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref8 =
                xranges::iter_move(std::as_const(it));
            values.emplace_back(rref8.get());

            assert(xranges::equal(values, xviews::iota(0, 8)));
            assert(xranges::all_of(values, &MoveOnlyInt::was_move_constructed));
        }

        {
            std::vector<MoveOnlyInt> values;
            values.reserve(8);

            auto cit = std::prev(std::as_const(jwv).end());
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref1 =
                iter_move(cit);
            values.emplace_back(rref1.get());
            cit--;
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref2 =
                iter_move(std::as_const(cit));
            values.emplace_back(rref2.get());
            --cit;
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref3 =
                xranges::iter_move(cit);
            values.emplace_back(rref3.get());
            cit--;
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref4 =
                xranges::iter_move(std::as_const(cit));
            values.emplace_back(rref4.get());
            --cit; // `it` points to element of `Pattern` from here
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref5 =
                iter_move(cit);
            values.emplace_back(rref5.get());
            cit--;
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref6 =
                iter_move(std::as_const(cit));
            values.emplace_back(rref6.get());
            --cit;
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref7 =
                xranges::iter_move(cit);
            values.emplace_back(rref7.get());
            cit--;
            std::same_as<CommonProxyRvalueRef> decltype(auto) rref8 =
                xranges::iter_move(std::as_const(cit));
            values.emplace_back(rref8.get());

            assert(
                xranges::equal(xviews::reverse(values), xviews::iota(8, 16)));
            assert(xranges::all_of(values, &MoveOnlyInt::was_move_constructed));
        }

        assert(xranges::all_of(jwv, &MoveOnlyInt::was_moved_from));
    }

    { // Make sure `iter_move` calls underlying's iterator `iter_move` (not
      // `std::move(*i)`).
        using Inner = std::vector<int>;
        using InnerTrackingIter = IterMoveTrackingIterator<Inner::iterator>;
        using TrackingInner = xranges::subrange<InnerTrackingIter>;
        using Pattern = std::array<int, 1>;
        using PatternTrackingIter = IterMoveTrackingIterator<Pattern::iterator>;
        using TrackingPattern = xranges::subrange<PatternTrackingIter>;
        using JWV =
            xranges::join_with_view<std::span<TrackingInner>, TrackingPattern>;

        std::array<Inner, 2> v{
            {{1}, {2}}
        };
        Pattern pat{-1};

        bool v_moved = false;
        std::array<TrackingInner, 2> tracking_v{
            TrackingInner(InnerTrackingIter(v[0].begin(), &v_moved),
                InnerTrackingIter(v[0].end())),
            TrackingInner(InnerTrackingIter(v[1].begin()),
                InnerTrackingIter(v[1].end()))};

        bool pat_moved = false;
        TrackingPattern tracking_pat(
            PatternTrackingIter(pat.begin(), &pat_moved),
            PatternTrackingIter(pat.end()));

        JWV jwv(tracking_v, tracking_pat);
        auto it = jwv.begin();

        // Test calling `iter_move` when `it` points to element of `v`
        assert(!v_moved);
        assert(iter_move(it) == 1);
        assert(v_moved);

        // Test calling `iter_move` when `it` points to element of `pat`
        ++it;
        assert(!pat_moved);
        assert(iter_move(it) == -1);
        assert(pat_moved);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
