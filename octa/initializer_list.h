/* Initializer list support for OctaSTD.
 *
 * This file is part of OctaSTD. See COPYING.md for futher information.
 */

#ifndef OCTA_INITIALIZER_LIST_H
#define OCTA_INITIALIZER_LIST_H

#include <stddef.h>

#include "octa/range.h"

/* must be in std namespace otherwise the compiler won't know about it */
namespace std {
    template<typename T>
    class initializer_list {
        const T *p_buf;
        size_t p_len;

        initializer_list(const T *v, size_t n): p_buf(v), p_len(n) {}
    public:
        typedef size_t     SizeType;
        typedef ptrdiff_t  DiffType;
        typedef       T    ValType;
        typedef const T   &RefType;
        typedef const T   &ConstRefType;
        typedef const T   *PtrType;
        typedef const T   *ConstPtrType;
        typedef octa::PointerRange<const T> RangeType;
        typedef octa::PointerRange<const T> ConstRangeType;

        initializer_list(): p_buf(nullptr), p_len(0) {}

        size_t length() const { return p_len; }

        const T *data() const { return p_buf; }

        octa::PointerRange<const T> each() {
            return octa::PointerRange<const T>(p_buf, p_len);
        }
    };
}

namespace octa {
    template<typename T> using InitializerList = std::initializer_list<T>;
}

#endif