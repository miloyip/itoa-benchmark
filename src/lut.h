#pragma once

// Use lookup table of two digits

const char gDigitsLut[201] =
	"0001020304050607080910111213141516171819"
	"2021222324252627282930313233343536373839"
	"4041424344454647484950515253545556575859"
	"6061626364656667686970717273747576777879"
	"8081828384858687888990919293949596979899";

inline void u32toa_lut(uint32_t value, char* buffer) {
	char temp[10];
	char* p = temp;
	
	while (value >= 100) {
		const unsigned i = (value % 100) << 1;
		value /= 100;
		*p++ = gDigitsLut[i + 1];
		*p++ = gDigitsLut[i];
	}

	if (value < 10)
		*p++ = char(value) + '0';
	else {
		const unsigned i = value << 1;
		*p++ = gDigitsLut[i + 1];
		*p++ = gDigitsLut[i];
	}

	do {
		*buffer++ = *--p;
	} while (p != temp);

	*buffer = '\0';
}

inline void i32toa_lut(int32_t value, char* buffer) {
	if (value < 0) {
		*buffer++ = '-';
		value = -value;
	}

	u32toa_lut(static_cast<uint32_t>(value), buffer);
}

inline void u64toa_lut(uint64_t value, char* buffer) {
	char temp[20];
	char* p = temp;
	
	while (value >= 100) {
		const unsigned i = static_cast<unsigned>(value % 100) << 1;
		value /= 100;
		*p++ = gDigitsLut[i + 1];
		*p++ = gDigitsLut[i];
	}

	if (value < 10)
		*p++ = char(value) + '0';
	else {
		const unsigned i = static_cast<unsigned>(value) << 1;
		*p++ = gDigitsLut[i + 1];
		*p++ = gDigitsLut[i];
	}

	do {
		*buffer++ = *--p;
	} while (p != temp);

	*buffer = '\0';
}

inline void i64toa_lut(int64_t value, char* buffer) {
	if (value < 0) {
		*buffer++ = '-';
		value = -value;
	}

	u64toa_lut(static_cast<uint64_t>(value), buffer);
}
