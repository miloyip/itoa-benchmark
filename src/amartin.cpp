#include "itoa_ljust.h"
#include "test.h"
using itoa_ljust::itoa;

void u32toa_amartin(uint32_t v, char* p) { itoa(v, p); }
void i32toa_amartin( int32_t v, char* p) { itoa(v, p); }
void u64toa_amartin(uint64_t v, char* p) { itoa(v, p); }
void i64toa_amartin( int64_t v, char* p) { itoa(v, p); }

REGISTER_TEST(amartin);
