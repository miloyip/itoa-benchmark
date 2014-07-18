#pragma once

#include "countdecimaldigit.h"

// Additional count number of digit pass 

inline void u32toa_count(uint32_t value, char* buffer) {
	unsigned digit = CountDecimalDigit32(value);
	buffer += digit;
	*buffer = '\0';

	do {
		*--buffer = char(value % 10) + '0';
		value /= 10;
	} while (value > 0);
}

inline void i32toa_count(int32_t value, char* buffer) {
	if (value < 0) {
		*buffer++ = '-';
		value = -value;
	}
	u32toa_count(static_cast<uint32_t>(value), buffer);
}

inline void u64toa_count(uint64_t value, char* buffer) {
	unsigned digit = CountDecimalDigit64(value);
	buffer += digit;
	*buffer = '\0';

	do {
		*--buffer = char(value % 10) + '0';
		value /= 10;
	} while (value > 0);
}

inline void i64toa_count(int64_t value, char* buffer) {
	if (value < 0) {
		*buffer++ = '-';
		value = -value;
	}
	u64toa_count(static_cast<uint64_t>(value), buffer);
}
