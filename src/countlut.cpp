#include "countdecimaldigit.h"
#include "digitslut.h"
#include "test.h"

// Additional count number of digit pass
// Use lookup table of two gDigitsLut

void u32toa_countlut(uint32_t value, char* buffer) {
	unsigned digit = CountDecimalDigit32(value);
	buffer += digit;
	*buffer = '\0';

	while (value >= 100) {
		const unsigned i = (value % 100) << 1;
		value /= 100;
		*--buffer = gDigitsLut[i + 1];
		*--buffer = gDigitsLut[i];
	}

	if (value < 10) {
		*--buffer = char(value) + '0';
	}
	else {
		const unsigned i = value << 1;
		*--buffer = gDigitsLut[i + 1];
		*--buffer = gDigitsLut[i];
	}
}

void i32toa_countlut(int32_t value, char* buffer) {
	if (value < 0) {
		*buffer++ = '-';
		value = -value;
	}
	
	u32toa_countlut(static_cast<uint32_t>(value), buffer);
}

void u64toa_countlut(uint64_t value, char* buffer) {
	unsigned digit = CountDecimalDigit64(value);
	buffer += digit;
	*buffer = '\0';

	while (value >= 100) {
		const unsigned i = static_cast<unsigned>(value % 100) << 1;
		value /= 100;
		*--buffer = gDigitsLut[i + 1];
		*--buffer = gDigitsLut[i];
	}

	if (value < 10) {
		*--buffer = char(value) + '0';
	}
	else {
		const unsigned i = static_cast<unsigned>(value) << 1;
		*--buffer = gDigitsLut[i + 1];
		*--buffer = gDigitsLut[i];
	}
}

void i64toa_countlut(int64_t value, char* buffer) {
	if (value < 0) {
		*buffer++ = '-';
		value = -value;
	}
	
	u64toa_countlut(static_cast<uint64_t>(value), buffer);
}

REGISTER_TEST(countlut);
