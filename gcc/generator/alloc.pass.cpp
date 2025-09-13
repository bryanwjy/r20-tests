// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2023-2025 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include "rxx/generator.h"

#if RXX_SUPPORTS_GENERATOR
#  include <bit>
#  include <cassert>
#  include <memory_resource>
#  include <utility>

namespace xpmr {
using rxx::pmr::generator;
}

template <typename... Args>
xpmr::generator<int> gen(Args...) {
    co_yield 1;
    co_yield 2;
}

struct S {
    template <typename... Args>
    xpmr::generator<char> gen(Args...) {
        co_yield '1';
        co_yield '2';
    }

#  if RXX_CXX23
    template <typename Self, typename... Args>
    xpmr::generator<long long> genx(this Self& self, Args...) {
        co_yield 1LL;
        co_yield 2LL;
    }
#  endif
};

struct bad_size : std::exception {
    bad_size() noexcept : std::exception() {}
    char const* what() const noexcept final {
        return "Invalid size argument during deallocation";
    }
};

struct bad_alignment : std::exception {
    bad_alignment() noexcept : std::exception() {}
    char const* what() const noexcept final {
        return "Invalid allocation argument during deallocation";
    }
};

class tracking_memory_resource : public std::pmr::memory_resource {
public:
    size_t number_of_active_allocations() const noexcept {
        auto count = 0u;
        for (auto current = active.next; current != &active;
             current = current->next)
            ++count;
        return count;
    }
    tracking_memory_resource() noexcept = default;
    tracking_memory_resource(tracking_memory_resource const&) = delete;
    tracking_memory_resource& operator=(
        tracking_memory_resource const&) = delete;
    ~tracking_memory_resource() {
        auto* head = std::exchange(active.next, &active);
        for (auto current = head; current != &active; current = current->next) {
            auto const alignment = 1u << current->alignment_shift;
            auto const header_offset =
                (1u << (current->alignment_shift - default_alignment_shift)) -
                1;
#  if RXX_SUPPORTS_SIZED_DEALLOCATION
            ::operator delete(current - header_offset, current->bytes,
                std::align_val_t(alignment));
#  else
            ::operator delete(
                current - header_offset, std::align_val_t(alignment));
#  endif
        }
    }

private:
    struct alignas(16) allocation_header {
        allocation_header* next;
        size_t bytes : 58;
        size_t alignment_shift : 6;
    };

    static constexpr size_t default_alignment_shift =
        std::popcount(alignof(allocation_header) - 1);

    allocation_header active{
        .next = &active,
        .bytes = 0,
        .alignment_shift = default_alignment_shift,
    };

    void* do_allocate(size_t bytes, size_t alignment) override {
        assert(std::popcount(alignment) == 1);
        alignment = std::max(alignment, alignof(allocation_header));
        auto const alignment_shift =
            static_cast<size_t>(std::popcount(alignment - 1));
        auto const header_block_size = 1u
            << (alignment_shift - default_alignment_shift);
        auto const header_offset = header_block_size - 1;

        auto* ptr = reinterpret_cast<allocation_header*>(::operator new(
            sizeof(allocation_header) * header_block_size + bytes,
            std::align_val_t(alignment)));
        ptr += header_offset;
        auto* header = ::new (ptr) allocation_header{
            .next = this->active.next,
            .bytes = bytes,
            .alignment_shift = alignment_shift,
        };
        this->active.next = header;
        return ::new (header + 1) std::byte[bytes];
    }

    void do_deallocate(void* ptr, size_t bytes, size_t alignment) override {
        if (std::popcount(alignment) != 1) {
            RXX_THROW(bad_alignment());
        }

        alignment = std::max(alignment, alignof(allocation_header));
        auto const alignment_shift =
            static_cast<size_t>(std::popcount(alignment - 1));
        auto const header_offset =
            (1u << (alignment_shift - default_alignment_shift)) - 1;

        for (auto *previous = &active, *next = active.next; next != &active;) {
            auto* current = std::exchange(next, next->next);
            if (current + 1 != ptr) {
                continue;
            }

            if (bytes != current->bytes) {
                RXX_THROW(bad_size());
            }

            if (alignment_shift != current->alignment_shift) {
                RXX_THROW(bad_alignment());
            }

            previous->next = next;
#  if RXX_SUPPORTS_SIZED_DEALLOCATION
            ::operator delete(current - header_offset, current->bytes,
                std::align_val_t(alignment));
#  else
            ::operator delete(
                current - header_offset, std::align_val_t(alignment));
#  endif
            return;
        }
        std::abort();
    }

    bool do_is_equal(
        std::pmr::memory_resource const& r) const noexcept override {
        return this == &r;
    }
};

int main() {
    tracking_memory_resource mr;
    for (auto _ : gen())
        assert(mr.number_of_active_allocations() == 0);

    for (auto _ : gen(std::allocator_arg))
        assert(mr.number_of_active_allocations() == 0);

    for (auto _ : gen(std::allocator_arg, std::pmr::new_delete_resource()))
        assert(mr.number_of_active_allocations() == 0);

#  if RXX_WITH_EXCEPTIONS
    try {
        for (auto _ :
            gen(std::allocator_arg, std::pmr::null_memory_resource())) {}
        assert(false);
    } catch (std::bad_alloc const&) {}
#  endif

    assert(mr.number_of_active_allocations() == 0);

    for (auto _ : gen(std::allocator_arg, &mr))
        assert(mr.number_of_active_allocations() == 1);

    assert(mr.number_of_active_allocations() == 0);

    S s;
    for (auto _ : s.gen(std::allocator_arg, &mr))
        assert(mr.number_of_active_allocations() == 1);

    assert(mr.number_of_active_allocations() == 0);
#  if RXX_CXX23
    for (auto _ : s.genx(std::allocator_arg, &mr))
        assert(mr.number_of_active_allocations() == 1);
#  endif

    assert(mr.number_of_active_allocations() == 0);
}
#else
int main() {
    return 0;
}
#endif