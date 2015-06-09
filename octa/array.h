/* Static array implementation for OctaSTD.
 *
 * This file is part of OctaSTD. See COPYING.md for futher information.
 */

#ifndef OCTA_ARRAY_H
#define OCTA_ARRAY_H

#include <stddef.h>

#include "octa/algorithm.h"
#include "octa/range.h"
#include "octa/string.h"

namespace octa {

template<typename T, octa::Size N>
struct Array {
    using Size = octa::Size;
    using Difference = octa::Ptrdiff;
    using Value = T;
    using Reference = T &;
    using ConstReference = const T &;
    using Pointer = T *;
    using ConstPointer = const T *;
    using Range = octa::PointerRange<T>;
    using ConstRange = octa::PointerRange<const T>;
    using ReverseRange = octa::ReverseRange<Range>;
    using ConstReverseRange = octa::ReverseRange<ConstRange>;

    T &operator[](Size i) { return p_buf[i]; }
    const T &operator[](Size i) const { return p_buf[i]; }

    T &at(Size i) { return p_buf[i]; }
    const T &at(Size i) const { return p_buf[i]; }

    T &front() { return p_buf[0]; }
    const T &front() const { return p_buf[0]; }

    T &back() { return p_buf[(N > 0) ? (N - 1) : 0]; }
    const T &back() const { return p_buf[(N > 0) ? (N - 1) : 0]; }

    Size size() const { return N; }

    bool empty() const { return N == 0; }

    bool in_range(Size idx) { return idx < N; }
    bool in_range(int idx) { return idx >= 0 && Size(idx) < N; }
    bool in_range(const T *ptr) {
        return ptr >= &p_buf[0] && ptr < &p_buf[N];
    }

    T *data() { return p_buf; }
    const T *data() const { return p_buf; }

    Range each() {
        return Range(p_buf, p_buf + N);
    }
    ConstRange each() const {
        return ConstRange(p_buf, p_buf + N);
    }
    ConstRange ceach() const {
        return ConstRange(p_buf, p_buf + N);
    }

    ReverseRange reach() {
        return each().reach();
    }
    ConstReverseRange reach() const {
        return each().reach();
    }
    ConstReverseRange creach() const {
        return ceach().reach();
    }

    void swap(Array &v) {
        octa::swap_ranges(each(), v.each());
    }

    T p_buf[(N > 0) ? N : 1];
};

} /* namespace octa */

#endif