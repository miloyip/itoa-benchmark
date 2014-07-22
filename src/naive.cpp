#include <stdint.h>
#include "test.h"

void u32toa_naive(uint32_t value, char* buffer) {
	char temp[10];
	char *p = temp;
	do {
		*p++ = char(value % 10) + '0';
		value /= 10;
	} while (value > 0);

	do {
		*buffer++ = *--p;
	} while (p != temp);

	*buffer = '\0';
}

void i32toa_naive(int32_t value, char* buffer) {
	if (value < 0) {
		*buffer++ = '-';
		value = -value;
	}
	u32toa_naive(static_cast<uint32_t>(value), buffer);
}

void u64toa_naive(uint64_t value, char* buffer) {
	char temp[20];
	char *p = temp;
	do {
		*p++ = char(value % 10) + '0';
		value /= 10;
	} while (value > 0);

	do {
		*buffer++ = *--p;
	} while (p != temp);

	*buffer = '\0';
}

void i64toa_naive(int64_t value, char* buffer) {
	if (value < 0) {
		*buffer++ = '-';
		value = -value;
	}
	u64toa_naive(static_cast<uint64_t>(value), buffer);
}

REGISTER_TEST(naive);
