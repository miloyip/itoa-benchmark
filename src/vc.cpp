#ifdef _MSC_VER

#include <stdint.h>
#include <cstdlib>
#include "test.h"

void u32toa_vc(uint32_t value, char* buffer) {
	_ui64toa(value, buffer, 10);	// No 32-bit unsigned version.
}

void i32toa_vc(int32_t value, char* buffer) {
	_itoa(value, buffer, 10);
}

void u64toa_vc(uint64_t value, char* buffer) {
	_ui64toa(value, buffer, 10);
}

void i64toa_vc(int64_t value, char* buffer) {
	_i64toa(value, buffer, 10);
}

REGISTER_TEST(vc);

#endif
