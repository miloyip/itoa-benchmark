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

    while (value >= 100000000) {
        const uint32_t a = static_cast<uint32_t>(value % 100000000);
        value /= 100000000;

        const uint32_t b = a / 10000;
        const uint32_t c = a % 10000;

        const uint32_t b1 = (b / 100) << 1;
        const uint32_t b2 = (b % 100) << 1;
        const uint32_t c1 = (c / 100) << 1;
        const uint32_t c2 = (c % 100) << 1;

        buffer -= 8;

        buffer[0] = gDigitsLut[b1];
        buffer[1] = gDigitsLut[b1 + 1];
        buffer[2] = gDigitsLut[b2];
        buffer[3] = gDigitsLut[b2 + 1];
        buffer[4] = gDigitsLut[c1];
        buffer[5] = gDigitsLut[c1 + 1];
        buffer[6] = gDigitsLut[c2];
        buffer[7] = gDigitsLut[c2 + 1];
    }

    uint32_t value32 = static_cast<uint32_t>(value);
    while (value32 >= 100) {
        const unsigned i = static_cast<unsigned>(value32 % 100) << 1;
        value32 /= 100;
        *--buffer = gDigitsLut[i + 1];
        *--buffer = gDigitsLut[i];
    }

    if (value32 < 10) {
        *--buffer = char(value32) + '0';
    }
    else {
        const unsigned i = static_cast<unsigned>(value32) << 1;
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
