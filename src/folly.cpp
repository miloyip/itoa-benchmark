#ifdef HAS_FOLLY

#include "test.h"
#include <folly/Conv.h>

using namespace folly;

// Refer to the code from Folly/Conv.h
// template <class Tgt, class Src>
// typename std::enable_if<
//   std::is_integral<Src>::value && std::is_signed<Src>::value &&
//   IsSomeString<Tgt>::value && sizeof(Src) >= 4>::type
// toAppend(Src value, Tgt * result)

void u32toa_folly(uint32_t value, char* buffer) {
    buffer[uint64ToBufferUnsafe(value, buffer)] = '\0';
}

void i32toa_folly(int32_t value, char* buffer) {
    if (value < 0) {
        *buffer++ = '-';
        buffer[uint64ToBufferUnsafe(-uint64_t(value), buffer)] = '\0';
    }
    else
        buffer[uint64ToBufferUnsafe(value, buffer)] = '\0';
}

void u64toa_folly(uint64_t value, char* buffer) {
    buffer[uint64ToBufferUnsafe(value, buffer)] = '\0';
}

void i64toa_folly(int64_t value, char* buffer) {
    if (value < 0) {
        *buffer++ = '-';
        buffer[uint64ToBufferUnsafe(-uint64_t(value), buffer)] = '\0';
    }
    else
        buffer[uint64ToBufferUnsafe(value, buffer)] = '\0';
}

REGISTER_TEST(folly);

#endif
