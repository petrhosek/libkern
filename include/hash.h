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

#ifndef HASH_H
#define HASH_H

#include <stdint.h>
#include <unistd.h>

/*
 * Knuth recommends primes in approximately golden ratio to the maximum
 * integer representable by a machine word for multiplicative _hashing.
 * Chuck Lever verified the effectiveness of this technique:
 * http://www.citi.umich.edu/techreports/reports/citi-tr-00-1.pdf
 *
 * These primes are chosen to be bit-sparse, that is operations on them can
 * use shifts and additions instead of multiplications for machines where
 * multiplications are slow.
 */

/* 2^31 + 2^29 - 2^25 + 2^22 - 2^19 - 2^16 + 1 */
#define GOLDEN_RATIO_PRIME_32 0x9e370001UL
/*  2^63 + 2^61 - 2^57 + 2^54 - 2^51 - 2^18 + 1 */
#define GOLDEN_RATIO_PRIME_64 0x9e37fffffffc0001UL

#if __WORDSIZE == 32
#define GOLDEN_RATIO_PRIME GOLDEN_RATIO_PRIME_32
#define hash_long(val, bits) hash_32(val, bits)
#elif __WORDSIZE == 64
#define hash_long(val, bits) hash_64(val, bits)
#define GOLDEN_RATIO_PRIME GOLDEN_RATIO_PRIME_64
#endif

static inline uint64_t hash_64(uint64_t val, unsigned int bits) {
  uint64_t hash = val;

  uint64_t n = hash;
  n <<= 18;
  hash -= n;
  n <<= 33;
  hash -= n;
  n <<= 3;
  hash += n;
  n <<= 3;
  hash -= n;
  n <<= 4;
  hash += n;
  n <<= 2;
  hash += n;

  // High bits are more random, so use them
  return hash >> (64 - bits);
}

static inline uint32_t hash_32(uint32_t val, unsigned int bits) {
  uint32_t hash = val * GOLDEN_RATIO_PRIME_32;

  // High bits are more random, so use them
  return hash >> (32 - bits);
}

static inline unsigned long hash_ptr(const void *ptr, unsigned int bits) {
  return hash_long((unsigned long)ptr, bits);
}

static inline unsigned long hash_internal(const void *data, unsigned int len) {
  unsigned char *p = (unsigned char *)data;
  unsigned char *e = *p + len;
  uint32_t h = 0xfeedbeef;

  while (p < e) {
    h ^= (uint32_t)(*p++);
    h *= 0x9e3779b9;
  }

  return h;
}

#endif // HASH_H
