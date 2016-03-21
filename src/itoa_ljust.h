#ifndef ITOA_LJUST_H
#define ITOA_LJUST_H

//=== itoa_ljust.h - Fast integer to ascii conversion             --*- C++ -*-//
//
// Fast and simple integer to ASCII conversion:
//
//   - 32 and 64-bit integers
//   - signed and unsigned
//   - user supplied buffer must be large enough for all decimal digits
//     in value plus minus sign if negative
//   - left-justified
//   - NUL terminated
//   - return value is pointer to NUL terminator
//
// Copyright (c) 2016 Arturo Martin-de-Nicolas
// arturomdn@gmail.com
// https://github.com/amdn/itoa_ljust/
//===----------------------------------------------------------------------===//

#include <stdint.h>

namespace itoa_ljust {

    char* itoa(uint32_t u, char* buffer);
    char* itoa( int32_t i, char* buffer);
    char* itoa(uint64_t u, char* buffer);
    char* itoa( int64_t i, char* buffer);

}

#endif // ITOA_LJUST_H
