/* Ranges for OctaSTD.
 *
 * This file is part of OctaSTD. See COPYING.md for futher information.
 */

#ifndef OCTA_RANGE_H
#define OCTA_RANGE_H

#include <stddef.h>

#include "octa/types.h"
#include "octa/utility.h"
#include "octa/type_traits.h"

namespace octa {
    struct InputRangeTag {};
    struct OutputRangeTag {};
    struct ForwardRangeTag {};
    struct BidirectionalRangeTag {};
    struct RandomAccessRangeTag {};

    template<typename T> using RangeCategory  = typename T::Category;
    template<typename T> using RangeSize      = typename T::SizeType;
    template<typename T> using RangeValue     = typename T::ValType;
    template<typename T> using RangeReference = typename T::RefType;

    template<typename T>
    struct __OctaRangeIterator {
        __OctaRangeIterator(): p_range() {}
        explicit __OctaRangeIterator(const T &range): p_range(range) {}
        __OctaRangeIterator &operator++() {
            p_range.pop_first();
            return *this;
        }
        RangeReference<T> operator*() {
            return p_range.first();
        }
        RangeReference<T> operator*() const {
            return p_range.first();
        }
        bool operator!=(__OctaRangeIterator) const { return !p_range.empty(); }
    private:
        T p_range;
    };

    template<typename R>
    RangeSize<R> __octa_pop_first_n(R &range, RangeSize<R> n) {
        for (RangeSize<R> i = 0; i < n; ++i) {
            if (range.empty()) return i;
            range.pop_first();
        }
        return n;
    }

    template<typename R>
    RangeSize<R> __octa_pop_last_n(R &range, RangeSize<R> n) {
        for (RangeSize<R> i = 0; i < n; ++i) {
            if (range.empty()) return i;
            range.pop_last();
        }
        return n;
    }

    template<typename B, typename C, typename V, typename R = V &,
             typename S = size_t
    > struct InputRange {
        typedef C Category;
        typedef S SizeType;
        typedef V ValType;
        typedef R RefType;

        __OctaRangeIterator<B> begin() {
            return __OctaRangeIterator<B>((const B &)*this);
        }
        __OctaRangeIterator<B> end() {
            return __OctaRangeIterator<B>();
        }

        SizeType pop_first_n(SizeType n) {
            return __octa_pop_first_n<B>(*((B *)this), n);
        }

        SizeType pop_last_n(SizeType n) {
            return __octa_pop_last_n<B>(*((B *)this), n);
        }
    };

    template<typename V, typename R = V &, typename S = size_t>
    struct OutputRange {
        typedef OutputRangeTag Category;
        typedef S SizeType;
        typedef V ValType;
        typedef R RefType;
    };

    template<typename T>
    struct ReverseRange: InputRange<ReverseRange<T>,
        RangeCategory<T>, RangeValue<T>, RangeReference<T>, RangeSize<T>
    > {
    private:
        typedef RangeReference<T> r_ref;
        typedef RangeSize<T> r_size;

        T p_range;

    public:
        ReverseRange(): p_range() {}

        ReverseRange(const T &range): p_range(range) {}

        ReverseRange(const ReverseRange &it): p_range(it.p_range) {}

        ReverseRange(ReverseRange &&it): p_range(move(it.p_range)) {}

        ReverseRange &operator=(const ReverseRange &v) {
            p_range = v.p_range;
            return *this;
        }
        ReverseRange &operator=(ReverseRange &&v) {
            p_range = move(v.p_range);
            return *this;
        }
        ReverseRange &operator=(const T &v) {
            p_range = v;
            return *this;
        }
        ReverseRange &operator=(T &&v) {
            p_range = move(v);
            return *this;
        }

        bool empty() const {
            return p_range.empty();
        }

        r_size length() const {
            return p_range.length();
        }

        void pop_first() {
            p_range.pop_last();
        }
        void pop_last() {
            p_range.pop_first();
        }

        r_size pop_first_n(r_size n) {
            return p_range.pop_first_n(n);
        }
        r_size pop_last_n(r_size n) {
            return p_range.pop_last_n(n);
        }

        bool operator==(const ReverseRange &v) const {
            return p_range == v.p_range;
        }
        bool operator!=(const ReverseRange &v) const {
            return p_range != v.p_range;
        }

        r_ref first() {
            return p_range.last();
        }
        r_ref first() const {
            return p_range.last();
        }

        r_ref last() {
            return p_range.first();
        }
        r_ref last() const {
            return p_range.first();
        }

        r_ref operator[](r_size i) {
            return p_range[length() - i - 1];
        }
        r_ref operator[](r_size i) const {
            return p_range[length() - i - 1];
        }

        ReverseRange<T> slice(r_size start, r_size end) {
            r_size len = p_range.length();
            return ReverseRange<T>(p_range.slice(len - end, len - start));
        }
    };

    template<typename T>
    ReverseRange<T> make_reverse_range(const T &it) {
        return ReverseRange<T>(it);
    }

    template<typename T>
    struct MoveRange: InputRange<MoveRange<T>,
        RangeCategory<T>, RangeValue<T>, RangeValue<T> &&, RangeSize<T>
    > {
    private:
        typedef RangeValue<T>   r_val;
        typedef RangeValue<T> &&r_ref;
        typedef RangeSize<T>    r_size;

        T p_range;

    public:
        MoveRange(): p_range() {}

        MoveRange(const T &range): p_range(range) {}

        MoveRange(const MoveRange &it): p_range(it.p_range) {}

        MoveRange(MoveRange &&it): p_range(move(it.p_range)) {}

        MoveRange &operator=(const MoveRange &v) {
            p_range = v.p_range;
            return *this;
        }
        MoveRange &operator=(MoveRange &&v) {
            p_range = move(v.p_range);
            return *this;
        }
        MoveRange &operator=(const T &v) {
            p_range = v;
            return *this;
        }
        MoveRange &operator=(T &&v) {
            p_range = move(v);
            return *this;
        }

        bool empty() const {
            return p_range.empty();
        }
        r_size length() const {
            return p_range.length();
        }

        void pop_first() {
            p_range.pop_first();
        }
        void pop_last() {
            p_range.pop_last();
        }

        r_size pop_first_n(r_size n) {
            return p_range.pop_first_n(n);
        }
        r_size pop_last_n(r_size n) {
            return p_range.pop_last_n(n);
        }

        bool operator==(const MoveRange &v) const {
            return p_range == v.p_range;
        }
        bool operator!=(const MoveRange &v) const {
            return p_range != v.p_range;
        }

        r_ref first() {
            return move(p_range.first());
        }
        r_ref last() {
            return move(p_range.last());
        }

        r_ref operator[](r_size i) {
            return move(p_range[i]);
        }

        MoveRange<T> slice(r_size start, r_size end) {
            return MoveRange<T>(p_range.slice(start, end));
        }

        void put(const r_val &v) {
            p_range.put(v);
        }
        void put(r_val &&v) {
            p_range.put(move(v));
        }
    };

    template<typename T>
    MoveRange<T> make_move_range(const T &it) {
        return MoveRange<T>(it);
    }

    template<typename T>
    struct NumberRange: InputRange<NumberRange<T>, ForwardRangeTag, T> {
        NumberRange(): p_a(0), p_b(0), p_step(0) {}
        NumberRange(const NumberRange &it): p_a(it.p_a), p_b(it.p_b),
            p_step(it.p_step) {}
        NumberRange(T a, T b, T step = T(1)): p_a(a), p_b(b),
            p_step(step) {}
        NumberRange(T v): p_a(0), p_b(v), p_step(1) {}

        bool operator==(const NumberRange &v) const {
            return p_a == v.p_a && p_b == v.p_b && p_step == v.p_step;
        }
        bool operator!=(const NumberRange &v) const {
            return p_a != v.p_a || p_b != v.p_b || p_step != v.p_step;
        }

        bool empty() const { return p_a * p_step >= p_b * p_step; }
        void pop_first() { p_a += p_step; }
        T &first() { return p_a; }

    private:
        T p_a, p_b, p_step;
    };

    template<typename T>
    NumberRange<T> range(T a, T b, T step = T(1)) {
        return NumberRange<T>(a, b, step);
    }

    template<typename T>
    NumberRange<T> range(T v) {
        return NumberRange<T>(v);
    }

    template<typename T>
    struct PointerRange: InputRange<PointerRange<T>, RandomAccessRangeTag, T> {
        PointerRange(): p_beg(nullptr), p_end(nullptr) {}
        PointerRange(const PointerRange &v): p_beg(v.p_beg),
            p_end(v.p_end) {}
        PointerRange(T *beg, T *end): p_beg(beg), p_end(end) {}
        PointerRange(T *beg, size_t n): p_beg(beg), p_end(beg + n) {}

        PointerRange &operator=(const PointerRange &v) {
            p_beg = v.p_beg;
            p_end = v.p_end;
            return *this;
        }

        bool operator==(const PointerRange &v) const {
            return p_beg == v.p_beg && p_end == v.p_end;
        }
        bool operator!=(const PointerRange &v) const {
            return p_beg != v.p_beg || p_end != v.p_end;
        }

        /* satisfy InputRange / ForwardRange */
        bool empty() const { return p_beg == nullptr; }

        void pop_first() {
            if (p_beg == nullptr) return;
            if (++p_beg == p_end) p_beg = p_end = nullptr;
        }

        size_t pop_first_n(size_t n) {
            T *obeg = p_beg;
            size_t olen = p_end - p_beg;
            p_beg += n;
            if (p_beg >= p_end) {
                p_beg = p_end = nullptr;
                return olen;
            }
            return p_beg - obeg;
        }

              T &first()       { return *p_beg; }
        const T &first() const { return *p_beg; }

        /* satisfy BidirectionalRange */
        void pop_last() {
            if (p_end-- == p_beg) { p_end = nullptr; return; }
            if (p_end   == p_beg) p_beg = p_end = nullptr;
        }

        size_t pop_last_n(size_t n) {
            T *oend = p_end;
            size_t olen = p_end - p_beg;
            p_end -= n;
            if (p_end <= (p_beg + 1)) {
                p_beg = p_end = nullptr;
                return olen;
            }
            return oend - p_end;
        }

              T &last()       { return *(p_end - 1); }
        const T &last() const { return *(p_end - 1); }

        /* satisfy RandomAccessRange */
        size_t length() const { return p_end - p_beg; }

        PointerRange slice(size_t start, size_t end) {
            return PointerRange(p_beg + start, p_beg + end);
        }

              T &operator[](size_t i)       { return p_beg[i]; }
        const T &operator[](size_t i) const { return p_beg[i]; }

        /* satisfy OutputRange */
        void put(const T &v) {
            *(p_beg++) = v;
        }
        void put(T &&v) {
            *(p_beg++) = move(v);
        }

    private:
        T *p_beg, *p_end;
    };

    template<typename T, typename S>
    struct EnumeratedValue {
        S index;
        T value;
    };

    template<typename T>
    struct EnumeratedRange: InputRange<EnumeratedRange<T>,
        InputRangeTag, RangeValue<T>,
        EnumeratedValue<RangeReference<T>, RangeSize<T>>,
        RangeSize<T>
    > {
    private:
        typedef RangeReference<T> r_ref;
        typedef RangeSize<T> r_size;

        T p_range;
        r_size p_index;

    public:
        EnumeratedRange(): p_range(), p_index(0) {}

        EnumeratedRange(const T &range): p_range(range), p_index(0) {}

        EnumeratedRange(const EnumeratedRange &it):
            p_range(it.p_range), p_index(it.p_index) {}

        EnumeratedRange(EnumeratedRange &&it):
            p_range(move(it.p_range)), p_index(it.p_index) {}

        EnumeratedRange &operator=(const EnumeratedRange &v) {
            p_range = v.p_range;
            p_index = v.p_index;
            return *this;
        }
        EnumeratedRange &operator=(EnumeratedRange &&v) {
            p_range = move(v.p_range);
            p_index = v.p_index;
            return *this;
        }
        EnumeratedRange &operator=(const T &v) {
            p_range = v;
            p_index = 0;
            return *this;
        }
        EnumeratedRange &operator=(T &&v) {
            p_range = move(v);
            p_index = 0;
            return *this;
        }

        bool empty() const {
            return p_range.empty();
        }

        void pop_first() {
            ++p_index; p_range.pop_first();
        }

        r_size pop_first_n(r_size n) {
            return p_range.pop_first_n(n);
        }

        EnumeratedValue<r_ref, r_size> first() {
            return EnumeratedValue<r_ref, r_size> { p_index, p_range.first() };
        }
        EnumeratedValue<r_ref, r_size> first() const {
            return EnumeratedValue<r_ref, r_size> { p_index, p_range.first() };
        }

        bool operator==(const EnumeratedRange &v) const {
            return p_range == v.p_range;
        }
        bool operator!=(const EnumeratedRange &v) const {
            return p_range != v.p_range;
        }
    };

    template<typename T>
    EnumeratedRange<T> enumerate(const T &it) {
        return EnumeratedRange<T>(it);
    }
}

#endif