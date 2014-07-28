#ifdef _MSC_VER
#include "msinttypes/inttypes.h"
#else
#include <inttypes.h>
#endif
#include <sstream>
#include "test.h"

void u32toa_ostringstream(uint32_t value, char* buffer) {
	std::ostringstream oss;
	oss << value;
	strcpy(buffer, oss.str().c_str());
}

void i32toa_ostringstream(int32_t value, char* buffer) {
	std::ostringstream oss;
	oss << value;
	strcpy(buffer, oss.str().c_str());
}

void u64toa_ostringstream(uint64_t value, char* buffer) {
	std::ostringstream oss;
	oss << value;
	strcpy(buffer, oss.str().c_str());
}

void i64toa_ostringstream(int64_t value, char* buffer) {
	std::ostringstream oss;
	oss << value;
	strcpy(buffer, oss.str().c_str());
}

REGISTER_TEST(ostringstream);
