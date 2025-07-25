// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// test if zip_view models input_range, forward_range, bidirectional_range,
//                         random_access_range, contiguous_range, common_range
//                         sized_range

#include "rxx/ranges.h"
#include "types.h"

#include <cassert>
#include <concepts>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

void testConceptPair() {
    int buffer1[2] = {1, 2};
    int buffer2[3] = {1, 2, 3};
    {
        xranges::zip_view v{
            ContiguousCommonView{buffer1}, ContiguousCommonView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::random_access_range<View>);
        static_assert(!xranges::contiguous_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{
            ContiguousNonCommonView{buffer1}, ContiguousNonCommonView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::random_access_range<View>);
        static_assert(!xranges::contiguous_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{ContiguousNonCommonSized{buffer1},
            ContiguousNonCommonSized{buffer2}};
        using View = decltype(v);
        static_assert(xranges::random_access_range<View>);
        static_assert(!xranges::contiguous_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{
            SizedRandomAccessView{buffer1}, ContiguousCommonView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::random_access_range<View>);
        static_assert(!xranges::contiguous_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{
            SizedRandomAccessView{buffer1}, SizedRandomAccessView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::random_access_range<View>);
        static_assert(!xranges::contiguous_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{NonSizedRandomAccessView{buffer1},
            NonSizedRandomAccessView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::random_access_range<View>);
        static_assert(!xranges::contiguous_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{
            BidiCommonView{buffer1}, SizedRandomAccessView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::bidirectional_range<View>);
        static_assert(!xranges::random_access_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{BidiCommonView{buffer1}, BidiCommonView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::bidirectional_range<View>);
        static_assert(!xranges::random_access_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{BidiCommonView{buffer1}, ForwardSizedView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::forward_range<View>);
        static_assert(!xranges::bidirectional_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{
            BidiNonCommonView{buffer1}, ForwardSizedView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::forward_range<View>);
        static_assert(!xranges::bidirectional_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{
            ForwardSizedView{buffer1}, ForwardSizedView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::forward_range<View>);
        static_assert(!xranges::bidirectional_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{
            ForwardSizedNonCommon{buffer1}, ForwardSizedView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::forward_range<View>);
        static_assert(!xranges::bidirectional_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{
            InputCommonView{buffer1}, ForwardSizedView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::input_range<View>);
        static_assert(!xranges::forward_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{InputCommonView{buffer1}, InputCommonView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::input_range<View>);
        static_assert(!xranges::forward_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{
            InputNonCommonView{buffer1}, InputCommonView{buffer2}};
        using View = decltype(v);
        static_assert(xranges::input_range<View>);
        static_assert(!xranges::forward_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }
}

void testConceptTuple() {
    int buffer1[2] = {1, 2};
    int buffer2[3] = {1, 2, 3};
    int buffer3[4] = {1, 2, 3, 4};

    {
        xranges::zip_view v{ContiguousCommonView{buffer1},
            ContiguousCommonView{buffer2}, ContiguousCommonView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::random_access_range<View>);
        static_assert(!xranges::contiguous_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{ContiguousNonCommonView{buffer1},
            ContiguousNonCommonView{buffer2}, ContiguousNonCommonView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::random_access_range<View>);
        static_assert(!xranges::contiguous_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{ContiguousNonCommonSized{buffer1},
            ContiguousNonCommonSized{buffer2},
            ContiguousNonCommonSized{buffer3}};
        using View = decltype(v);
        static_assert(xranges::random_access_range<View>);
        static_assert(!xranges::contiguous_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{SizedRandomAccessView{buffer1},
            ContiguousCommonView{buffer2}, ContiguousCommonView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::random_access_range<View>);
        static_assert(!xranges::contiguous_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{SizedRandomAccessView{buffer1},
            SizedRandomAccessView{buffer2}, SizedRandomAccessView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::random_access_range<View>);
        static_assert(!xranges::contiguous_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{NonSizedRandomAccessView{buffer1},
            NonSizedRandomAccessView{buffer2},
            NonSizedRandomAccessView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::random_access_range<View>);
        static_assert(!xranges::contiguous_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{BidiCommonView{buffer1},
            SizedRandomAccessView{buffer2}, SizedRandomAccessView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::bidirectional_range<View>);
        static_assert(!xranges::random_access_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{BidiCommonView{buffer1}, BidiCommonView{buffer2},
            BidiCommonView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::bidirectional_range<View>);
        static_assert(!xranges::random_access_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{BidiCommonView{buffer1}, ForwardSizedView{buffer2},
            ForwardSizedView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::forward_range<View>);
        static_assert(!xranges::bidirectional_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{BidiNonCommonView{buffer1},
            ForwardSizedView{buffer2}, ForwardSizedView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::forward_range<View>);
        static_assert(!xranges::bidirectional_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{ForwardSizedView{buffer1},
            ForwardSizedView{buffer2}, ForwardSizedView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::forward_range<View>);
        static_assert(!xranges::bidirectional_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{ForwardSizedNonCommon{buffer1},
            ForwardSizedView{buffer2}, ForwardSizedView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::forward_range<View>);
        static_assert(!xranges::bidirectional_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{InputCommonView{buffer1}, ForwardSizedView{buffer2},
            ForwardSizedView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::input_range<View>);
        static_assert(!xranges::forward_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{InputCommonView{buffer1}, InputCommonView{buffer2},
            InputCommonView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::input_range<View>);
        static_assert(!xranges::forward_range<View>);
        static_assert(xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }

    {
        xranges::zip_view v{InputNonCommonView{buffer1},
            InputCommonView{buffer2}, InputCommonView{buffer3}};
        using View = decltype(v);
        static_assert(xranges::input_range<View>);
        static_assert(!xranges::forward_range<View>);
        static_assert(!xranges::common_range<View>);
        static_assert(!xranges::sized_range<View>);
    }
}

using OutputIter = cpp17_output_iterator<int*>;
static_assert(std::output_iterator<OutputIter, int>);

struct OutputView : xranges::view_base {
    OutputIter begin() const;
    sentinel_wrapper<OutputIter> end() const;
};
static_assert(xranges::output_range<OutputView, int>);
static_assert(!xranges::input_range<OutputView>);

template <class... Ts>
concept zippable = requires { typename xranges::zip_view<Ts...>; };

// output_range is not supported
static_assert(!zippable<OutputView>);
static_assert(!zippable<SimpleCommon, OutputView>);
static_assert(zippable<SimpleCommon>);
