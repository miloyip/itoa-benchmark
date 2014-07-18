#pragma once
#include <cstdio>
#include <cinttypes>

inline void u32toa_sprintf(uint32_t value, char* buffer) {
	sprintf(buffer, "%u", value);
}

inline void i32toa_sprintf(int32_t value, char* buffer) {
	sprintf(buffer, "%d", value);
}

inline void u64toa_sprintf(uint64_t value, char* buffer) {
	sprintf(buffer, "%" PRIu64, value);
}

inline void i64toa_sprintf(int64_t value, char* buffer) {
	sprintf(buffer, "%" PRIi64, value);
}
