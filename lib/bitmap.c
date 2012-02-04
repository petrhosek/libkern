#include "bitmap.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Bitmaps provide an array of bits, implemented using an an array of unsigned
 * longs.  The number of valid bits in a given bitmap does _not_ need to be an
 * exact multiple of BITS_PER_LONG.
 *
 * The possible unused bits in the last, partially used word of a bitmap are
 * 'don't care'.  The implementation makes no particular effort to keep them
 * zero.  It ensures that their value will not affect the results of any
 * operation.  The bitmap operations that return Boolean (bitmap_empty, for
 * example) or scalar (bitmap_weight, for example) results carefully filter
 * out these unused bits from impacting their results.
 *
 * These operations actually hold to a slightly stronger rule: if you don't
 * input any bitmaps to these ops that have some unused bits set, then they
 * won't output any set unused bits in output bitmaps.
 *
 * The byte ordering of bitmaps is more natural on little endian
 * architectures.  See the big-endian headers include/asm-ppc64/bitops.h and
 * include/asm-s390/bitops.h for the best explanations of this ordering.
 */

bool __bitmap_empty(const unsigned long *bitmap, int bits) {
    int k, lim = bits/BITS_PER_LONG;
    for (k = 0; k < lim; ++k)
        if (bitmap[k])
            return 0;

    if (bits % BITS_PER_LONG)
        if (bitmap[k] & BITMAP_LAST_WORD_MASK(bits))
            return 0;

    return 1;
}

bool __bitmap_full(const unsigned long *bitmap, int bits) {
    int k, lim = bits/BITS_PER_LONG;
    for (k = 0; k < lim; ++k)
        if (~bitmap[k])
            return 0;

    if (bits % BITS_PER_LONG)
        if (~bitmap[k] & BITMAP_LAST_WORD_MASK(bits))
            return 0;

    return 1;
}

bool __bitmap_equal(const unsigned long *bitmap1, const unsigned long *bitmap2, int bits) {
    int k, lim = bits/BITS_PER_LONG;
    for (k = 0; k < lim; ++k)
        if (bitmap1[k] != bitmap2[k])
            return 0;

    if (bits % BITS_PER_LONG)
        if ((bitmap1[k] ^ bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
            return 0;

    return 1;
}

void __bitmap_complement(unsigned long *dst, const unsigned long *src, int bits) {
    int k, lim = bits/BITS_PER_LONG;
    for (k = 0; k < lim; ++k)
        dst[k] = ~src[k];

    if (bits % BITS_PER_LONG)
        dst[k] = ~src[k] & BITMAP_LAST_WORD_MASK(bits);
}

/**
 * Logical right shift of the bits in a bitmap.
 *
 * Shifting right (dividing) means moving bits in the MS -> LS bit direction.
 * Zeros are fed into the vacated MS positions and the LS bits shifted off the
 * bottom are lost.
 *
 * @param dst destination bitmap
 * @param src source bitmap
 * @param shift shift by this many bits
 * @param bits bitmap size, in bits
 */
void __bitmap_shift_right(unsigned long *dst, const unsigned long *src, int shift, int bits) {
    int k, lim = BITS_TO_LONGS(bits), left = bits % BITS_PER_LONG;
    int off = shift/BITS_PER_LONG, rem = shift % BITS_PER_LONG;
    unsigned long mask = (1UL << left) - 1;
    for (k = 0; off + k < lim; ++k) {
        unsigned long upper, lower;

        /*
         * If shift is not word aligned, take lower rem bits of
         * word above and make them the top rem bits of result.
         */
        if (!rem || off + k + 1 >= lim)
            upper = 0;
        else {
            upper = src[off + k + 1];
            if (off + k + 1 == lim - 1 && left)
                upper &= mask;
        }
        lower = src[off + k];
        if (left && off + k == lim - 1)
            lower &= mask;
        dst[k] = upper << (BITS_PER_LONG - rem) | lower >> rem;
        if (left && k == lim - 1)
            dst[k] &= mask;
    }
    if (off)
        memset(&dst[lim - off], 0, off*sizeof(unsigned long));
}

/**
 * Logical left shift of the bits in a bitmap.
 *
 * Shifting left (multiplying) means moving bits in the LS -> MS direction.
 * Zeros are fed into the vacated LS bit positions and those MS bits shifted
 * off the top are lost.
 *
 * @param dst destination bitmap
 * @param src source bitmap
 * @param shift shift by this many bits
 * @param bits bitmap size, in bits
 */
void __bitmap_shift_left(unsigned long *dst, const unsigned long *src, int shift, int bits) {
    int k, lim = BITS_TO_LONGS(bits), left = bits % BITS_PER_LONG;
    int off = shift/BITS_PER_LONG, rem = shift % BITS_PER_LONG;
    for (k = lim - off - 1; k >= 0; --k) {
        unsigned long upper, lower;

        /*
         * If shift is not word aligned, take upper rem bits of
         * word below and make them the bottom rem bits of result.
         */
        if (rem && k > 0)
            lower = src[k - 1];
        else
            lower = 0;
        upper = src[k];
        if (left && k == lim - 1)
            upper &= (1UL << left) - 1;
        dst[k + off] = lower  >> (BITS_PER_LONG - rem) | upper << rem;
        if (left && k + off == lim - 1)
            dst[k + off] &= (1UL << left) - 1;
    }
    if (off)
        memset(dst, 0, off*sizeof(unsigned long));
}

bool __bitmap_and(unsigned long *dst, const unsigned long *bitmap1, const unsigned long *bitmap2, int bits) {
    int k;
    int nr = BITS_TO_LONGS(bits);
    unsigned long result = 0;

    for (k = 0; k < nr; k++)
        result |= (dst[k] = bitmap1[k] & bitmap2[k]);
    return result != 0;
}

void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1, const unsigned long *bitmap2, int bits) {
    int k;
    int nr = BITS_TO_LONGS(bits);

    for (k = 0; k < nr; k++)
        dst[k] = bitmap1[k] | bitmap2[k];
}

void __bitmap_xor(unsigned long *dst, const unsigned long *bitmap1, const unsigned long *bitmap2, int bits) {
    int k;
    int nr = BITS_TO_LONGS(bits);

    for (k = 0; k < nr; k++)
        dst[k] = bitmap1[k] ^ bitmap2[k];
}

bool __bitmap_andnot(unsigned long *dst, const unsigned long *bitmap1, const unsigned long *bitmap2, int bits) {
    int k;
    int nr = BITS_TO_LONGS(bits);
    unsigned long result = 0;

    for (k = 0; k < nr; k++)
        result |= (dst[k] = bitmap1[k] & ~bitmap2[k]);
    return result != 0;
}

bool __bitmap_intersects(const unsigned long *bitmap1, const unsigned long *bitmap2, int bits) {
    int k, lim = bits/BITS_PER_LONG;
    for (k = 0; k < lim; ++k)
        if (bitmap1[k] & bitmap2[k])
            return 1;

    if (bits % BITS_PER_LONG)
        if ((bitmap1[k] & bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
            return 1;
    return 0;
}

bool __bitmap_subset(const unsigned long *bitmap1, const unsigned long *bitmap2, int bits) {
    int k, lim = bits/BITS_PER_LONG;
    for (k = 0; k < lim; ++k)
        if (bitmap1[k] & ~bitmap2[k])
            return 0;

    if (bits % BITS_PER_LONG)
        if ((bitmap1[k] & ~bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
            return 0;
    return 1;
}

int __bitmap_weight(const unsigned long *bitmap, int bits) {
    int k, w = 0, lim = bits/BITS_PER_LONG;

    for (k = 0; k < lim; k++)
        w += hweight_long(bitmap[k]);

    if (bits % BITS_PER_LONG)
        w += hweight_long(bitmap[k] & BITMAP_LAST_WORD_MASK(bits));

    return w;
}

#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) % BITS_PER_LONG))

void bitmap_set(unsigned long *map, int start, int nr) {
    unsigned long *p = map + BIT_WORD(start);
    const int size = start + nr;
    int bits_to_set = BITS_PER_LONG - (start % BITS_PER_LONG);
    unsigned long mask_to_set = BITMAP_FIRST_WORD_MASK(start);

    while (nr - bits_to_set >= 0) {
        *p |= mask_to_set;
        nr -= bits_to_set;
        bits_to_set = BITS_PER_LONG;
        mask_to_set = ~0UL;
        p++;
    }
    if (nr) {
        mask_to_set &= BITMAP_LAST_WORD_MASK(size);
        *p |= mask_to_set;
    }
}

void bitmap_clear(unsigned long *map, int start, int nr) {
    unsigned long *p = map + BIT_WORD(start);
    const int size = start + nr;
    int bits_to_clear = BITS_PER_LONG - (start % BITS_PER_LONG);
    unsigned long mask_to_clear = BITMAP_FIRST_WORD_MASK(start);

    while (nr - bits_to_clear >= 0) {
        *p &= ~mask_to_clear;
        nr -= bits_to_clear;
        bits_to_clear = BITS_PER_LONG;
        mask_to_clear = ~0UL;
        p++;
    }
    if (nr) {
        mask_to_clear &= BITMAP_LAST_WORD_MASK(size);
        *p &= ~mask_to_clear;
    }
}

/*
 * Find a contiguous aligned zero area.
 *
 * The @p align_mask should be one less than a power of 2; the effect is that
 * the bit offset of all zero areas this function finds is multiples of that
 * power of 2. A @p align_mask of 0 means no alignment is required.
 *
 * @param map address to base the search on
 * @param size bitmap size in bits
 * @param start bitnumber to start searching at
 * @param nr number of zeroed bits we're looking for
 * @param align_mask alignment mask for zero area
 */
unsigned long bitmap_find_next_zero_area(unsigned long *map, unsigned long size, unsigned long start, unsigned int nr, unsigned long align_mask) {
    unsigned long index, end, i;
again:
    index = find_next_zero_bit(map, size, start);

    /* Align allocation */
    index = __ALIGN_MASK(index, align_mask);

    end = index + nr;
    if (end > size)
        return end;
    i = find_next_bit(map, end, index);
    if (i < end) {
        start = i + 1;
        goto again;
    }
    return index;
}

/*
 * Bitmap printing & parsing functions
 */
#define CHUNKSZ 32
#define nbits_to_hold_value(val) fls(val)
#define unhex(c) (isdigit(c) ? (c - '0') : (toupper(c) - 'A' + 10))
#define BASEDEC 10 /* fancier CPUset lists input in decimal */

/**
 * Convert bitmap to an ASCII hex string.
 *
 * Exactly @p nmaskbits bits are displayed. Hex digits are grouped into
 * comma-separated sets of eight digits per set.
 *
 * @param buf byte buffer into which string is placed
 * @param buflen reserved size of @p buf, in bytes
 * @param maskp pointer to bitmap to convert
 * @param nmaskbits size of bitmap, in bits
 */
int bitmap_snprintf(char *buf, unsigned int buflen, const unsigned long *maskp, int nmaskbits) {
    int i, word, bit, len = 0;
    unsigned long val;
    const char *sep = "";
    int chunksz;
    uint32_t chunkmask;

    chunksz = nmaskbits & (CHUNKSZ - 1);
    if (chunksz == 0)
        chunksz = CHUNKSZ;

    i = ALIGN(nmaskbits, CHUNKSZ) - CHUNKSZ;
    for (; i >= 0; i -= CHUNKSZ) {
        chunkmask = ((1ULL << chunksz) - 1);
        word = i / BITS_PER_LONG;
        bit = i % BITS_PER_LONG;
        val = (maskp[word] >> bit) & chunkmask;
        len += snprintf(buf + len, buflen - len, "%s%0*lx", sep, (chunksz + 3) / 4, val);
        chunksz = CHUNKSZ;
        sep = ",";
    }
    return len;
}

/**
 * Convert an ASCII hex string into a bitmap.
 *
 * Commas group hex digits into chunks.  Each chunk defines exactly 32 bits of
 * the resultant bitmask.  No chunk may specify a value larger than 32 bits
 * (%-EOVERFLOW), and if a chunk specifies a smaller value then leading 0-bits
 * are prepended.  %-EINVAL is returned for illegal characters and for
 * grouping errors such as "1,,5", ",44", "," and "".  Leading and trailing
 * whitespace accepted, but not embedded whitespace.
 *
 * @param buf pointer to buffer containing string
 * @param buflen buffer size in bytes, if string is smaller than this then it
 *      must be terminated with a \0
 * @param maskp pointer to bitmap array that will contain result
 * @param nmaskbits size of bitmap, in bits
 */
int __bitmap_parse(const char *buf, unsigned int buflen, unsigned long *maskp, int nmaskbits) {
    int c, old_c, totaldigits, ndigits, nchunks, nbits;
    uint32_t chunk;
    const char *ubuf = buf;

    bitmap_zero(maskp, nmaskbits);

    nchunks = nbits = totaldigits = c = 0;
    do {
        chunk = ndigits = 0;

        /* Get the next chunk of the bitmap */
        while (buflen) {
            old_c = c;
            c = *buf++;
            buflen--;
            if (isspace(c))
                continue;

            /*
             * If the last character was a space and the current
             * character isn't '\0', we've got embedded whitespace.
             * This is a no-no, so throw an error.
             */
            if (totaldigits && c && isspace(old_c))
                return -EINVAL;

            /* A '\0' or a ',' signal the end of the chunk */
            if (c == '\0' || c == ',')
                break;

            if (!isxdigit(c))
                return -EINVAL;

            /*
             * Make sure there are at least 4 free bits in 'chunk'.
             * If not, this hexdigit will overflow 'chunk', so
             * throw an error.
             */
            if (chunk & ~((1UL << (CHUNKSZ - 4)) - 1))
                return -EOVERFLOW;

            chunk = (chunk << 4) | unhex(c);
            ndigits++; totaldigits++;
        }
        if (ndigits == 0)
            return -EINVAL;
        if (nchunks == 0 && chunk == 0)
            continue;

        __bitmap_shift_left(maskp, maskp, CHUNKSZ, nmaskbits);
        *maskp |= chunk;
        nchunks++;
        nbits += (nchunks == 1) ? nbits_to_hold_value(chunk) : CHUNKSZ;
        if (nbits > nmaskbits)
            return -EOVERFLOW;
    } while (buflen && c == ',');

    return 0;
}

/*
 * Helper routine for bitmap_scnlistprintf(). Write decimal number or range
 * to buf, suppressing output past buf+buflen, with optional comma-prefix.
 * Return len of what would be written to buf, if it all fit.
 */
static inline int bscnl_emit(char *buf, int buflen, int rbot, int rtop, int len) {
    if (len > 0)
        len += snprintf(buf + len, buflen - len, ",");
    if (rbot == rtop)
        len += snprintf(buf + len, buflen - len, "%d", rbot);
    else
        len += snprintf(buf + len, buflen - len, "%d-%d", rbot, rtop);
    return len;
}

/**
 * Convert bitmap to list format ASCII string
 *
 * Output format is a comma-separated list of decimal numbers and ranges.
 * Consecutively set bits are shown as two hyphen-separated decimal numbers,
 * the smallest and largest bit numbers set in the range. Output format is
 * compatible with the format accepted as input by bitmap_parselist().
 *
 * @param buf byte buffer into which string is placed
 * @param buflen reserved size of @p buf, in bytes
 * @param maskp pointer to bitmap to convert
 * @param nmaskbits size of bitmap, in bits
 * @return the number of characters which would be generated for the given
 *      input, excluding the trailing '\0', as per ISO C99
 */
int bitmap_snlistprintf(char *buf, unsigned int buflen, const unsigned long *maskp, int nmaskbits) {
    int len = 0;
    /* current bit is 'cur', most recently seen range is [rbot, rtop] */
    int cur, rbot, rtop;

    if (buflen == 0)
        return 0;
    buf[0] = 0;

    rbot = cur = find_first_bit(maskp, nmaskbits);
    while (cur < nmaskbits) {
        rtop = cur;
        cur = find_next_bit(maskp, nmaskbits, cur+1);
        if (cur >= nmaskbits || cur > rtop + 1) {
            len = bscnl_emit(buf, buflen, rbot, rtop, len);
            rbot = cur;
        }
    }
    return len;
}

/**
 * Convert list format ASCII string to bitmap.
 *
 * Input format is a comma-separated list of decimal numbers and ranges.
 * Consecutively set bits are shown as two hyphen-separated decimal numbers,
 * the smallest and largest bit numbers set in the range.
 *
 * @param bp read null-terminated user string from this buffer
 * @param maskp write resulting mask here
 * @param nmaskbits number of bits in mask to be written
 * @return 0 on success, -errno on invalid input strings
 * @retval -EINVAL second number in range smaller than first
 * @retval -EINVAL invalid character in string
 * @retval -ERANGE bit number specified too large for mask
 */
int bitmap_parselist(const char *bp, unsigned long *maskp, int nmaskbits) {
    unsigned a, b;

    bitmap_zero(maskp, nmaskbits);
    do {
        if (!isdigit(*bp))
            return -EINVAL;
        b = a = strtoul(bp, (char **)&bp, BASEDEC);
        if (*bp == '-') {
            bp++;
            if (!isdigit(*bp))
                return -EINVAL;
            b = strtoul(bp, (char **)&bp, BASEDEC);
        }
        if (!(a <= b))
            return -EINVAL;
        if (b >= nmaskbits)
            return -ERANGE;
        while (a <= b) {
            set_bit(a, maskp);
            a++;
        }
        if (*bp == ',')
            bp++;
    } while (*bp != '\0' && *bp != '\n');
    return 0;
}

/**
 * Find ordinal of set bit at given position in bitmap.
 *
 * Map the bit at position @pos in @pbuf (of length @p bits) to the ordinal of
 * which set bit it is.  If it is not set or if @pos is not a valid bit
 * position, map to -1.
 *
 * If for example, just bits 4 through 7 are set in @p buf, then @pos values 4
 * through 7 will get mapped to 0 through 3, respectively, and other @pos
 * values will get mapped to 0.  When @pos value 7 gets mapped to (returns) @p
 * ord value 3 in this example, that means that bit 7 is the 3rd (starting
 * with 0th) set bit in @p buf.
 *
 * The bit positions 0 through @p bits are valid positions in @p buf.
 * @param buf pointer to a bitmap
 * @param pos a bit position in @p buf (0 <= @p pos < @p bits)
 * @param bits number of valid bit positions in @p buf
 */
static int bitmap_pos_to_ord(const unsigned long *buf, int pos, int bits) {
    int i, ord;

    if (pos < 0 || pos >= bits || !test_bit(pos, buf))
        return -1;

    i = find_first_bit(buf, bits);
    ord = 0;
    while (i < pos) {
        i = find_next_bit(buf, bits, i + 1);
            ord++;
    }

    return ord;
}

/**
 * Find position of n-th set bit in bitmap.
 *
 * Map the ordinal offset of bit @p ord in @p buf to its position in @p buf.
 * Value of @p ord should be in range 0 <= @pord < weight(buf), else results
 * are undefined.
 *
 * If for example, just bits 4 through 7 are set in @p buf, then @p ord values
 * 0 through 3 will get mapped to 4 through 7, respectively, and all other @p
 * ord values return undefined values.  When @p ord value 3 gets mapped to
 * (returns) @pos value 7 in this example, that means that the 3rd set bit
 * (starting with 0th) is at position 7 in @p buf.
 *
 * The bit positions 0 through @p bits are valid positions in @p buf.
 *
 * @param buf: pointer to bitmap
 * @param ord ordinal bit position (n-th set bit, n >= 0)
 * @param bits number of valid bit positions in @p buf
 */
static int bitmap_ord_to_pos(const unsigned long *buf, int ord, int bits) {
    int pos = 0;

    if (ord >= 0 && ord < bits) {
        int i;

        for (i = find_first_bit(buf, bits);
             i < bits && ord > 0;
             i = find_next_bit(buf, bits, i + 1))
                ord--;
        if (i < bits && ord == 0)
            pos = i;
    }

    return pos;
}

/**
 * Apply map defined by a pair of bitmaps to another bitmap.
 *
 * Let @p old and @p new define a mapping of bit positions, such that whatever
 * position is held by the n-th set bit in @p old is mapped to the n-th set
 * bit in @p new.  In the more general case, allowing for the possibility that
 * the weight 'w' of @p new is less than the weight of @p old, map the
 * position of the n-th set bit in @p old to the position of the m-th set bit
 * in @p new, where m == n % w.
 *
 * If either of the @p old and @p new bitmaps are empty, or if @p src and @p
 * dst point to the same location, then this routine copies @p src to @p dst.
 *
 * The positions of unset bits in @p old are mapped to themselves (the
 * identify map).
 *
 * Apply the above specified mapping to @p src, placing the result in @p dst,
 * clearing any bits previously set in @p dst.
 *
 * For example, lets say that @p old has bits 4 through 7 set, and @p new has
 * bits 12 through 15 set.  This defines the mapping of bit position 4 to 12,
 * 5 to 13, 6 to 14 and 7 to 15, and of all other bit positions unchanged. So
 * if say @p src comes into this routine with bits 1, 5 and 7 set, then @p dst
 * should leave with bits 1, 13 and 15 set.
 *
 * @param dst remapped result
 * @param src subset to be remapped
 * @param old defines domain of map
 * @param new defines range of map
 * @param bits number of bits in each of these bitmaps
 */
void bitmap_remap(unsigned long *dst, const unsigned long *src, const unsigned long *old, const unsigned long *new, int bits) {
    int oldbit, w;

    if (dst == src) /* following doesn't handle inplace remaps */
        return;
    bitmap_zero(dst, bits);

    w = bitmap_weight(new, bits);
    for_each_set_bit(oldbit, src, bits) {
        int n = bitmap_pos_to_ord(old, oldbit, bits);

        if (n < 0 || w == 0)
            set_bit(oldbit, dst); /* identity map */
        else
            set_bit(bitmap_ord_to_pos(new, n % w, bits), dst);
    }
}

/**
 * Apply map defined by a pair of bitmaps to a single bit.
 *
 * Let @p old and @p new define a mapping of bit positions, such that
 * whatever position is held by the n-th set bit in @p old is mapped
 * to the n-th set bit in @p new.  In the more general case, allowing
 * for the possibility that the weight 'w' of @p new is less than the
 * weight of @old, map the position of the n-th set bit in @p old to
 * the position of the m-th set bit in @p new, where m == n % w.
 *
 * The positions of unset bits in @p old are mapped to themselves
 * (the identify map).
 *
 * Apply the above specified mapping to bit position @p oldbit, returning
 * the new bit position.
 *
 * For example, lets say that @p old has bits 4 through 7 set, and
 * @p new has bits 12 through 15 set.  This defines the mapping of bit
 * position 4 to 12, 5 to 13, 6 to 14 and 7 to 15, and of all other
 * bit positions unchanged.  So if say @p oldbit is 5, then this routine
 * returns 13.
 *
 * @param oldbit bit position to be mapped
 * @param old defines domain of map
 * @param new defines range of map
 * @param bits number of bits in each of these bitmaps
 */
int bitmap_bitremap(int oldbit, const unsigned long *old, const unsigned long *new, int bits) {
    int w = bitmap_weight(new, bits);
    int n = bitmap_pos_to_ord(old, oldbit, bits);
    if (n < 0 || w == 0)
        return oldbit;
    else
        return bitmap_ord_to_pos(new, n % w, bits);
}

/**
 * Translate one bitmap relative to another.
 *
 * Set the n-th bit of @p dst iff there exists some m such that the n-th bit of
 * @p relmap is set, the m-th bit of @p orig is set, and the n-th bit of @p
 * relmap is also the m-th _set_ bit of @p relmap.  (If you understood the
 * previous sentence the first time your read it, you're overqualified for
 * your current job.)
 *
 * In other words, @p orig is mapped onto (surjectively) @p dst, using the the
 * map { <n, m> | the n-th bit of @p relmap is the m-th set bit of @p relmap
 * }.
 *
 * Any set bits in @p orig above bit number W, where W is the weight of
 * (number of set bits in) @p relmap are mapped nowhere.  In particular, if
 * for all bits m set in @p orig, m >= W, then @p dst will end up empty.  In
 * situations where the possibility of such an empty result is not desired,
 * one way to avoid it is to use the bitmap_fold() operator, below, to first
 * fold the @p orig bitmap over itself so that all its set bits x are in the
 * range 0 <= x < W.  The bitmap_fold() operator does this by setting the bit
 * (m % W) in @p dst, for each bit (m) set in @p orig.
 *
 * Example [1] for bitmap_onto():
 *  Let's say @p relmap has bits 30-39 set, and @p orig has bits 1, 3, 5, 7, 9
 *  and 11 set.  Then on return from this routine, @p dst will have bits 31, 33,
 *  35, 37 and 39 set.
 *
 *  When bit 0 is set in @p orig, it means turn on the bit in @p dst
 *  corresponding to whatever is the first bit (if any) that is turned on in
 *  @p relmap.  Since bit 0 was off in the above example, we leave off that bit
 *  (bit 30) in @p dst.
 *
 *  When bit 1 is set in @p orig (as in the above example), it means turn on
 *  the bit in @p dst corresponding to whatever is the second bit that is
 *  turned on in @p relmap.  The second bit in @p relmap that was turned on in
 *  the above example was bit 31, so we turned on bit 31 in @p dst.
 *
 *  Similarly, we turned on bits 33, 35, 37 and 39 in @p dst, because they
 *  were the 4th, 6th, 8th and 10th set bits set in @p relmap, and the 4th,
 *  6th, 8th and 10th bits of @p orig (i.e. bits 3, 5, 7 and 9) were also set.
 *
 *  When bit 11 is set in @p orig, it means turn on the bit in @p dst
 *  corresponding to whatever is the twelth bit that is turned on in @p relmap.
 *  In the above example, there were only ten bits turned on in @p relmap
 *  (30..39), so that bit 11 was set in @p orig had no affect on @p dst.
 *
 * Example [2] for bitmap_fold() + bitmap_onto():
 *  Let's say @p relmap has these ten bits set:
 *      40 41 42 43 45 48 53 61 74 95
 *  (for the curious, that's 40 plus the first ten terms of the Fibonacci
 *  sequence.)
 *
 *  Further lets say we use the following code, invoking bitmap_fold() then
 *  bitmap_onto, as suggested above to avoid the possitility of an empty @pdst
 *  result:
 *
 *  unsigned long *tmp; // a temporary bitmap's bits
 *
 *  bitmap_fold(tmp, orig, bitmap_weight(relmap, bits), bits);
 *  bitmap_onto(dst, tmp, relmap, bits);
 *
 *  Then this table shows what various values of @p dst would be, for various
 *  @p orig's.  I list the zero-based positions of each set bit.  The tmp
 *  column shows the intermediate result, as computed by using bitmap_fold()
 *  to fold the @p orig bitmap modulo ten (the weight of @p relmap).
 *
 *      @p orig          tmp           @p dst
 *      0                0             40
 *      1                1             41
 *      9                9             95
 *      10               0             40 (*)
 *      1 3 5 7          1 3 5 7       41 43 48 61
 *      0 1 2 3 4        0 1 2 3 4     40 41 42 43 45
 *      0 9 18 27        0 9 8 7       40 61 74 95
 *      0 10 20 30       0             40
 *      0 11 22 33       0 1 2 3       40 41 42 43
 *      0 12 24 36       0 2 4 6       40 42 45 53
 *      78 102 211       1 2 8         41 42 74 (*)
 *
 * (*) For these marked lines, if we hadn't first done bitmap_fold() into tmp,
 *     then the @p dst result would have been empty.
 *
 * If either of @p orig or @p relmap is empty (no set bits), then @p dst will
 * be returned empty.
 *
 * If (as explained above) the only set bits in @p orig are in positions m
 * where m >= W, (where W is the weight of @p relmap) then @p dst will once
 * again be returned empty.
 *
 * All bits in @p dst not set by the above rule are cleared.
 *
 * @param dst resulting translated bitmap
 * @param orig original untranslated bitmap
 * @param relmap bitmap relative to which translated
 * @param bits number of bits in each of these bitmaps
 */
void bitmap_onto(unsigned long *dst, const unsigned long *orig, const unsigned long *relmap, int bits) {
    int n, m; /* same meaning as in above comment */

    if (dst == orig) /* following doesn't handle inplace mappings */
        return;
    bitmap_zero(dst, bits);

    /*
     * The following code is a more efficient, but less
     * obvious, equivalent to the loop:
     *  for (m = 0; m < bitmap_weight(relmap, bits); m++) {
     *      n = bitmap_ord_to_pos(orig, m, bits);
     *      if (test_bit(m, orig))
     *          set_bit(n, dst);
     *  }
     */

    m = 0;
    for_each_set_bit(n, relmap, bits) {
        /* m == bitmap_pos_to_ord(relmap, n, bits) */
        if (test_bit(m, orig))
            set_bit(n, dst);
        m++;
    }
}

/**
 * Fold larger bitmap into smaller, modulo specified size.
 *
 * For each bit oldbit in @p orig, set bit oldbit mod @p sz in @p dst. Clear
 * all other bits in @p dst.
 *
 * @param dst resulting smaller bitmap
 * @param orig original larger bitmap
 * @param sz specified size
 * @param bits number of bits in each of these bitmaps
 */
void bitmap_fold(unsigned long *dst, const unsigned long *orig, int sz, int bits) {
    int oldbit;

    if (dst == orig)
        return;
    bitmap_zero(dst, bits);

    for_each_set_bit(oldbit, orig, bits)
        set_bit(oldbit % sz, dst);
}

enum {
    REG_OP_ISFREE, /* true if region is all zero bits */
    REG_OP_ALLOC, /* set all bits in region */
    REG_OP_RELEASE, /* clear all bits in region */
};

/*
 * Can set, verify and/or release a region of bits in a bitmap.
 *
 * A region of a bitmap is a sequence of bits in the bitmap, of some size '1
 * << order' (a power of two), aligned to that same '1 << order' power of two.
 *
 * @param bitmap an array of unsigned longs corresponding to the bitmap
 * @param pos the beginning of the region
 * @param order the region size (log base 2 of number of bits)
 * @param reg_op the operation(s) to perform on that region of bitmap
 * @return 1 if REG_OP_ISFREE succeeds (region is all zero bits), 0 otherwise
 */
static int __reg_op(unsigned long *bitmap, int pos, int order, int reg_op) {
    int nbits_reg;  /* number of bits in region */
    int index;  /* index first long of region in bitmap */
    int offset; /* bit offset region in bitmap[index] */
    int nlongs_reg; /* num longs spanned by region in bitmap */
    int nbitsinlong; /* num bits of region in each spanned long */
    unsigned long mask; /* bitmask for one long of region */
    int i; /* scans bitmap by longs */
    int ret = 0; /* return value */

    /*
     * Either nlongs_reg == 1 (for small orders that fit in one long)
     * or (offset == 0 && mask == ~0UL) (for larger multiword orders.)
     */
    nbits_reg = 1 << order;
    index = pos / BITS_PER_LONG;
    offset = pos - (index * BITS_PER_LONG);
    nlongs_reg = BITS_TO_LONGS(nbits_reg);
    nbitsinlong = min(nbits_reg,  BITS_PER_LONG);

    /*
     * Can't do "mask = (1UL << nbitsinlong) - 1", as that
     * overflows if nbitsinlong == BITS_PER_LONG.
     */
    mask = (1UL << (nbitsinlong - 1));
    mask += mask - 1;
    mask <<= offset;

    switch (reg_op) {
    case REG_OP_ISFREE:
        for (i = 0; i < nlongs_reg; i++) {
            if (bitmap[index + i] & mask)
                goto done;
        }
        ret = 1; /* all bits in region free (zero) */
        break;

    case REG_OP_ALLOC:
        for (i = 0; i < nlongs_reg; i++)
            bitmap[index + i] |= mask;
        break;

    case REG_OP_RELEASE:
        for (i = 0; i < nlongs_reg; i++)
            bitmap[index + i] &= ~mask;
        break;
    }
done:
    return ret;
}

/**
 * Find a contiguous aligned mem region.
 *
 * Find a region of free (zero) bits in a @p bitmap of @p bits bits and
 * allocate them (set them to one). Only consider regions of length a power
 * (@p order) of two, aligned to that power of two, which makes the search
 * algorithm much faster.
 *
 * @param bitmap an array of unsigned longs corresponding to the bitmap
 * @param bits the number of bits in the bitmap
 * @param order the region size (log base 2 of number of bits) to find
 * @return the bit offset in bitmap of the allocated region,
 *      or -errno on failure
 */
int bitmap_find_free_region(unsigned long *bitmap, int bits, int order) {
    int pos, end; /* scans bitmap by regions of size order */

    for (pos = 0 ; (end = pos + (1 << order)) <= bits; pos = end) {
        if (!__reg_op(bitmap, pos, order, REG_OP_ISFREE))
            continue;
        __reg_op(bitmap, pos, order, REG_OP_ALLOC);
        return pos;
    }
    return -ENOMEM;
}

/**
 * Release allocated bitmap region (by clearing it in the bitmap).
 *
 * @param bitmap an array of unsigned longs corresponding to the bitmap
 * @param pos the beginning of bit region to release
 * @param order the region size (log base 2 of number of bits) to release
 */
void bitmap_release_region(unsigned long *bitmap, int pos, int order) {
    __reg_op(bitmap, pos, order, REG_OP_RELEASE);
}

/**
 * Allocate (set bits in) a specified region of a bitmap.
 *
 * @param bitmap an array of unsigned longs corresponding to the bitmap
 * @param pos the beginning of bit region to allocate
 * @param order the region size (log base 2 of number of bits) to allocate
 * @return 0 on success, or %-EBUSY if specified region wasn't free
 *      (not all bits were zero)
 */
int bitmap_allocate_region(unsigned long *bitmap, int pos, int order) {
    if (!__reg_op(bitmap, pos, order, REG_OP_ISFREE))
        return -EBUSY;
    __reg_op(bitmap, pos, order, REG_OP_ALLOC);
    return 0;
}
