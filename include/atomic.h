#ifndef ATOMIC_H_
#define ATOMIC_H_

#include <stdbool.h>
#include <inttypes.h>
#include <wchar.h>

/**
 * @defgroup atomics Atomics
 *
 * Components for performing atomic operations.
 * @{
 */

// Enumeration for memory_order
typedef enum memory_order {
  memory_order_relaxed,
  memory_order_consume,
  memory_order_acquire,
  memory_order_release,
  memory_order_acq_rel,
  memory_order_seq_cst
} memory_order;

// Base for atomic_flag.
typedef struct __atomic_flag_base {
  bool _M_i;
} __atomic_flag_base;

#define ATOMIC_FLAG_INIT { false }

#define _ATOMIC_MEMBER_ ((__a)->_M_i)

// Pod base structs for atomic integral types.
struct __atomic_bool_base {
  bool _M_i;
};

struct __atomic_char_base {
  char _M_i;
};

struct __atomic_schar_base {
  signed char _M_i;
};

struct __atomic_uchar_base {
  unsigned char _M_i;
};

struct __atomic_short_base {
  short _M_i;
};

struct __atomic_ushort_base {
  unsigned short _M_i;
};

struct __atomic_int_base {
  int _M_i;
};

struct __atomic_uint_base {
  unsigned int _M_i;
};

struct __atomic_long_base {
  long _M_i;
};

struct __atomic_ulong_base {
  unsigned long _M_i;
};

struct __atomic_llong_base {
  long long _M_i;
};

struct __atomic_ullong_base {
  unsigned long long _M_i;
};

struct __atomic_wchar_t_base {
  wchar_t _M_i;
};

typedef struct __atomic_flag_base     atomic_flag;
typedef struct __atomic_address_base  atomic_address;
typedef struct __atomic_bool_base     atomic_bool;
typedef struct __atomic_char_base     atomic_char;
typedef struct __atomic_schar_base    atomic_schar;
typedef struct __atomic_uchar_base    atomic_uchar;
typedef struct __atomic_short_base    atomic_short;
typedef struct __atomic_ushort_base   atomic_ushort;
typedef struct __atomic_int_base      atomic_int;
typedef struct __atomic_uint_base     atomic_uint;
typedef struct __atomic_long_base     atomic_long;
typedef struct __atomic_ulong_base    atomic_ulong;
typedef struct __atomic_llong_base    atomic_llong;
typedef struct __atomic_ullong_base   atomic_ullong;
typedef struct __atomic_wchar_t_base  atomic_wchar_t;
typedef struct __atomic_short_base    atomic_char16_t;
typedef struct __atomic_int_base      atomic_char32_t;

#define atomic_is_lock_free(__a)          \
  false

#define atomic_load_explicit(__a, __x) \
  _ATOMIC_LOAD_(__a, __x)

#define atomic_load(__a) \
  atomic_load_explicit(__a, memory_order_seq_cst)

#define atomic_store_explicit(__a, __m, __x) \
  _ATOMIC_STORE_(__a, __m, __x)

#define atomic_store(__a, __m) \
  atomic_store_explicit(__a, __m, memory_order_seq_cst)

#define atomic_exchange_explicit(__a, __m, __x) \
  _ATOMIC_MODIFY_(__a, =, __m, __x)

#define atomic_exchange(__a, __m) \
  atomic_exchange_explicit(__a, __m, memory_order_seq_cst)

#define atomic_compare_exchange_explicit(__a, __e, __m, __x, __y) \
  _ATOMIC_CMPEXCHNG_(__a, __e, __m, __x)

#define atomic_compare_exchange(__a, __e, __m) \
  _ATOMIC_CMPEXCHNG_(__a, __e, __m, memory_order_seq_cst)

#define atomic_fetch_add_explicit(__a, __m, __x) \
  _ATOMIC_MODIFY_(__a, +=, __m, __x)

#define atomic_fetch_add(__a, __m) \
  atomic_fetch_add_explicit(__a, __m, memory_order_seq_cst)

#define atomic_fetch_sub_explicit(__a, __m, __x) \
  _ATOMIC_MODIFY_(__a, -=, __m, __x)

#define atomic_fetch_sub(__a, __m) \
  atomic_fetch_sub_explicit(__a, __m, memory_order_seq_cst)

#define atomic_fetch_and_explicit(__a, __m, __x) \
  _ATOMIC_MODIFY_(__a, &=, __m, __x)

#define atomic_fetch_and(__a, __m) \
  atomic_fetch_and_explicit(__a, __m, memory_order_seq_cst)

#define atomic_fetch_or_explicit(__a, __m, __x) \
  _ATOMIC_MODIFY_(__a, |=, __m, __x)

#define atomic_fetch_or(__a, __m) \
  atomic_fetch_or_explicit(__a, __m, memory_order_seq_cst)

#define atomic_fetch_xor_explicit(__a, __m, __x) \
  _ATOMIC_MODIFY_(__a, ^=, __m, __x)

#define atomic_fetch_xor(__a, __m) \
  atomic_fetch_xor_explicit(__a, __m, memory_order_seq_cst)

// Typedefs for other atomic integral types.
typedef atomic_schar    atomic_int_least8_t;
typedef atomic_uchar    atomic_uint_least8_t;
typedef atomic_short    atomic_int_least16_t;
typedef atomic_ushort   atomic_uint_least16_t;
typedef atomic_int      atomic_int_least32_t;
typedef atomic_uint     atomic_uint_least32_t;
typedef atomic_llong    atomic_int_least64_t;
typedef atomic_ullong   atomic_uint_least64_t;

typedef atomic_schar    atomic_int_fast8_t;
typedef atomic_uchar    atomic_uint_fast8_t;
typedef atomic_short    atomic_int_fast16_t;
typedef atomic_ushort   atomic_uint_fast16_t;
typedef atomic_int      atomic_int_fast32_t;
typedef atomic_uint     atomic_uint_fast32_t;
typedef atomic_llong    atomic_int_fast64_t;
typedef atomic_ullong   atomic_uint_fast64_t;

typedef atomic_long     atomic_intptr_t;
typedef atomic_ulong    atomic_uintptr_t;

typedef atomic_long     atomic_ssize_t;
typedef atomic_ulong    atomic_size_t;

typedef atomic_llong    atomic_intmax_t;
typedef atomic_ullong   atomic_uintmax_t;

typedef atomic_long     atomic_ptrdiff_t;

// Accessor functions for base atomic_flag type.
bool atomic_flag_test_and_set_explicit(volatile __atomic_flag_base*, memory_order);

inline bool atomic_flag_test_and_set(volatile __atomic_flag_base* __a) {
  return atomic_flag_test_and_set_explicit(__a, memory_order_seq_cst);
}

void atomic_flag_clear_explicit(volatile __atomic_flag_base*, memory_order);

inline void atomic_flag_clear(volatile __atomic_flag_base* __a) {
  atomic_flag_clear_explicit(__a, memory_order_seq_cst);
}

void __atomic_flag_wait_explicit(volatile __atomic_flag_base*, memory_order);

volatile __atomic_flag_base *__atomic_flag_for_address(const volatile void* __z) __attribute__((const));

// Implementation specific defines.
#define _ATOMIC_LOAD_(__a, __x) \
  ({ volatile __typeof__ _ATOMIC_MEMBER_* __p = &_ATOMIC_MEMBER_; \
     volatile atomic_flag* __g = __atomic_flag_for_address(__p);  \
    __atomic_flag_wait_explicit(__g, __x); \
    __typeof__ _ATOMIC_MEMBER_ __r = *__p; \
    atomic_flag_clear_explicit(__g, __x); \
    __r; })

#define _ATOMIC_STORE_(__a, __m, __x) \
  ({ volatile __typeof__ _ATOMIC_MEMBER_* __p = &_ATOMIC_MEMBER_; \
    __typeof__(__m) __v = (__m); \
    volatile atomic_flag* __g = __atomic_flag_for_address(__p);   \
    __atomic_flag_wait_explicit(__g, __x); \
    *__p = __v; \
    atomic_flag_clear_explicit(__g, __x); \
    __v; })

#define _ATOMIC_MODIFY_(__a, __o, __m, __x) \
  ({ volatile __typeof__ _ATOMIC_MEMBER_* __p = &_ATOMIC_MEMBER_; \
    __typeof__(__m) __v = (__m); \
    volatile atomic_flag* __g = __atomic_flag_for_address(__p);   \
    __atomic_flag_wait_explicit(__g, __x); \
    __typeof__ _ATOMIC_MEMBER_ __r = *__p; \
    *__p __o __v; \
    atomic_flag_clear_explicit(__g, __x); \
    __r; })

#define _ATOMIC_CMPEXCHNG_(__a, __e, __m, __x) \
  ({ volatile __typeof__ _ATOMIC_MEMBER_* __p = &_ATOMIC_MEMBER_; \
    __typeof__(__e) __q = (__e); \
    __typeof__(__m) __v = (__m); \
    bool __r; \
    volatile atomic_flag* __g = __atomic_flag_for_address(__p); \
    __atomic_flag_wait_explicit(__g, __x);\
    __typeof__ _ATOMIC_MEMBER_ __t__ = *__p; \
    if (__t__ == *__q) { *__p = __v; __r = true; } \
    else { *__q = __t__; __r = false; } \
    atomic_flag_clear_explicit(__g, __x); \
    __r; })

/* @} group atomics */

#endif // ATOMIC_H
