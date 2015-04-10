#include "countdecimaldigit.h"
#include "test.h"

// Additional count number of digit pass 

void u32toa_count(uint32_t value, char* buffer) {
	unsigned digit = CountDecimalDigit32(value);
	buffer += digit;
	*buffer = '\0';

	do {
		*--buffer = char(value % 10) + '0';
		value /= 10;
	} while (value > 0);
}

void i32toa_count(int32_t value, char* buffer) {
    uint32_t u = static_cast<uint32_t>(value);
    if (value < 0) {
        *buffer++ = '-';
        u = ~u + 1;
    }
    u32toa_count(u, buffer);
}

void u64toa_count(uint64_t value, char* buffer) {
	unsigned digit = CountDecimalDigit64(value);
	buffer += digit;
	*buffer = '\0';

	do {
		*--buffer = char(value % 10) + '0';
		value /= 10;
	} while (value > 0);
}

void i64toa_count(int64_t value, char* buffer) {
    uint64_t u = static_cast<uint64_t>(value);
    if (value < 0) {
        *buffer++ = '-';
        u = ~u + 1;
    }
    u64toa_count(u, buffer);
}

REGISTER_TEST(count);
