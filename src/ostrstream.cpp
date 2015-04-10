#ifdef _MSC_VER
#include "msinttypes/inttypes.h"
#else
#include <inttypes.h>
#endif
#include <strstream>
#include "test.h"

void u32toa_ostrstream(uint32_t value, char* buffer) {
    std::ostrstream oss(buffer, 11);
    oss << value << std::ends;
}

void i32toa_ostrstream(int32_t value, char* buffer) {
    std::ostrstream oss(buffer, 12);
    oss << value << std::ends;
}

void u64toa_ostrstream(uint64_t value, char* buffer) {
    std::ostrstream oss(buffer, 21);
    oss << value << std::ends;
}

void i64toa_ostrstream(int64_t value, char* buffer) {
    std::ostrstream oss(buffer, 22);
    oss << value << std::ends;
}

#if RUN_CPPITOA
REGISTER_TEST(ostrstream);
#endif
