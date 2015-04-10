#include <stdint.h>
#include <algorithm>
#include "test.h"

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

// Efficient Integer to String Conversions, by Matthew Wilson.
template<typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char* p = buf;

    do {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0) {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

void u32toa_mwilson(uint32_t value, char* buffer) {
    convert(buffer, value);
}

void i32toa_mwilson(int32_t value, char* buffer) {
    convert(buffer, value);
}

void u64toa_mwilson(uint64_t value, char* buffer) {
    convert(buffer, value);
}

void i64toa_mwilson(int64_t value, char* buffer) {
    convert(buffer, value);
}

REGISTER_TEST(mwilson);
