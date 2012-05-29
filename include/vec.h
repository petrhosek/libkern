#ifndef VEC_H
#define VEC_H

#include <assert.h>
#include <stdlib.h>

#define vec_free(a)    ((a) ? free(__vec_raw(a)), 0 : 0)
#define vec_push(a, v) (__vec_maybegrow(a, 1), (a)[__vec_n(a)++] = (v))
#define vec_count(a)   ((a) ? __vec_n(a) : 0)
#define vec_add(a, n)  (__vec_maybegrow(a,n), __vec_n(a) += (n), &(a)[__vec_n(a)-(n)])
#define vec_last(a)    ((a)[__vec_n(a) - 1])

#define __vec_raw(a) ((int *)(a) - 2)
#define __vec_m(a)   __vec_raw(a)[0]
#define __vec_n(a)   __vec_raw(a)[1]

#define __vec_needgrow(a, n)  ((a) == NULL || __vec_n(a) + n >= __vec_m(a))
#define __vec_maybegrow(a, n) (__vec_needgrow(a, (n)) ? __vec_grow(a, n) : 0)
#define __vec_grow(a, n)      __vec_growf((void **)&(a), (n), sizeof(*(a)))

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

#endif // VEC_H
