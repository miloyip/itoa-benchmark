#include "itoa_ljust_impl.h"
#include "test.h"

void u32toa_amartin(uint32_t v, char* out) { (void)to_dec(out,v); }
void u64toa_amartin(uint64_t v, char* out) { (void)to_dec(out,v); }
void i32toa_amartin( int32_t v, char* out) { (void)to_dec(out,v); }
void i64toa_amartin( int64_t v, char* out) { (void)to_dec(out,v); }

REGISTER_TEST(amartin);
