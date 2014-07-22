#ifdef _MSC_VER
#include "msinttypes/inttypes.h"
#else
#include <inttypes.h>
#endif
#include <cstdio>
#include "test.h"

void u32toa_sprintf(uint32_t value, char* buffer) {
	sprintf(buffer, "%u", value);
}

void i32toa_sprintf(int32_t value, char* buffer) {
	sprintf(buffer, "%d", value);
}

void u64toa_sprintf(uint64_t value, char* buffer) {
	sprintf(buffer, "%" PRIu64, value);
}

void i64toa_sprintf(int64_t value, char* buffer) {
	sprintf(buffer, "%" PRIi64, value);
}

REGISTER_TEST(sprintf);
