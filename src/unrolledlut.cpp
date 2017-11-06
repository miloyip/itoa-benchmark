// unrolledlut.cpp: Fast integer to string conversion by using per-digit-count unrolling and a lookuptable
//
// ===-------- DESCRIPTION --------===
//
// Very fast implementation of uint32_t to string:
// - Automatically takes advantage of two-byte load/store on
//   architectures that support it (memcpy will be optimized).
// - Avoids as many jumps as possible, by unrolling the whole thing for every digit count.
// - Con: Costs some memory for the duplicated instructions of all branches
//
// Further optimization possible:
// - You may reorder the digit-cases, so that the most
//   commonly used cases come first. Currently digit-counts
//   from 7 to 10 are processed first, as they cover ~99.7% of all uint32_t values.
//   By reordering these for your specific needs, you can save one or two extra instructions for these cases.
//
// ===-------- LICENSE --------===
//
// The MIT License (MIT)
//
// Copyright (c) 2017 nyronium (nyronium@genthree.io)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdint.h>
#include "test.h"

static const char TWO_DIGITS_TO_STR[201] =
	"0001020304050607080910111213141516171819"
	"2021222324252627282930313233343536373839"
	"4041424344454647484950515253545556575859"
	"6061626364656667686970717273747576777879"
	"8081828384858687888990919293949596979899";


#define COPY_2_DIGITS(out, value) \
	memcpy(out, &reinterpret_cast<const uint16_t*>(TWO_DIGITS_TO_STR)[value], 2); \
	out += 2;

#define COPY_1_DIGIT(out, value) \
	*out++ = '0' + value;


#define UNROLL_EXACT_DIGITS_8(out, value) {                   \
	uint32_t digits;                                          \
	digits = value  / 1000000;    COPY_2_DIGITS(out, digits); \
	value -= digits * 1000000;                                \
	digits = value  / 10000;      COPY_2_DIGITS(out, digits); \
	value -= digits * 10000;                                  \
	digits = value  / 100;        COPY_2_DIGITS(out, digits); \
	value -= digits * 100;                                    \
								  COPY_2_DIGITS(out, value);  \
	*out = '\0';                                              \
}

#define UNROLL_REMAINING_DIGITS_8(out, value, digits) {       \
	value -= digits * 100000000;                              \
	digits = value  / 1000000;    COPY_2_DIGITS(out, digits); \
	value -= digits * 1000000;                                \
	digits = value  / 10000;      COPY_2_DIGITS(out, digits); \
	value -= digits * 10000;                                  \
	digits = value  / 100;        COPY_2_DIGITS(out, digits); \
	value -= digits * 100;                                    \
								  COPY_2_DIGITS(out, value);  \
	*out = '\0'; return out;                                  \
}

#define UNROLL_REMAINING_DIGITS_6(out, value, digits) {       \
	value -= digits * 1000000;                                \
	digits = value  / 10000;      COPY_2_DIGITS(out, digits); \
	value -= digits * 10000;                                  \
	digits = value  / 100;        COPY_2_DIGITS(out, digits); \
	value -= digits * 100;                                    \
								  COPY_2_DIGITS(out, value);  \
	*out = '\0'; return out;                                  \
}

#define UNROLL_REMAINING_DIGITS_4(out, value, digits) {       \
	value -= digits * 10000;                                  \
	digits = value  / 100;        COPY_2_DIGITS(out, digits); \
	value -= digits * 100;                                    \
								  COPY_2_DIGITS(out, value);  \
	*out = '\0'; return out;                                  \
}

#define UNROLL_REMAINING_DIGITS_2(out, value, digits) {       \
	value -= digits * 100;                                    \
								  COPY_2_DIGITS(out, value);  \
	*out = '\0'; return out;                                  \
}

#define UNROLL_REMAINING_DIGITS_0(out, value) {               \
	*out = '\0'; return out;                                  \
}


#define UNROLL_DIGIT_PAIR_9_10(out, value) {                      \
	uint32_t digits;                                              \
	if (value >= 1000000000) {                                    \
		digits = value  / 100000000;  COPY_2_DIGITS(out, digits); \
		UNROLL_REMAINING_DIGITS_8(out, value, digits);            \
	} else {                                                      \
		digits = value  / 100000000;  COPY_1_DIGIT(out, digits);  \
		UNROLL_REMAINING_DIGITS_8(out, value, digits);            \
	}                                                             \
}

#define UNROLL_DIGIT_PAIR_7_8(out, value) {                       \
	uint32_t digits;                                              \
	if (value >= 10000000) {                                      \
		digits = value  / 1000000;    COPY_2_DIGITS(out, digits); \
		UNROLL_REMAINING_DIGITS_6(out, value, digits);            \
	} else {                                                      \
		digits = value  / 1000000;    COPY_1_DIGIT(out, digits);  \
		UNROLL_REMAINING_DIGITS_6(out, value, digits);            \
	}                                                             \
}

#define UNROLL_DIGIT_PAIR_5_6(out, value) {                       \
	uint32_t digits;                                              \
	if (value >= 100000) {                                        \
		digits = value  / 10000;      COPY_2_DIGITS(out, digits); \
		UNROLL_REMAINING_DIGITS_4(out, value, digits);            \
	} else {                                                      \
		digits = value  / 10000;      COPY_1_DIGIT(out, digits);  \
		UNROLL_REMAINING_DIGITS_4(out, value, digits);            \
	}                                                             \
}

#define UNROLL_DIGIT_PAIR_3_4(out, value) {                       \
	uint32_t digits;                                              \
	if (value >= 1000) {                                          \
		digits = value  / 100;        COPY_2_DIGITS(out, digits); \
		UNROLL_REMAINING_DIGITS_2(out, value, digits);            \
	} else {                                                      \
		digits = value  / 100;        COPY_1_DIGIT(out, digits);  \
		UNROLL_REMAINING_DIGITS_2(out, value, digits);            \
	}                                                             \
}

#define UNROLL_DIGIT_PAIR_1_2(out, value) {                       \
	if (value >= 10) {                                            \
									  COPY_2_DIGITS(out, value);  \
		UNROLL_REMAINING_DIGITS_0(out, value);                    \
	} else {                                                      \
									  COPY_1_DIGIT(out, value);   \
		UNROLL_REMAINING_DIGITS_0(out, value);                    \
	}                                                             \
}

inline char* unrolledlut(uint32_t value, char* out) {
	if (value >= 100000000) {
		UNROLL_DIGIT_PAIR_9_10(out, value);
	} else if (value >= 1000000) {
		UNROLL_DIGIT_PAIR_7_8(out, value);
	} else if (value <  100) {
		UNROLL_DIGIT_PAIR_1_2(out, value);
	} else if (value <  10000) {
		UNROLL_DIGIT_PAIR_3_4(out, value);
	} else { /* (value <  1000000) */
		UNROLL_DIGIT_PAIR_5_6(out, value);
	}
}

char* unrolledlut64(uint64_t value, char* buffer) {
    uint32_t least_significant = static_cast<uint32_t>(value);
	if (least_significant == value) {
		return unrolledlut(least_significant, buffer);
	}

    uint64_t high12 = value / 100000000;

	/* optimized unrolled recursion */
	least_significant = static_cast<uint32_t>(high12);
	if (least_significant == high12) {
		buffer = unrolledlut(least_significant, buffer);
	} else {
		uint64_t high4 = high12 / 100000000;
		buffer = unrolledlut(high4, buffer);

		uint32_t digits_15_8 = high12 - (high4 * 100000000);
		UNROLL_EXACT_DIGITS_8(buffer, digits_15_8);
	}

    uint32_t digits_7_0 = value - (high12 * 100000000);
	UNROLL_EXACT_DIGITS_8(buffer, digits_7_0);
	return buffer;
}

#undef UNROLL_DIGIT_PAIR_1_2
#undef UNROLL_DIGIT_PAIR_3_4
#undef UNROLL_DIGIT_PAIR_5_6
#undef UNROLL_DIGIT_PAIR_7_8
#undef UNROLL_DIGIT_PAIR_9_10

#undef UNROLL_REMAINING_DIGITS_0
#undef UNROLL_REMAINING_DIGITS_2
#undef UNROLL_REMAINING_DIGITS_4
#undef UNROLL_REMAINING_DIGITS_6
#undef UNROLL_REMAINING_DIGITS_8
#undef UNROLL_EXACT_DIGITS_8

#undef COPY_1_DIGIT
#undef COPY_2_DIGITS


void u32toa_unrolledlut(uint32_t value, char* buffer) {
	unrolledlut(value, buffer);
}

void i32toa_unrolledlut(int32_t value, char* buffer) {
    uint32_t uvalue = static_cast<uint32_t>(value);
    if (value < 0) {
        *buffer++ = '-';
        uvalue = -uvalue;
    }

	unrolledlut(uvalue, buffer);
}

void u64toa_unrolledlut(uint64_t value, char* buffer) {
	unrolledlut64(value, buffer);
}

void i64toa_unrolledlut(int64_t value, char* buffer) {
    uint64_t uvalue = static_cast<uint64_t>(value);
    if (value < 0) {
        *buffer++ = '-';
        uvalue = -uvalue;
    }

	unrolledlut64(uvalue, buffer);
}


REGISTER_TEST(unrolledlut);
