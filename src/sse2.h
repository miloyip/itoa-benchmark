#pragma once

//#if defined(_MSC_VER) && _M_IX86_FP >= 2

#include <emmintrin.h>
#include "digitslut.h"

// Branching for different cases (forward)
// Use lookup table of two digits

const uint32_t kDiv10000 = 0xd1b71759;
__declspec(align(16)) const uint32_t kDiv10000Vector[4] = { kDiv10000, kDiv10000, kDiv10000, kDiv10000 };
__declspec(align(16)) const uint32_t k10000Vector[4] = { 10000,  10000, 10000, 10000 };
__declspec(align(16)) const uint16_t kDivPowersVector[8] = { 8389, 5243, 13108, 32768, 8389, 5243, 13108, 32768 }; // 10^3, 10^2, 10^1, 10^0
__declspec(align(16)) const uint16_t kShiftPowersVector[8] = {
    1 << (16 - (23 + 2 - 16)),
    1 << (16 - (19 + 2 - 16)),
    1 << (16 - 1 - 2),
    1 << (15),
    1 << (16 - (23 + 2 - 16)),
    1 << (16 - (19 + 2 - 16)),
    1 << (16 - 1 - 2),
    1 << (15)
};
__declspec(align(16)) const uint16_t k10Vector[8] = { 10, 10, 10, 10, 10, 10, 10, 10 };
__declspec(align(16)) const char kAsciiZero[16] = { '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' };

inline __m128i Convert8DigitsSSE2(uint32_t value) {
	const __m128i v = _mm_cvtsi32_si128(value);
	const __m128i div10000 = _mm_srli_epi64(_mm_mul_epu32(v, reinterpret_cast<const __m128i*>(kDiv10000Vector)[0]), 45);
	const __m128i v0 = _mm_mul_epu32(div10000, reinterpret_cast<const __m128i*>(k10000Vector)[0]);
	const __m128i mod10000 = _mm_sub_epi32(v, v0);
	const __m128i v1 = _mm_unpacklo_epi16(div10000, mod10000);
	const __m128i v1a = _mm_slli_epi64(v1, 2);
	const __m128i v2a = _mm_unpacklo_epi16(v1a, v1a);
	const __m128i v2 = _mm_unpacklo_epi32(v2a, v2a);
	const __m128i v3 = _mm_mulhi_epu16(v2, reinterpret_cast<const __m128i*>(kDivPowersVector)[0]);
	const __m128i v4 = _mm_mulhi_epu16(v3, reinterpret_cast<const __m128i*>(kShiftPowersVector)[0]);
	const __m128i v5 = _mm_mullo_epi16(v4, reinterpret_cast<const __m128i*>(k10Vector)[0]);
	const __m128i v6 = _mm_slli_epi64(v5, 16);
	const __m128i v7 = _mm_sub_epi16(v4, v6);
	const __m128i v8 = _mm_packus_epi16(_mm_setzero_si128(), v7);
	const __m128i v9 = _mm_add_epi8(v8, reinterpret_cast<const __m128i*>(kAsciiZero)[0]);
	return v9;
}

inline void u32toa_sse2(uint32_t value, char* buffer) {
    if (value < 10000) {
        const uint32_t d1 = (value / 100) << 1;
        const uint32_t d2 = (value % 100) << 1;
        
        if (value >= 1000)
            *buffer++ = gDigitsLut[d1];
        if (value >= 100)
            *buffer++ = gDigitsLut[d1 + 1];
        if (value >= 10)
            *buffer++ = gDigitsLut[d2];
        *buffer++ = gDigitsLut[d2 + 1];
		*buffer++ = '\0';
	}
    else if (value < 100000000) {
		const __m128i a = Convert8DigitsSSE2(value);
		const unsigned mask = _mm_movemask_epi8(_mm_cmpeq_epi8(a, reinterpret_cast<const __m128i*>(kAsciiZero)[0]));
        unsigned long digit;
		_BitScanForward(&digit, ~mask | 0x8000);
		__m128i result;
		switch (digit) {
			case 8: result = _mm_srli_si128(a, 8); break;
			case 9: result = _mm_srli_si128(a, 9); break;
			case 10: result = _mm_srli_si128(a, 10); break;
			case 11: result = _mm_srli_si128(a, 11); break;
			case 12: result = _mm_srli_si128(a, 12); break;
			case 13: result = _mm_srli_si128(a, 13); break;
			case 14: result = _mm_srli_si128(a, 14); break;
			case 15: result = _mm_srli_si128(a, 15); break;
			case 16: result = _mm_srli_si128(a, 16); break;
		}
		_mm_storel_epi64(reinterpret_cast<__m128i*>(buffer), result);
		buffer[16 - digit] = '\0';
    }
    else {
        // value = aabbbbbbbb in decimal
        
        const uint32_t a = value / 100000000; // 1 to 42
        value %= 100000000;
        
        if (a >= 10) {
            const unsigned i = a << 1;
            *buffer++ = gDigitsLut[i];
            *buffer++ = gDigitsLut[i + 1];
        }
        else
            *buffer++ = '0' + static_cast<char>(a);

		const __m128i b = Convert8DigitsSSE2(value);
		const __m128i result = _mm_srli_si128(b, 8);
		_mm_storel_epi64(reinterpret_cast<__m128i*>(buffer), result);
		buffer[8] = '\0';
	}
}

inline void i32toa_sse2(int32_t value, char* buffer) {
	if (value < 0) {
		*buffer++ = '-';
		value = -value;
	}

	u32toa_sse2(static_cast<uint32_t>(value), buffer);
}

inline void u64toa_sse2(uint64_t value, char* buffer) {
    if (value < 100000000) {
        uint32_t v = static_cast<uint32_t>(value);
        if (v < 10000) {
            const uint32_t d1 = (v / 100) << 1;
            const uint32_t d2 = (v % 100) << 1;
            
            if (v >= 1000)
                *buffer++ = gDigitsLut[d1];
            if (v >= 100)
                *buffer++ = gDigitsLut[d1 + 1];
            if (v >= 10)
                *buffer++ = gDigitsLut[d2];
            *buffer++ = gDigitsLut[d2 + 1];
        }
        else {
            // value = bbbbcccc
            const uint32_t b = v / 10000;
            const uint32_t c = v % 10000;
            
            const uint32_t d1 = (b / 100) << 1;
            const uint32_t d2 = (b % 100) << 1;
            
            const uint32_t d3 = (c / 100) << 1;
            const uint32_t d4 = (c % 100) << 1;
            
            if (value >= 10000000)
                *buffer++ = gDigitsLut[d1];
            if (value >= 1000000)
                *buffer++ = gDigitsLut[d1 + 1];
            if (value >= 100000)
                *buffer++ = gDigitsLut[d2];
            *buffer++ = gDigitsLut[d2 + 1];
            
            *buffer++ = gDigitsLut[d3];
            *buffer++ = gDigitsLut[d3 + 1];
            *buffer++ = gDigitsLut[d4];
            *buffer++ = gDigitsLut[d4 + 1];
        }
    }
    else if (value < 10000000000000000) {
        const uint32_t v0 = static_cast<uint32_t>(value / 100000000);
        const uint32_t v1 = static_cast<uint32_t>(value % 100000000);
        
        const uint32_t b0 = v0 / 10000;
        const uint32_t c0 = v0 % 10000;
        
        const uint32_t d1 = (b0 / 100) << 1;
        const uint32_t d2 = (b0 % 100) << 1;
        
        const uint32_t d3 = (c0 / 100) << 1;
        const uint32_t d4 = (c0 % 100) << 1;

        const uint32_t b1 = v1 / 10000;
        const uint32_t c1 = v1 % 10000;
        
        const uint32_t d5 = (b1 / 100) << 1;
        const uint32_t d6 = (b1 % 100) << 1;
        
        const uint32_t d7 = (c1 / 100) << 1;
        const uint32_t d8 = (c1 % 100) << 1;

        if (value >= 1000000000000000)
            *buffer++ = gDigitsLut[d1];
        if (value >= 100000000000000)
            *buffer++ = gDigitsLut[d1 + 1];
        if (value >= 10000000000000)
            *buffer++ = gDigitsLut[d2];
        if (value >= 1000000000000)
            *buffer++ = gDigitsLut[d2 + 1];
        if (value >= 100000000000)
            *buffer++ = gDigitsLut[d3];
        if (value >= 10000000000)
            *buffer++ = gDigitsLut[d3 + 1];
        if (value >= 1000000000)
            *buffer++ = gDigitsLut[d4];
        if (value >= 100000000)
            *buffer++ = gDigitsLut[d4 + 1];
        
        *buffer++ = gDigitsLut[d5];
        *buffer++ = gDigitsLut[d5 + 1];
        *buffer++ = gDigitsLut[d6];
        *buffer++ = gDigitsLut[d6 + 1];
        *buffer++ = gDigitsLut[d7];
        *buffer++ = gDigitsLut[d7 + 1];
        *buffer++ = gDigitsLut[d8];
        *buffer++ = gDigitsLut[d8 + 1];
    }
    else {
        const uint32_t a = static_cast<uint32_t>(value / 10000000000000000); // 1 to 1844
        value %= 10000000000000000;
        
        if (a < 10)
            *buffer++ = '0' + static_cast<char>(a);
        else if (a < 100) {
            const uint32_t i = a << 1;
            *buffer++ = gDigitsLut[i];
            *buffer++ = gDigitsLut[i + 1];
        }
        else if (a < 1000) {
            *buffer++ = '0' + static_cast<char>(a / 100);
            
            const uint32_t i = (a % 100) << 1;
            *buffer++ = gDigitsLut[i];
            *buffer++ = gDigitsLut[i + 1];
        }
        else {
            const uint32_t i = (a / 100) << 1;
            const uint32_t j = (a % 100) << 1;
            *buffer++ = gDigitsLut[i];
            *buffer++ = gDigitsLut[i + 1];
            *buffer++ = gDigitsLut[j];
            *buffer++ = gDigitsLut[j + 1];
        }
        
        const uint32_t v0 = static_cast<uint32_t>(value / 100000000);
        const uint32_t v1 = static_cast<uint32_t>(value % 100000000);
        
        const uint32_t b0 = v0 / 10000;
        const uint32_t c0 = v0 % 10000;
        
        const uint32_t d1 = (b0 / 100) << 1;
        const uint32_t d2 = (b0 % 100) << 1;
        
        const uint32_t d3 = (c0 / 100) << 1;
        const uint32_t d4 = (c0 % 100) << 1;
        
        const uint32_t b1 = v1 / 10000;
        const uint32_t c1 = v1 % 10000;
        
        const uint32_t d5 = (b1 / 100) << 1;
        const uint32_t d6 = (b1 % 100) << 1;
        
        const uint32_t d7 = (c1 / 100) << 1;
        const uint32_t d8 = (c1 % 100) << 1;
        
        *buffer++ = gDigitsLut[d1];
        *buffer++ = gDigitsLut[d1 + 1];
        *buffer++ = gDigitsLut[d2];
        *buffer++ = gDigitsLut[d2 + 1];
        *buffer++ = gDigitsLut[d3];
        *buffer++ = gDigitsLut[d3 + 1];
        *buffer++ = gDigitsLut[d4];
        *buffer++ = gDigitsLut[d4 + 1];
        *buffer++ = gDigitsLut[d5];
        *buffer++ = gDigitsLut[d5 + 1];
        *buffer++ = gDigitsLut[d6];
        *buffer++ = gDigitsLut[d6 + 1];
        *buffer++ = gDigitsLut[d7];
        *buffer++ = gDigitsLut[d7 + 1];
        *buffer++ = gDigitsLut[d8];
        *buffer++ = gDigitsLut[d8 + 1];
    }
    
	*buffer = '\0';
}

inline void i64toa_sse2(int64_t value, char* buffer) {
	if (value < 0) {
		*buffer++ = '-';
		value = -value;
	}

	u64toa_sse2(static_cast<uint64_t>(value), buffer);
}

//#endif 