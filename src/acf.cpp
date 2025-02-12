#include <stdint.h>
#include "test.h"

#include <string.h>

#define LIKELY(x)   __builtin_expect(static_cast<bool>(x), 1)
#define UNLIKELY(x) __builtin_expect(static_cast<bool>(x), 0)

/*
 * We only use functions for clarity here.  Eyeballing -S output
 * reveals GCC makes strange inlining decisions on this file,
 * especially with LIKELY.  Make sure our helpers are really
 * inlined.
 */
#define INLINE inline __attribute__((always_inline)) static

typedef __uint128_t uint128_t;

/* Powers of 10. */
#define PO2                100ULL
#define PO4              10000ULL
#define PO8          100000000ULL
#define PO10       10000000000ULL
#define PO16 10000000000000000ULL

/* 64 bit's worth of '0' characters (i.e., 8 characters). */
#define ZERO_CHARS 0x3030303030303030ULL

/**
 * encode_* functions unpack (pairs of) numbers into BCD: each byte
 * contains exactly one decimal digit.
 *
 * The basic idea is to use SWAR (SIMD within a register) and perform
 * low-precision arithmetic on several values in parallel.
 *
 * Most non-obviousness lies in the conversion of integer division
 * constants to multiplication, shift and mask by hand.  Decent
 * compilers do it for scalars, but we can't easily express a SWAR
 * integer division.
 *
 * The trick is to choose a low enough precision that the fixed point
 * multiplication won't overflow into the next packed value (and high
 * enough that the truncated division is exact for the relevant
 * range), and to pack values so that the final result ends up in the
 * byte we want.  There are formulae to determine how little precision
 * we need given an input range and a constant divisor, but for our
 * cases, one can also check exhaustively (:
 *
 * The remainder is simple: given d = x / k, x % k = x - k * d.
 */

/**
 * SWAR unpack [100 * hi + lo] to 4 decimal bytes, assuming hi and lo
 * \in [0, 100)
 */
INLINE uint32_t
encode_hundreds(uint32_t hi, uint32_t lo)
{
    /*
     * Pack everything in a single 32 bit value.
     *
     * merged = [ hi 0 lo 0 ]
     */
    uint32_t merged = hi | (lo << 16);
    /*
     * Fixed-point multiplication by 103/1024 ~= 1/10.
     */
    uint32_t tens = (merged * 103UL) >> 10;

    /*
     * Mask away garbage bits between our digits.
     *
     * tens = [ hi/10 0 lo/10 0 ]
     *
     * On a platform with more restricted literals (ARM, for
     * instance), it may make sense to and-not with the middle
     * bits.
     */
    tens &= (0xFUL << 16) | 0xFUL;

    /*
     * x mod 10 = x - 10 * (x div 10).
     *
     * (merged - 10 * tens) = [ hi%10 0 lo%10 0 ]
     *
     * Then insert these values between tens.  Arithmetic instead
     * of bitwise operation helps the compiler merge this with
     * later increments by a constant (e.g., ZERO_CHARS).
     */
    return tens + ((merged - 10UL * tens) << 8);
}

/**
 * SWAR encode 10000 hi + lo to byte (unpacked) BCD.
 */
INLINE uint64_t
encode_ten_thousands(uint64_t hi, uint64_t lo)
{
    uint64_t merged = hi | (lo << 32);
    /* Truncate division by 100: 10486 / 2**20 ~= 1/100. */
    uint64_t top = ((merged * 10486ULL) >> 20) & ((0x7FULL << 32) | 0x7FULL);
    /* Trailing 2 digits in the 1e4 chunks. */
    uint64_t bot = merged - 100ULL * top;
    uint64_t hundreds;
    uint64_t tens;

    /*
     * We now have 4 radix-100 digits in little-endian order, each
     * in its own 16 bit area.
     */
    hundreds = (bot << 16) + top;

    /* Divide and mod by 10 all 4 radix-100 digits in parallel. */
    tens = (hundreds * 103ULL) >> 10;
    tens &= (0xFULL << 48) | (0xFULL << 32) | (0xFULL << 16) | 0xFULL;
    tens += (hundreds - 10ULL * tens) << 8;

    return tens;
}

/**
 * Range-specialised version of itoa.
 *
 * We always convert to fixed-width BCD then shift away any leading
 * zero.  The slop will manifest as writing zero bytes after our
 * encoded string, which is acceptable: we never write more than the
 * maximal length (10 or 20 characters).
 */

/**
 * itoa for x < 100.
 */
INLINE char*
itoa_hundred(char* out, unsigned int x)
{
    /*
     * -1 if x < 10, 0 otherwise.  Tried to get an sbb, but ?:
     * gets us branches.
     */
    int small = (int)(x - 10) >> 8;
    unsigned int base = (unsigned int)'0' | ((unsigned int)'0' << 8);
    /*
     * Probably not necessary, but why not abuse smaller constants?
     * Also, see block comment above idiv_POx functions.
     */
    unsigned int hi = (x * 103UL) >> 10;
    unsigned int lo = x - 10UL * hi;

    base += hi + (lo << 8);
    /* Shift away the leading zero (shift by 8) if x < 10. */
    base >>= small & 8;
    memcpy(out, &base, 2);
    /* 2 + small = 1 if x < 10, 2 otherwise. */
    return out + 2 + small;
}

/**
 * itoa for x < 10k.
 */
INLINE char*
itoa_ten_thousand(char* out, unsigned int x)
{
    uint32_t buf;
    unsigned int x_div_PO2, x_mod_PO2;
    unsigned int zeros;

    x_div_PO2 = (x * 10486UL) >> 20;
    x_mod_PO2 = x - PO2 * x_div_PO2;
    buf = encode_hundreds(x_div_PO2, x_mod_PO2);
    /*
     * Count leading (in memory, trailing in register: we're
     * little endian) zero bytes: count leading zero bits and
     * round down to 8.
     */
    zeros = __builtin_ctz(buf) & -8U;
    buf += ZERO_CHARS; /* BCD -> ASCII. */
    buf >>= zeros; /* Shift away leading zero characters */
    memcpy(out, &buf, 4);

    /* zeros is in bits; convert to bytes to find actual length. */
    return out + 4 - zeros / 8;
}

/**
 * 32 bit helpers for truncation by constant.
 *
 * We only need them because GCC is stupid with likely/unlikely
 * annotations: unlikely code is compiled with an extreme emphasis on
 * size, up to compiling integer division by constants to actual div
 * instructions.  In turn, we want likely annotations because we only
 * get a nice ladder of forward conditional jumps when there is no
 * code between if blocks.  We convince GCC that our "special" cases
 * for shorter integers aren't slowpathed guards by marking each
 * conditional as likely.
 *
 * The constants are easily proven correct (or compared with those
 * generated by a reference compiler, e.g., GCC or clang).  For
 * example,
 *
 *   1/10000 ~= k = 3518437209 / 2**45 = 1/10000 + 73/21990232555520000.
 *
 * Let eps = 73/21990232555520000; for any 0 <= x < 2**32,
 * floor(k * x) <= floor(x / 10000 + x * eps)
 *              <= floor(x / 10000 + 2**32 * eps)
 *              <= floor(x / 10000 + 2e-5).
 *
 * Given that x is unsigned, flooring the left and right -hand sides
 * will yield the same value as long as the error term
 * (x * eps <= 2e-5) is less than 1/10000, and 2e-5 < 10000.  We finally
 * conclude that 3518437209 / 2**45, our fixed point approximation of
 * 1/10000, is always correct for truncated division of 32 bit
 * unsigned ints.
 */

/**
 * Divide a 32 bit int by 1e4.
 */
INLINE uint32_t
idiv_PO4(uint32_t x)
{
    uint64_t wide = x;
    uint64_t mul = 3518437209UL;

    return (wide * mul) >> 45;
}

/**
 * Divide a 32 bit int by 1e8.
 */
INLINE uint64_t
idiv_PO8(uint32_t x)
{
    uint64_t wide = x;
    uint64_t mul = 1441151881UL;

    return (wide * mul) >> 57;
}

INLINE
char*
an_itoa(char* out, uint32_t x)
{
    uint64_t buf;
    uint32_t x_div_PO4;
    uint32_t x_mod_PO4;
    uint32_t x_div_PO8;

    /*
     * Smaller numbers can be encoded more quickly.  Special
     * casing them makes a significant difference compared to
     * always going through 8-digit encoding.
     */
    if (LIKELY(x < PO2)) {
        return itoa_hundred(out, x);
    }

    if (LIKELY(x < PO4)) {
        return itoa_ten_thousand(out, x);
    }

    /*
     * Manual souped up common subexpression elimination.
     *
     * The sequel always needs x / PO4 and x % PO4.  Compute them
     * here, before branching.  We may also need x / PO8 if
     * x >= PO8.  Benchmarking shows that performing this division
     * by constant unconditionally doesn't hurt.  If x >= PO8, we'll
     * always want x_div_PO4 = (x % PO8) / PO4.  We compute that
     * in a roundabout manner to reduce the makespan, i.e., the
     * length of the dependency chain for (x % PO8) % PO4 = x % PO4.
     */
    x_div_PO4 = idiv_PO4(x);
    x_mod_PO4 = x - x_div_PO4 * PO4;
    x_div_PO8 = idiv_PO8(x);
    /*
     * We actually want x_div_PO4 = (x % PO8) / PO4.
     * Subtract what would have been removed by (x % PO8) from
     * x_div_PO4.
     */
    x_div_PO4 -= x_div_PO8 * PO4;
    /*
     * Finally, we can unconditionally encode_ten_thousands the
     * values we obtain after division by PO8 and fixup by
     * x_div_PO8 * PO4.
     */
    buf = encode_ten_thousands(x_div_PO4, x_mod_PO4);

    if (LIKELY(x < PO8)) {
        unsigned zeros;

        zeros = __builtin_ctzll(buf) & -8U;
        buf += ZERO_CHARS;
        buf >>= zeros;

        memcpy(out, &buf, 8);
        return out + 8 - zeros / 8;
    }

    /* 32 bit integers are always below 1e10. */
    buf += ZERO_CHARS;
    out = itoa_hundred(out, x_div_PO8);
    memcpy(out, &buf, 8);
    return out + 8;
}

/**
 * 64 bit helpers for truncation by constant.
 */

/**
 * Divide a 64 bit int by 1e4.
 */
INLINE uint64_t
ldiv_PO4(uint64_t x)
{
    uint128_t wide = x;
    uint128_t mul = 3777893186295716171ULL;

    return (wide * mul) >> 75;
}

/**
 * Divide a 64 bit int by 1e8.
 */
INLINE uint64_t
ldiv_PO8(uint64_t x)
{
    uint128_t wide = x;
    uint128_t mul = 12379400392853802749ULL;

    return (wide * mul) >> 90;
}

/**
 * Divide a 64 bit int by 1e16.
 */
INLINE uint64_t
ldiv_PO16(uint64_t x)
{
    uint128_t wide = x;
    uint128_t mul = 4153837486827862103ULL;

    return (wide * mul) >> 115;
}

INLINE
char*
an_ltoa(char* out, uint64_t x)
{
    uint64_t x_div_PO4;
    uint64_t x_mod_PO4;
    uint64_t x_div_PO8;
    uint64_t buf;

    if (LIKELY(x < PO2)) {
        return itoa_hundred(out, x);
    }

    if (LIKELY(x < PO4)) {
        return itoa_ten_thousand(out, x);
    }

    x_div_PO4 = ldiv_PO4(x);
    x_mod_PO4 = x - x_div_PO4 * PO4;

    /*
     * Benchmarking shows the long division by PO8 hurts
     * performance for PO4 <= x < PO8.  Keep encode_ten_thousands
     * conditional for an_ltoa.
     */
    if (LIKELY(x < PO8)) {
        uint64_t buf;
        unsigned zeros;

        buf = encode_ten_thousands(x_div_PO4, x_mod_PO4);
        zeros = __builtin_ctzll(buf) & -8U;
        buf += ZERO_CHARS;
        buf >>= zeros;

        memcpy(out, &buf, 8);
        return out + 8 - zeros / 8;
    }

    /* See block comment in an_itoa. */
    x_div_PO8 = ldiv_PO8(x);
    x_div_PO4 = x_div_PO4 - x_div_PO8 * PO4;
    buf = encode_ten_thousands(x_div_PO4, x_mod_PO4) + ZERO_CHARS;

    /*
     * Add a case for PO8 <= x < PO10 because itoa_hundred is much
     * quicker than a second call to encode_ten_thousands; the
     * same isn't true of itoa_ten_thousand.
     */
    if (LIKELY(x < PO10)) {
        out = itoa_hundred(out, x_div_PO8);
        memcpy(out, &buf, 8);
        return out + 8;
    }

    /*
     * Again, long division by PO16 hurts, so do the rest
     * conditionally.
     */
    if (LIKELY(x < PO16)) {
        uint64_t buf_hi;
        uint32_t hi_hi, hi_lo;
        unsigned zeros;

        /* x_div_PO8 < PO8 < 2**32, so idiv_PO4 is safe. */
        hi_hi = idiv_PO4(x_div_PO8);
        hi_lo = x_div_PO8 - hi_hi * PO4;
        buf_hi = encode_ten_thousands(hi_hi, hi_lo);
        zeros = __builtin_ctzll(buf_hi) & -8U;
        buf_hi += ZERO_CHARS;
        buf_hi >>= zeros;

        memcpy(out, &buf_hi, 8);
        out += 8 - zeros / 8;
        memcpy(out, &buf, 8);
        return out + 8;
    } else {
        uint64_t hi = ldiv_PO16(x);
        uint64_t mid = x_div_PO8 - hi * PO8;
        uint64_t buf_mid;
        uint32_t mid_hi, mid_lo;

        mid_hi = idiv_PO4(mid);
        mid_lo = mid - mid_hi * PO4;
        buf_mid = encode_ten_thousands(mid_hi, mid_lo) + ZERO_CHARS;

        out = itoa_ten_thousand(out, hi);
        memcpy(out, &buf_mid, 8);
        memcpy(out + 8, &buf, 8);
        return out + 16;
    }
}



void u32toa_acf(uint32_t value, char* buffer) {
    *an_itoa(buffer, value) = '\0';
}

void i32toa_acf(int32_t value, char* buffer) {
    uint32_t u = static_cast<uint32_t>(value);
    if (value < 0) {
        *buffer++ = '-';
        u = ~u + 1;
    }
    u32toa_acf(u, buffer);
}

void u64toa_acf(uint64_t value, char* buffer) {
    *an_ltoa(buffer, value) = '\0';
}

void i64toa_acf(int64_t value, char* buffer) {
    uint64_t u = static_cast<uint64_t>(value);
    if (value < 0) {
        *buffer++ = '-';
        u = ~u + 1;
    }
    u64toa_acf(u, buffer);
}

REGISTER_TEST(acf);

