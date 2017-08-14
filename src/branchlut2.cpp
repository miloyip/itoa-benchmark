#include <stdint.h>
#include "digitslut.h"
#include "test.h"


namespace Impl {

struct DigitPair { char data[2]; };

inline void toStringMiddle2(uint8_t x, char*& p) {
    
    *reinterpret_cast<DigitPair*>(p) = reinterpret_cast<const DigitPair*>(gDigitsLut)[x], p += 2;
    
}
inline void toStringBegin2(uint8_t x, char*& p) {
    
    if(x < 10) *p++ = '0' + x;
    else toStringMiddle2(x, p);
    
}
inline void toStringMiddle4(uint16_t x, char*& p) {
    
    uint8_t a = x / 100, b = x % 100;
    toStringMiddle2(a, p); toStringMiddle2(b, p);
    
}
inline void toStringBegin4(uint16_t x, char*& p) {
    
    if(x < 100) toStringBegin2(x, p);
    else { uint8_t a = x / 100, b = x % 100; toStringBegin2(a, p), toStringMiddle2(b, p); }
    
}
inline void toStringMiddle8(uint32_t x, char*& p) {
    
    uint16_t a = x / 10000, b = x % 10000;
    toStringMiddle4(a, p), toStringMiddle4(b, p);
    
}
inline void toStringBegin8(uint32_t x, char*& p) {
    
    if(x < 10000) toStringBegin4(x, p);
    else { uint16_t a = x / 10000, b = x % 10000; toStringBegin4(a, p), toStringMiddle4(b, p); }
    
}
inline void toStringMiddle16(uint64_t x, char*& p) {
    
    uint32_t a = x / 100000000, b = x % 100000000;
    toStringMiddle8(a, p), toStringMiddle8(b, p);
    
}

}

void u32toa_branchlut2(uint32_t x, char* p) {
    
    if(x < 100000000) Impl::toStringBegin8(x, p);
    else {
        uint32_t a = x / 100000000, b = x % 100000000;
        Impl::toStringBegin2(a, p), Impl::toStringMiddle8(b, p);
    }
    *p = 0;
    
}
void i32toa_branchlut2(int32_t x, char* p) {
    
    uint32_t t;
    if(x >= 0) t = x;
    else *p++ = '-', t = -uint32_t(x);
    u32toa_branchlut2(t, p);
    
}
void u64toa_branchlut2(uint64_t x, char* p) {
    
    if(x < 100000000) Impl::toStringBegin8(x, p);
    else if(x < 10000000000000000) {
        uint32_t a = x / 100000000, b = x % 100000000;
        Impl::toStringBegin8(a, p), Impl::toStringMiddle8(b, p);
    } else {
        uint64_t a = x / 10000000000000000, b = x % 10000000000000000;
        Impl::toStringBegin4(a, p), Impl::toStringMiddle16(b, p);
    }
    *p = 0;
    
}
void i64toa_branchlut2(int64_t x, char* p) {
    
    uint64_t t;
    if(x >= 0) t = x;
    else *p++ = '-', t = -uint64_t(x);
    u64toa_branchlut2(t, p);
    
}


REGISTER_TEST(branchlut2);
