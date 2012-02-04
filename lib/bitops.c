#include "bitops.h"

#define BITOP_WORD(nr) ((nr) / BITS_PER_LONG)

/**
 * Find the next set bit in a memory region.
 *
 * @param addr address to base the search on
 * @param offset bitnumber to start searching at
 * @param size bitmap size in bits
 */
unsigned long find_next_bit(const unsigned long *addr, unsigned long size, unsigned long offset) {
    const unsigned long *p = addr + BITOP_WORD(offset);
    unsigned long result = offset & ~(BITS_PER_LONG-1);
    unsigned long tmp;

    if (offset >= size)
        return size;
    size -= result;
    offset %= BITS_PER_LONG;
    if (offset) {
        tmp = *(p++);
        tmp &= (~0UL << offset);
        if (size < BITS_PER_LONG)
            goto found_first;
        if (tmp)
            goto found_middle;
        size -= BITS_PER_LONG;
        result += BITS_PER_LONG;
    }
    while (size & ~(BITS_PER_LONG-1)) {
        if ((tmp = *(p++)))
            goto found_middle;
        result += BITS_PER_LONG;
        size -= BITS_PER_LONG;
    }
    if (!size)
        return result;
    tmp = *p;

found_first:
    tmp &= (~0UL >> (BITS_PER_LONG - size));
    if (tmp == 0UL) /* any bits set? */
        return result + size; /* nope */
found_middle:
    return result + __ffs(tmp);
}

/**
 * Find the next cleared bit in a memory region.
 *
 * @param addr address to base the search on
 * @param offset bitnumber to start searching at
 * @param size bitmap size in bits
 */
unsigned long find_next_zero_bit(const unsigned long *addr, unsigned long size, unsigned long offset) {
    const unsigned long *p = addr + BITOP_WORD(offset);
    unsigned long result = offset & ~(BITS_PER_LONG-1);
    unsigned long tmp;

    if (offset >= size)
        return size;
    size -= result;
    offset %= BITS_PER_LONG;
    if (offset) {
        tmp = *(p++);
        tmp |= ~0UL >> (BITS_PER_LONG - offset);
        if (size < BITS_PER_LONG)
            goto found_first;
        if (~tmp)
            goto found_middle;
        size -= BITS_PER_LONG;
        result += BITS_PER_LONG;
    }
    while (size & ~(BITS_PER_LONG-1)) {
        if (~(tmp = *(p++)))
            goto found_middle;
        result += BITS_PER_LONG;
        size -= BITS_PER_LONG;
    }
    if (!size)
        return result;
    tmp = *p;

found_first:
    tmp |= ~0UL << size;
    if (tmp == ~0UL) /* any bits zero? */
        return result + size; /* nope */
found_middle:
    return result + ffz(tmp);
}

/**
 * Find the first set bit in a memory region.
 * @param addr address to start the search at
 *
 * @param size maximum size to search
 * @return bit number of the first set bit
 */
unsigned long find_first_bit(const unsigned long *addr, unsigned long size) {
    const unsigned long *p = addr;
    unsigned long result = 0;
    unsigned long tmp;

    while (size & ~(BITS_PER_LONG-1)) {
        if ((tmp = *(p++)))
            goto found;
        result += BITS_PER_LONG;
        size -= BITS_PER_LONG;
    }
    if (!size)
        return result;

    tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
    if (tmp == 0UL) /* any bits set? */
        return result + size; /* nope */
found:
    return result + __ffs(tmp);
}

/**
 * Find the first cleared bit in a memory region.
 *
 * @param addr address to start the search at
 * @param size maximum size to search
 * @return The number of the first cleared bit
 */
unsigned long find_first_zero_bit(const unsigned long *addr, unsigned long size) {
    const unsigned long *p = addr;
    unsigned long result = 0;
    unsigned long tmp;

    while (size & ~(BITS_PER_LONG-1)) {
        if (~(tmp = *(p++)))
            goto found;
        result += BITS_PER_LONG;
        size -= BITS_PER_LONG;
    }
    if (!size)
        return result;

    tmp = (*p) | (~0UL << size);
    if (tmp == ~0UL) /* any bits zero? */
        return result + size; /* nope */
found:
    return result + ffz(tmp);
}

/**
 * Find the last set bit in a memory region.
 *
 * @param addr address to start the search at
 * @param size  maximum size to search
 * @return bit number of the first set bit, or size
 */
unsigned long find_last_bit(const unsigned long *addr, unsigned long size) {
    unsigned long words;
    unsigned long tmp;

    /* start at final word */
    words = size / BITS_PER_LONG;

    /* partial final word? */
    if (size & (BITS_PER_LONG-1)) {
        tmp = (addr[words] & (~0UL >> (BITS_PER_LONG
                     - (size & (BITS_PER_LONG-1)))));
        if (tmp)
            goto found;
    }

    while (words) {
        tmp = addr[--words];
        if (tmp) {
found:
            return words * BITS_PER_LONG + fls(tmp);
        }
    }

    /* not found */
    return size;
}
