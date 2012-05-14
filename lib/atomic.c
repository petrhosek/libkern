#include "atomic.h"

#include <assert.h>

#define LOGSIZE 4

__atomic_flag_base flag_table[1 << LOGSIZE] = {
  ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT,
  ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT,
  ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT,
  ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT,
};

bool atomic_flag_test_and_set_explicit(volatile __atomic_flag_base *__a, memory_order __m) {
  // Redundant synchronize if built-in for lock is a full barrier.
  if (__m != memory_order_acquire && __m != memory_order_acq_rel)
    __sync_synchronize();
  return __sync_lock_test_and_set(&__a->_M_i, 1);
}

void atomic_flag_clear_explicit(volatile __atomic_flag_base *__a, memory_order __m) {
  assert(__m != memory_order_consume);
  assert(__m != memory_order_acquire);
  assert(__m != memory_order_acq_rel);

  __sync_lock_release(&__a->_M_i);
  if (__m != memory_order_acquire && __m != memory_order_acq_rel)
    __sync_synchronize();
}

void __atomic_flag_wait_explicit(volatile __atomic_flag_base *__a, memory_order __m) {
  while (atomic_flag_test_and_set_explicit(__a, __m)) { };
}

volatile __atomic_flag_base *__atomic_flag_for_address(const volatile void *__z) {
  uintptr_t __u = (uintptr_t)__z;
  __u += (__u >> 2) + (__u << 4);
  __u += (__u >> 7) + (__u << 5);
  __u += (__u >> 17) + (__u << 13);
  if (sizeof(uintptr_t) > 4)
    __u += (__u >> 31);
  __u &= ~((~((uintptr_t)0)) << LOGSIZE);
  return flag_table + __u;
}
