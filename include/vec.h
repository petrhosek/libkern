/*
 * This file is part of libkern.
 *
 * libkern is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libkern is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libkern.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VEC_H_
#define VEC_H_

#define vec_init(a, n) ((a) = NULL, __vec_grow(a, n))
#define vec_destroy(a) ((a) ? free(__vec_raw(a)), 0 : 0)
#define vec_size(a) ((a) ? __vec_n(a) : 0)

#define vec_push(a, v) (__vec_maybegrow(a, 1), (a)[__vec_n(a)++] = (v))
#define vec_pop(a) ((a) ? __vec_n(a)-- : 0)

#define vec_add(a, n) (__vec_maybegrow(a, n), __vec_n(a) += (n), &(a)[__vec_n(a) - (n)])

#define vec_first(a) ((a)[0])
#define vec_last(a) ((a)[__vec_n(a) - 1])

#define vec_insert(a, i, v) \
  (__vec_maybegrow(a, 1), __vec_rshift(a, i, __vec_n(a) - (i)), (a)[__vec_n(a)++] = (v))
#define vec_erase(a, i) \
  ((a) ? __vec_lshift(a, i, --__vec_n(a) - (i)) : 0)

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define __vec_raw(a) ((int *)(a) - 2)
#define __vec_m(a) __vec_raw(a)[0]
#define __vec_n(a) __vec_raw(a)[1]

#define __vec_needgrow(a, n) ((a) == NULL || __vec_n(a) + n >= __vec_m(a))
#define __vec_maybegrow(a, n) (__vec_needgrow(a, (n)) ? __vec_grow(a, n) : 0)
#define __vec_grow(a, n) __vec_growf((void **)&(a), (n), sizeof(*(a)))
#define __vec_rshift(a, i, n) memmove(&(a)[(i) + 1], &(a)[(i)], (n) * sizeof(*(a)))
#define __vec_lshift(a, i, n) memmove(&(a)[(i)], &(a)[(i) + 1], (n) * sizeof(*(a)))

static inline void __vec_growf(void **arr, int incr, int size) {
   int m = *arr ? 2 * __vec_m(*arr) + incr : incr + 1;
   void *p = realloc(*arr ? __vec_raw(*arr) : 0, size * m + sizeof(int) * 2);
   assert(p);
   if (p) {
      if (!*arr) ((int *)p)[1] = 0;
      *arr = (void *)((int *)p + 2);
      __vec_m(*arr) = m;
   }
}

#endif // VEC_H_
