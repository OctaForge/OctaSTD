/* Ranges for OctaSTD.
 *
 * This file is part of OctaSTD. See COPYING.md for futher information.
 */

#ifndef OCTA_RANGE_H
#define OCTA_RANGE_H

#include <stddef.h>

#include "octa/types.h"
#include "octa/utility.h"

namespace octa {
    struct InputRangeTag {};
    struct OutputRangeTag {};
    struct ForwardRangeTag {};
    struct BidirectionalRangeTag {};
    struct RandomAccessRangeTag {};

    template<typename T>
    struct RangeTraits {
        typedef typename T::range_category range_category;
        typedef typename T::size_type      size_type;
        typedef typename T::value_type     value_type;
        typedef typename T::reference      reference;
    };

    template<typename T>
    struct __OctaRangeIterator {
        __OctaRangeIterator(): p_range() {}
        explicit __OctaRangeIterator(const T &range): p_range(range) {}
        __OctaRangeIterator &operator++() {
            p_range.pop_first();
            return *this;
        }
        typename RangeTraits<T>::reference operator*() {
            return p_range.first();
        }
        typename RangeTraits<T>::reference operator*() const {
            return p_range.first();
        }
        bool operator!=(__OctaRangeIterator) const { return !p_range.empty(); }
    private:
        T p_range;
    };

    template<typename B, typename C, typename V, typename R = V &,
             typename S = size_t
    > struct InputRange {
        typedef C range_category;
        typedef S size_type;
        typedef V value_type;
        typedef R reference;

        __OctaRangeIterator<B> begin() {
            return __OctaRangeIterator<B>((const B &)*this);
        }
        __OctaRangeIterator<B> end() {
            return __OctaRangeIterator<B>();
        }
    };

    template<typename V, typename R = V &, typename S = size_t>
    struct OutputRange {
        typedef OutputRangeTag range_category;
        typedef S size_type;
        typedef V value_type;
        typedef R reference;
    };

    template<typename T>
    struct ReverseRange: InputRange<ReverseRange<T>,
        typename RangeTraits<T>::range_category,
        typename RangeTraits<T>::value_type,
        typename RangeTraits<T>::reference,
        typename RangeTraits<T>::size_type
    > {
    private:
        typedef typename RangeTraits<T>::reference r_ref;
        typedef typename RangeTraits<T>::size_type r_size;

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
            p_range = forward<T>(v);
            return *this;
        }

        bool empty() const { return p_range.empty(); }

        r_size length() const { return p_range.length(); }

        void pop_first() { p_range.pop_last (); }
        void pop_last () { p_range.pop_first(); }

        bool operator==(const ReverseRange &v) const {
            return p_range == v.p_range;
        }
        bool operator!=(const ReverseRange &v) const {
            return p_range != v.p_range;
        }

        r_ref first()       { return p_range.last(); }
        r_ref first() const { return p_range.last(); }

        r_ref last()       { return p_range.first(); }
        r_ref last() const { return p_range.first(); }

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

    private:
        T p_range;
    };

    template<typename T>
    ReverseRange<T> make_reverse_range(const T &it) {
        return ReverseRange<T>(it);
    }

    template<typename T>
    struct MoveRange: InputRange<MoveRange<T>,
        typename RangeTraits<T>::range_category,
        typename RangeTraits<T>::value_type,
        typename RangeTraits<T>::value_type &&,
        typename RangeTraits<T>::size_type
    > {
    private:
        typedef typename RangeTraits<T>::value_type   r_val;
        typedef typename RangeTraits<T>::value_type &&r_ref;
        typedef typename RangeTraits<T>::size_type    r_size;

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
            p_range = forward<T>(v);
            return *this;
        }

        bool   empty () const { return p_range.empty (); }
        r_size length() const { return p_range.length(); }

        void pop_first() { p_range.pop_first(); }
        void pop_last () { p_range.pop_last (); }

        bool operator==(const MoveRange &v) const {
            return p_range == v.p_range;
        }
        bool operator!=(const MoveRange &v) const {
            return p_range != v.p_range;
        }

        r_ref first() { return move(p_range.first()); }
        r_ref last () { return move(p_range.last()); }

        r_ref operator[](r_size i) {
            return move(p_range[i]);
        }

        MoveRange<T> slice(r_size start, r_size end) {
            return MoveRange<T>(p_range.slice(start, end));
        }

        void put(r_val &v) { p_range.put(v); }

    private:
        T p_range;
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
        NumberRange(T a, T b, T step = 1): p_a(a), p_b(b), p_step(step) {}
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
    NumberRange<T> range(T a, T b, T step = 1) {
        return NumberRange<T>(a, b, step);
    }

    template<typename T>
    NumberRange<T> range(T v) {
        return NumberRange<T>(v);
    }

    template<typename T>
    struct PointerRange: InputRange<PointerRange<T>, RandomAccessRangeTag, T> {
        PointerRange(): p_beg(nullptr), p_end(nullptr) {}
        PointerRange(const PointerRange &v): p_beg(v.p_beg), p_end(v.p_end) {}
        PointerRange(T *beg, T *end): p_beg(beg), p_end(end) {}
        PointerRange(T *beg, size_t n): p_beg(beg), p_end(beg + n) {}

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

              T &first()       { return *p_beg; }
        const T &first() const { return *p_beg; }

        /* satisfy BidirectionalRange */
        void pop_last() {
            if (p_end-- == p_beg) { p_end = nullptr; return; }
            if (p_end   == p_beg) p_beg = p_end = nullptr;
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
        void put(const T &v) { *(p_beg++) = v; }

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
        InputRangeTag,  typename RangeTraits<T>::value_type,
        EnumeratedValue<typename RangeTraits<T>::reference,
                        typename RangeTraits<T>::size_type>,
        typename RangeTraits<T>::size_type
    > {
    private:
        typedef typename RangeTraits<T>::reference r_ref;
        typedef typename RangeTraits<T>::size_type r_size;

    public:
        EnumeratedRange(): p_range(), p_index(0) {}
        EnumeratedRange(const T &range): p_range(range), p_index(0) {}
        EnumeratedRange(const EnumeratedRange &it): p_range(it.p_range),
            p_index(it.p_index) {}
        EnumeratedRange(EnumeratedRange &&it): p_range(move(it.p_range)),
            p_index(it.p_index) {}

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
            p_range = forward<T>(v);
            p_index = 0;
            return *this;
        }

        bool empty() const { return p_range.empty(); }

        void pop_first() { ++p_index; p_range.pop_first(); }

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

    private:
        T p_range;
        r_size p_index;
    };

    template<typename T>
    EnumeratedRange<T> enumerate(const T &it) {
        return EnumeratedRange<T>(it);
    }
}

#endif