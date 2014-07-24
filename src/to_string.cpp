#if __cpluslpus >= 201103L || _MSC_VER >= 1700

#ifdef _MSC_VER
#include "msinttypes/inttypes.h"
#else
#include <inttypes.h>
#endif
#include <string>
#include "test.h"

void u32toa_to_string(uint32_t value, char* buffer) {
	strcpy(buffer, std::to_string(value).c_str());
}

void i32toa_to_string(int32_t value, char* buffer) {
	strcpy(buffer, std::to_string(value).c_str());
}

void u64toa_to_string(uint64_t value, char* buffer) {
	strcpy(buffer, std::to_string(value).c_str());
}

void i64toa_to_string(int64_t value, char* buffer) {
	strcpy(buffer, std::to_string(value).c_str());
}

REGISTER_TEST(to_string);

#endif