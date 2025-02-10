/*
 * Integer to ascii conversion (ANSI C)
 *
 * Description
 *     The itoa function converts an integer value to a character string in
 *     decimal and stores the result in the buffer. If value is negative, the
 *     resulting string is preceded with a minus sign (-).
 *
 * Parameters
 *     val: Value to be converted.
 *     buf: Buffer that holds the result of the conversion.
 *
 * Return Value
 *     A pointer to the end of resulting string.
 *
 * Notice
 *     The resulting string is not null-terminated.
 *     The buffer should be large enough to hold any possible result:
 *         uint32_t: 10 bytes
 *         uint64_t: 20 bytes
 *         int32_t: 11 bytes
 *         int64_t: 20 bytes
 *
 * Copyright (c) 2018 YaoYuan <ibireme@gmail.com>.
 * Released under the MIT license (MIT).
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "test.h"

#if defined(__has_attribute)
#define yy_attribute(x) __has_attribute(x)
#else
#define yy_attribute(x) 0
#endif

#if !defined(force_inline)
#if yy_attribute(always_inline) || (defined(__GNUC__) && __GNUC__ >= 4)
#define force_inline __inline__ __attribute__((always_inline))
#elif defined(__clang__) || defined(__GNUC__)
#define force_inline __inline__
#elif defined(_MSC_VER) && _MSC_VER >= 1200
#define force_inline __forceinline
#elif defined(_MSC_VER)
#define force_inline __inline
#elif defined(__cplusplus) || \
    (defined(__STDC__) && __STDC__ && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
#define force_inline inline
#else
#define force_inline
#endif
#endif

#if !defined(table_align)
#if yy_attribute(aligned) || defined(__GNUC__)
#define table_align(x) __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#define table_align(x) __declspec(align(x))
#else
#define table_align(x)
#endif
#endif

table_align(2) static const char digit_table[200] = {
    '0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9', '1', '0', '1',
    '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9', '2', '0', '2', '1', '2', '2',
    '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9', '3', '0', '3', '1', '3', '2', '3', '3', '3',
    '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9', '4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5',
    '4', '6', '4', '7', '4', '8', '4', '9', '5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5',
    '7', '5', '8', '5', '9', '6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8',
    '6', '9', '7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9', '8',
    '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9', '9', '0', '9', '1',
    '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'};

static force_inline void byte_copy_2(void *dst, const void *src) { memcpy(dst, src, 2); }

static force_inline char *itoa_u32_impl(uint32_t val, char *buf) {
    /* The maximum value of uint32_t is 4294967295 (10 digits), */
    /* these digits are named as 'aabbccddee' here.             */
    uint32_t aa, bb, cc, dd, ee, aabb, bbcc, ccdd, ddee, aabbcc;

    /* Leading zero count in the first pair.                    */
    uint32_t lz;

    /* Although most compilers may convert the "division by     */
    /* constant value" into "multiply and shift", manual        */
    /* conversion can still help some compilers generate        */
    /* fewer and better instructions.                           */

    if (val < 100) { /* 1-2 digits: aa */
        lz = val < 10;
        byte_copy_2(buf + 0, digit_table + val * 2 + lz);
        buf -= lz;
        return buf + 2;

    } else if (val < 10000) {    /* 3-4 digits: aabb */
        aa = (val * 5243) >> 19; /* (val / 100) */
        bb = val - aa * 100;     /* (val % 100) */
        lz = aa < 10;
        byte_copy_2(buf + 0, digit_table + aa * 2 + lz);
        buf -= lz;
        byte_copy_2(buf + 2, digit_table + bb * 2);
        return buf + 4;

    } else if (val < 1000000) {                          /* 5-6 digits: aabbcc */
        aa = (uint32_t)(((uint64_t)val * 429497) >> 32); /* (val / 10000) */
        bbcc = val - aa * 10000;                         /* (val % 10000) */
        bb = (bbcc * 5243) >> 19;                        /* (bbcc / 100) */
        cc = bbcc - bb * 100;                            /* (bbcc % 100) */
        lz = aa < 10;
        byte_copy_2(buf + 0, digit_table + aa * 2 + lz);
        buf -= lz;
        byte_copy_2(buf + 2, digit_table + bb * 2);
        byte_copy_2(buf + 4, digit_table + cc * 2);
        return buf + 6;

    } else if (val < 100000000) { /* 7~8 digits: aabbccdd */
        /* (val / 10000) */
        aabb = (uint32_t)(((uint64_t)val * 109951163) >> 40);
        ccdd = val - aabb * 10000; /* (val % 10000) */
        aa = (aabb * 5243) >> 19;  /* (aabb / 100) */
        cc = (ccdd * 5243) >> 19;  /* (ccdd / 100) */
        bb = aabb - aa * 100;      /* (aabb % 100) */
        dd = ccdd - cc * 100;      /* (ccdd % 100) */
        lz = aa < 10;
        byte_copy_2(buf + 0, digit_table + aa * 2 + lz);
        buf -= lz;
        byte_copy_2(buf + 2, digit_table + bb * 2);
        byte_copy_2(buf + 4, digit_table + cc * 2);
        byte_copy_2(buf + 6, digit_table + dd * 2);
        return buf + 8;

    } else { /* 9~10 digits: aabbccddee */
        /* (val / 10000) */
        aabbcc = (uint32_t)(((uint64_t)val * 3518437209ul) >> 45);
        /* (aabbcc / 10000) */
        aa = (uint32_t)(((uint64_t)aabbcc * 429497) >> 32);
        ddee = val - aabbcc * 10000; /* (val % 10000) */
        bbcc = aabbcc - aa * 10000;  /* (aabbcc % 10000) */
        bb = (bbcc * 5243) >> 19;    /* (bbcc / 100) */
        dd = (ddee * 5243) >> 19;    /* (ddee / 100) */
        cc = bbcc - bb * 100;        /* (bbcc % 100) */
        ee = ddee - dd * 100;        /* (ddee % 100) */
        lz = aa < 10;
        byte_copy_2(buf + 0, digit_table + aa * 2 + lz);
        buf -= lz;
        byte_copy_2(buf + 2, digit_table + bb * 2);
        byte_copy_2(buf + 4, digit_table + cc * 2);
        byte_copy_2(buf + 6, digit_table + dd * 2);
        byte_copy_2(buf + 8, digit_table + ee * 2);
        return buf + 10;
    }
}

static force_inline char *itoa_u64_impl_len_8(uint32_t val, char *buf) {
    uint32_t aa, bb, cc, dd, aabb, ccdd;
    aabb = (uint32_t)(((uint64_t)val * 109951163) >> 40); /* (val / 10000) */
    ccdd = val - aabb * 10000;                            /* (val % 10000) */
    aa = (aabb * 5243) >> 19;                             /* (aabb / 100) */
    cc = (ccdd * 5243) >> 19;                             /* (ccdd / 100) */
    bb = aabb - aa * 100;                                 /* (aabb % 100) */
    dd = ccdd - cc * 100;                                 /* (ccdd % 100) */
    byte_copy_2(buf + 0, digit_table + aa * 2);
    byte_copy_2(buf + 2, digit_table + bb * 2);
    byte_copy_2(buf + 4, digit_table + cc * 2);
    byte_copy_2(buf + 6, digit_table + dd * 2);
    return buf + 8;
}

static force_inline char *itoa_u64_impl_len_4(uint32_t val, char *buf) {
    uint32_t aa, bb;
    aa = (val * 5243) >> 19; /* (val / 100) */
    bb = val - aa * 100;     /* (val % 100) */
    byte_copy_2(buf + 0, digit_table + aa * 2);
    byte_copy_2(buf + 2, digit_table + bb * 2);
    return buf + 4;
}

static force_inline char *itoa_u64_impl_len_1_to_8(uint32_t val, char *buf) {
    uint32_t aa, bb, cc, dd, aabb, bbcc, ccdd, lz;

    if (val < 100) { /* 1-2 digits: aa */
        lz = val < 10;
        byte_copy_2(buf + 0, digit_table + val * 2 + lz);
        buf -= lz;
        return buf + 2;

    } else if (val < 10000) {    /* 3-4 digits: aabb */
        aa = (val * 5243) >> 19; /* (val / 100) */
        bb = val - aa * 100;     /* (val % 100) */
        lz = aa < 10;
        byte_copy_2(buf + 0, digit_table + aa * 2 + lz);
        buf -= lz;
        byte_copy_2(buf + 2, digit_table + bb * 2);
        return buf + 4;

    } else if (val < 1000000) {                          /* 5-6 digits: aabbcc */
        aa = (uint32_t)(((uint64_t)val * 429497) >> 32); /* (val / 10000) */
        bbcc = val - aa * 10000;                         /* (val % 10000) */
        bb = (bbcc * 5243) >> 19;                        /* (bbcc / 100) */
        cc = bbcc - bb * 100;                            /* (bbcc % 100) */
        lz = aa < 10;
        byte_copy_2(buf + 0, digit_table + aa * 2 + lz);
        buf -= lz;
        byte_copy_2(buf + 2, digit_table + bb * 2);
        byte_copy_2(buf + 4, digit_table + cc * 2);
        return buf + 6;

    } else { /* 7-8 digits: aabbccdd */
        /* (val / 10000) */
        aabb = (uint32_t)(((uint64_t)val * 109951163) >> 40);
        ccdd = val - aabb * 10000; /* (val % 10000) */
        aa = (aabb * 5243) >> 19;  /* (aabb / 100) */
        cc = (ccdd * 5243) >> 19;  /* (ccdd / 100) */
        bb = aabb - aa * 100;      /* (aabb % 100) */
        dd = ccdd - cc * 100;      /* (ccdd % 100) */
        lz = aa < 10;
        byte_copy_2(buf + 0, digit_table + aa * 2 + lz);
        buf -= lz;
        byte_copy_2(buf + 2, digit_table + bb * 2);
        byte_copy_2(buf + 4, digit_table + cc * 2);
        byte_copy_2(buf + 6, digit_table + dd * 2);
        return buf + 8;
    }
}

static force_inline char *itoa_u64_impl_len_5_to_8(uint32_t val, char *buf) {
    uint32_t aa, bb, cc, dd, aabb, bbcc, ccdd, lz;

    if (val < 1000000) {                                 /* 5-6 digits: aabbcc */
        aa = (uint32_t)(((uint64_t)val * 429497) >> 32); /* (val / 10000) */
        bbcc = val - aa * 10000;                         /* (val % 10000) */
        bb = (bbcc * 5243) >> 19;                        /* (bbcc / 100) */
        cc = bbcc - bb * 100;                            /* (bbcc % 100) */
        lz = aa < 10;
        byte_copy_2(buf + 0, digit_table + aa * 2 + lz);
        buf -= lz;
        byte_copy_2(buf + 2, digit_table + bb * 2);
        byte_copy_2(buf + 4, digit_table + cc * 2);
        return buf + 6;

    } else { /* 7-8 digits: aabbccdd */
        /* (val / 10000) */
        aabb = (uint32_t)(((uint64_t)val * 109951163) >> 40);
        ccdd = val - aabb * 10000; /* (val % 10000) */
        aa = (aabb * 5243) >> 19;  /* (aabb / 100) */
        cc = (ccdd * 5243) >> 19;  /* (ccdd / 100) */
        bb = aabb - aa * 100;      /* (aabb % 100) */
        dd = ccdd - cc * 100;      /* (ccdd % 100) */
        lz = aa < 10;
        byte_copy_2(buf + 0, digit_table + aa * 2 + lz);
        buf -= lz;
        byte_copy_2(buf + 2, digit_table + bb * 2);
        byte_copy_2(buf + 4, digit_table + cc * 2);
        byte_copy_2(buf + 6, digit_table + dd * 2);
        return buf + 8;
    }
}

static force_inline char *itoa_u64_impl(uint64_t val, char *buf) {
    uint64_t tmp, hgh;
    uint32_t mid, low;

    if (val < 100000000) { /* 1-8 digits */
        buf = itoa_u64_impl_len_1_to_8((uint32_t)val, buf);
        return buf;

    } else if (val < (uint64_t)100000000 * 100000000) { /* 9-16 digits */
        hgh = val / 100000000;
        low = (uint32_t)(val - hgh * 100000000); /* (val % 100000000) */
        buf = itoa_u64_impl_len_1_to_8((uint32_t)hgh, buf);
        buf = itoa_u64_impl_len_8(low, buf);
        return buf;

    } else { /* 17-20 digits */
        tmp = val / 100000000;
        low = (uint32_t)(val - tmp * 100000000); /* (val % 100000000) */
        hgh = (uint32_t)(tmp / 10000);
        mid = (uint32_t)(tmp - hgh * 10000); /* (tmp % 10000) */
        buf = itoa_u64_impl_len_5_to_8((uint32_t)hgh, buf);
        buf = itoa_u64_impl_len_4(mid, buf);
        buf = itoa_u64_impl_len_8(low, buf);
        return buf;
    }
}

char *itoa_u32_yy(uint32_t val, char *buf) { return itoa_u32_impl(val, buf); }

char *itoa_i32_yy(int32_t val, char *buf) {
    uint32_t pos = (uint32_t)val;
    uint32_t neg = ~pos + 1;
    size_t sign = val < 0;
    *buf = '-';
    return itoa_u32_impl(sign ? neg : pos, buf + sign);
}

char *itoa_u64_yy(uint64_t val, char *buf) { return itoa_u64_impl(val, buf); }

char *itoa_i64_yy(int64_t val, char *buf) {
    uint64_t pos = (uint64_t)val;
    uint64_t neg = ~pos + 1;
    size_t sign = val < 0;
    *buf = '-';
    return itoa_u64_impl(sign ? neg : pos, buf + sign);
}

void u32toa_yy(uint32_t v, char* out) { (void)itoa_u32_yy(v, out); }
void u64toa_yy(uint64_t v, char* out) { (void)itoa_u64_yy(v,out); }
void i32toa_yy(int32_t v, char* out) { (void)itoa_i32_yy(v, out); }
void i64toa_yy(int64_t v, char* out) { (void)itoa_i64_yy(v, out); }

REGISTER_TEST(yy);