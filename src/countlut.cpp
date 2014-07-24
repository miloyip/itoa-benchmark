#include "countdecimaldigit.h"
#include "digitslut.h"
#include "test.h"

// Additional count number of digit pass
// Use lookup table of two gDigitsLut

void u32toa_countlut(uint32_t value, char* buffer) {
    unsigned digit = CountDecimalDigit32(value);
    buffer += digit;
    *buffer = '\0';

    const uint16_t* gDigitsLut16 = reinterpret_cast<const uint16_t*>(gDigitsLut);

    while (value >= 100) {
        buffer -= 2;
        reinterpret_cast<uint16_t*>(buffer)[0] = gDigitsLut16[value % 100];

        value /= 100;
    }

    if (value < 10) {
        --buffer;
        buffer[0] = char(value) + '0';
    }
    else {
        buffer -= 2;
        reinterpret_cast<uint16_t*>(buffer)[0] = gDigitsLut16[value];
    }
}

void i32toa_countlut(int32_t value, char* buffer) {
    if (value < 0) {
        *buffer++ = '-';
        value = -value;
    }

    u32toa_countlut(static_cast<uint32_t>(value), buffer);
}

void u64toa_countlut(uint64_t value, char* buffer) {
    unsigned digit = CountDecimalDigit64(value);
    buffer += digit;
    *buffer = '\0';

    const uint16_t* gDigitsLut16 = reinterpret_cast<const uint16_t*>(gDigitsLut);

    while (value >= 100000000) {
        uint32_t a = static_cast<uint32_t>(value % 100000000);
        value /= 100000000;

        uint32_t b = a / 10000;
        uint32_t c = a % 10000;

        buffer -= 8;
        reinterpret_cast<uint16_t*>(buffer)[0] = gDigitsLut16[b / 100];
        reinterpret_cast<uint16_t*>(buffer)[1] = gDigitsLut16[b % 100];
        reinterpret_cast<uint16_t*>(buffer)[2] = gDigitsLut16[c / 100];
        reinterpret_cast<uint16_t*>(buffer)[3] = gDigitsLut16[c % 100];
    }

    uint32_t value32 = static_cast<uint32_t>(value);
    while (value32 >= 100) {
        buffer -= 2;
        reinterpret_cast<uint16_t*>(buffer)[0] = gDigitsLut16[value32 % 100];

        value32 /= 100;
    }

    if (value32 < 10) {
        --buffer;
        buffer[0] = char(value32) + '0';
    } else {
        buffer -= 2;
        reinterpret_cast<uint16_t*>(buffer)[0] = gDigitsLut16[value32];
    }
}

void i64toa_countlut(int64_t value, char* buffer) {
    if (value < 0) {
        *buffer++ = '-';
        value = -value;
    }

    u64toa_countlut(static_cast<uint64_t>(value), buffer);
}

REGISTER_TEST(countlut);
