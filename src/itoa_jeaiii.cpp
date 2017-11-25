/*
MIT License

Copyright (c) 2017 James Edward Anhalt III (jeaiii)
https://github.com/jeaiii/itoa

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdint.h>

#define A(N) t = (1ULL << (32 + N / 5 * N * 53 / 16)) / uint32_t(1e##N) + 1 - N / 9, t *= u, t >>= N / 5 * N * 53 / 16, t += N / 5 * 4

#if 0
// 1 char at a time

#define D(N) b[N] = char(t >> 32) + '0'
#define E t = 10ULL * uint32_t(t)

#define L0 b[0] = char(u) + '0'
#define L1 A(1), D(0), E, D(1)
#define L2 A(2), D(0), E, D(1), E, D(2)
#define L3 A(3), D(0), E, D(1), E, D(2), E, D(3)
#define L4 A(4), D(0), E, D(1), E, D(2), E, D(3), E, D(4)
#define L5 A(5), D(0), E, D(1), E, D(2), E, D(3), E, D(4), E, D(5)
#define L6 A(6), D(0), E, D(1), E, D(2), E, D(3), E, D(4), E, D(5), E, D(6)
#define L7 A(7), D(0), E, D(1), E, D(2), E, D(3), E, D(4), E, D(5), E, D(6), E, D(7)
#define L8 A(8), D(0), E, D(1), E, D(2), E, D(3), E, D(4), E, D(5), E, D(6), E, D(7), E, D(8)
#define L9 A(9), D(0), E, D(1), E, D(2), E, D(3), E, D(4), E, D(5), E, D(6), E, D(7), E, D(8), E, D(9)

#else
// 2 chars at a time, little endian only, unaligned 2 byte writes

static const uint16_t s_100s[] = {
    '00', '10', '20', '30', '40', '50', '60', '70', '80', '90',
    '01', '11', '21', '31', '41', '51', '61', '71', '81', '91',
    '02', '12', '22', '32', '42', '52', '62', '72', '82', '92',
    '03', '13', '23', '33', '43', '53', '63', '73', '83', '93',
    '04', '14', '24', '34', '44', '54', '64', '74', '84', '94',
    '05', '15', '25', '35', '45', '55', '65', '75', '85', '95',
    '06', '16', '26', '36', '46', '56', '66', '76', '86', '96',
    '07', '17', '27', '37', '47', '57', '67', '77', '87', '97',
    '08', '18', '28', '38', '48', '58', '68', '78', '88', '98',
    '09', '19', '29', '39', '49', '59', '69', '79', '89', '99',
};

#define W(N, I) *(uint16_t*)&b[N] = s_100s[I]
#define Q(N) b[N] = char((10ULL * uint32_t(t)) >> 32) + '0'
#define D(N) W(N, t >> 32)
#define E t = 100ULL * uint32_t(t)

#define L0 b[0] = char(u) + '0'
#define L1 W(0, u)
#define L2 A(1), D(0), Q(2)
#define L3 A(2), D(0), E, D(2)
#define L4 A(3), D(0), E, D(2), Q(4)
#define L5 A(4), D(0), E, D(2), E, D(4)
#define L6 A(5), D(0), E, D(2), E, D(4), Q(6)
#define L7 A(6), D(0), E, D(2), E, D(4), E, D(6) 
#define L8 A(7), D(0), E, D(2), E, D(4), E, D(6), Q(8)
#define L9 A(8), D(0), E, D(2), E, D(4), E, D(6), E, D(8)

#endif

#define LN(N) (L##N, b += N + 1)
#define LZ(N) (L##N, b[N + 1] = '\0')
#define LG(F) (u<100 ? u<10 ? F(0) : F(1) : u<1000000 ? u<10000 ? u<1000 ? F(2) : F(3) : u<100000 ? F(4) : F(5) : u<100000000 ? u<10000000 ? F(6) : F(7) : u<1000000000 ? F(8) : F(9))

void u32toa_jeaiii(uint32_t u, char* b)
{
    uint64_t t;
    LG(LZ);
}

void i32toa_jeaiii(int32_t i, char* b)
{
    uint32_t u = i < 0 ? *b++ = '-', 0 - uint32_t(i) : i;
    uint64_t t;
    LG(LZ);
}

void u64toa_jeaiii(uint64_t n, char* b)
{
    uint32_t u;
    uint64_t t;

    if (uint32_t(n >> 32) == 0)
        return u = uint32_t(n), (void)LG(LZ);

    uint64_t a = n / 100000000;

    if (uint32_t(a >> 32) == 0)
    {
        u = uint32_t(a);
        LG(LN);
    }
    else
    {
        u = uint32_t(a / 100000000);
        LG(LN);
        u = a % 100000000;
        LN(7);
    }

    u = n % 100000000;
    LZ(7);
}

void i64toa_jeaiii(int64_t i, char* b)
{
    uint64_t n = i < 0 ? *b++ = '-', 0 - uint64_t(i) : i;
    u64toa_jeaiii(n, b);
}
