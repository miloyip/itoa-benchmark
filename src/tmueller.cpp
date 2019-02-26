// #include "itoa_ljust_impl.h"
#include "test.h"

#if defined(__GNUC__) || defined(__clang__)
#define t_likely(expr)   __builtin_expect(expr, 1)
#define t_unlikely(expr) __builtin_expect(expr, 0)
#else
#define t_likely(expr)   (expr)
#define t_unlikely(expr) (expr)
#endif

#define t_inline __attribute__((always_inline))

static const char DIGITS[] =
    "0001020304050607080910111213141516171819"
    "2021222324252627282930313233343536373839"
    "4041424344454647484950515253545556575859"
    "6061626364656667686970717273747576777879"
    "8081828384858687888990919293949596979899";

t_inline void uint32toa_tmueller(uint64_t x, char* out) {
    char* str = out;
    if (t_likely(x < 100000)) {
        if (t_likely(x) < 1000) {
            if (t_likely(x) < 10) {
                *str++ = (char) ('0' + x);
                *str = 0;
                return;
            }
            uint32_t inc = 0;
            x = (x * ((0xffffUL / 100UL) + 1));
            uint32_t d;
            d = (x >> 16); *str = (char) ('0' | d); inc |= -d; str += inc >> 31;
            x = (x & 0xffffUL) * 10;
            d = (x >> 16); *str = (char) ('0' | d); inc |= -d; str += inc >> 31;
            x = (x & 0xffffUL) * 10;
            *str++ = (char) ('0' + (x >> 16));
            *str = 0;
        } else {
            uint32_t inc = 0;
            x = (x * ((0xffffffffUL / 10000UL) + 1));
            uint32_t d;
            d = (x >> 32); *str = (char) ('0' | d); inc |= -d; str += inc >> 31;
            x = (x & 0xffffffffUL) * 100;
            memcpy(str, DIGITS + (x >> 32) * 2, 2); str += 2;
            x = (x & 0xffffffffUL) * 100;
            memcpy(str, DIGITS + (x >> 32) * 2, 2); str += 2;
            *str = 0;
        }
    } else {
        if (t_likely(x < 10000000)) {
            uint32_t inc = 0;
            x = (x * ((0xfffffffffffUL / 1000000) + 1));
            uint32_t d;
            d = (x >> 44); *str = (char) ('0' | d); inc |= -d; str += inc >> 31;
            x = (x & 0xfffffffffffUL) * 100;
            memcpy(str, DIGITS + (x >> 44) * 2, 2); str += 2;
            x = (x & 0xfffffffffffUL) * 100;
            memcpy(str, DIGITS + (x >> 44) * 2, 2); str += 2;
            x = (x & 0xfffffffffffUL) * 100;
            memcpy(str, DIGITS + (x >> 44) * 2, 2); str += 2;
            *str = 0;
        } else {
            uint32_t inc = 0;
            x = (((x * 2305843009L) >> 29) + 4);
            uint32_t d;
            d = (x >> 32); *str = (char) ('0' | d); inc |= -d; str += inc >> 31;
            x = (x & 0xffffffffUL) * 10;
            d = (x >> 32); *str = (char) ('0' | d); inc |= -d; str += inc >> 31;
            x = (x & 0xffffffffUL) * 100;
            memcpy(str, DIGITS + (x >> 32) * 2, 2); str += 2;
            x = (x & 0xffffffffUL) * 100;
            memcpy(str, DIGITS + (x >> 32) * 2, 2); str += 2;
            x = (x & 0xffffffffUL) * 100;
            memcpy(str, DIGITS + (x >> 32) * 2, 2); str += 2;
            x = (x & 0xffffffffUL) * 100;
            memcpy(str, DIGITS + (x >> 32) * 2, 2); str += 2;
            *str = 0;
        }
    }
}

void u32toa_tmueller(uint32_t v, char* out) {
    uint32toa_tmueller(v, out);
}

void i32toa_tmueller( int32_t v, char* out) {
    // branchless (from amartin)
    *out = '-';
    uint32_t mask = v < 0 ? ~(int32_t) 0 : 0;
    uint32_t u = ((2 * (uint32_t)(v)) & ~mask) - v;
    out += mask & 1;
    uint64_t x = u;
    uint32toa_tmueller(x, out);
}

static const uint64_t POW_10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000,
    100000000, 1000000000, 10000000000ULL, 100000000000ULL, 1000000000000ULL, 10000000000000ULL,
    100000000000000ULL, 1000000000000000ULL, 10000000000000000ULL, 100000000000000000ULL,
    1000000000000000000ULL, 10000000000000000000ULL };

void u64toa_tmueller(uint64_t v, char* out) {
    if(v < 10) {
        *out++ = '0' + v;
        *out = 0;
        return;
    }
    int zeros = 64 - __builtin_clzl(v);
    int len = (1233 * zeros) >> 12;
    uint64_t p10 = POW_10[len];
    if (v >= p10) {
        len++;
    }
    out += len;
    *out = 0;
    while (v >= 100) {
        uint64_t d100 = v / 100;
        uint64_t index = v - d100 * 100;
        v = d100;
        out -= 2;
        memcpy(out, DIGITS + index * 2, 2);
    }
    if (v < 10) {
        *--out = '0' + v;
        return;
    }
    out -= 2;
    memcpy(out, DIGITS + v * 2, 2);
}

void i64toa_tmueller( int64_t v, char* out) {
    // branchless (from amartin)
    *out = '-';
    uint64_t mask = v < 0 ? ~(int64_t) 0 : 0;
    uint64_t u = ((2 * (uint64_t)(v)) & ~mask) - v;
    out += mask & 1;
    u64toa_tmueller(u, out);
}

REGISTER_TEST(tmueller);
