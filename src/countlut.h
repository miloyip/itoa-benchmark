#pragma once

#include "countdecimaldigit.h"

// Additional count number of digit pass
// Use lookup table of two gDigitsCountLut

const char gDigitsCountLut[201] =
	"0001020304050607080910111213141516171819"
	"2021222324252627282930313233343536373839"
	"4041424344454647484950515253545556575859"
	"6061626364656667686970717273747576777879"
	"8081828384858687888990919293949596979899";

inline void u32toa_countlut(uint32_t value, char* buffer) {
	unsigned digit = CountDecimalDigit32(value);
	buffer += digit;
	*buffer = '\0';

	while (value >= 100) {
		const unsigned i = (value % 100) << 1;
		value /= 100;
		*--buffer = gDigitsCountLut[i + 1];
		*--buffer = gDigitsCountLut[i];
	}

	if (value < 10) {
		*--buffer = char(value) + '0';
	}
	else {
		const unsigned i = value << 1;
		*--buffer = gDigitsCountLut[i + 1];
		*--buffer = gDigitsCountLut[i];
	}
}

inline void i32toa_countlut(int32_t value, char* buffer) {
	if (value < 0) {
		*buffer++ = '-';
		value = -value;
	}
	
	u32toa_countlut(static_cast<uint32_t>(value), buffer);
}

inline void u64toa_countlut(uint64_t value, char* buffer) {
	unsigned digit = CountDecimalDigit64(value);
	buffer += digit;
	*buffer = '\0';

	while (value >= 100) {
		const unsigned i = static_cast<unsigned>(value % 100) << 1;
		value /= 100;
		*--buffer = gDigitsCountLut[i + 1];
		*--buffer = gDigitsCountLut[i];
	}

	if (value < 10) {
		*--buffer = char(value) + '0';
	}
	else {
		const unsigned i = static_cast<unsigned>(value) << 1;
		*--buffer = gDigitsCountLut[i + 1];
		*--buffer = gDigitsCountLut[i];
	}
}

inline void i64toa_countlut(int64_t value, char* buffer) {
	if (value < 0) {
		*buffer++ = '-';
		value = -value;
	}
	
	u64toa_countlut(static_cast<uint64_t>(value), buffer);
}
