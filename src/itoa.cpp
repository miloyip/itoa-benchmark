#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exception>
#include <limits>
#include "timer.h"

#include "null.h"
#include "sprintf.h"
#include "naive.h"
#include "lut.h"
#include "count.h"
#include "countlut.h"
#include "branchlut.h"
#include "vc.h"
#include "sse2.h"

const unsigned kIterationPerDigit = 1000000;

template <typename T>
struct Traits {
};

template <>
struct Traits<uint32_t> {
	enum { kBufferSize = 11 };
	enum { kMaxDigit = 10 };
	static uint32_t Negate(uint32_t x) { return x; };
};

template <>
struct Traits<int32_t> {
	enum { kBufferSize = 12 };
	enum { kMaxDigit = 10 };
	static int32_t Negate(int32_t x) { return -x; };
};

template <>
struct Traits<uint64_t> {
	enum { kBufferSize = 21 };
	enum { kMaxDigit = 20 };
	static uint64_t Negate(uint64_t x) { return x; };
};

template <>
struct Traits<int64_t> {
	enum { kBufferSize = 22 };
	enum { kMaxDigit = 20 };
	static int64_t Negate(int64_t x) { return -x; };
};

template <typename T>
static void test(T value, void(*f)(T, char*), void(*g)(T, char*), const char* fname, const char* gname) {
	char buffer1[Traits<T>::kBufferSize];
	char buffer2[Traits<T>::kBufferSize];

	f(value, buffer1);
	g(value, buffer2);

	if (strcmp(buffer1, buffer2) != 0) {
		printf("\nError: %s -> %s, %s -> %s\n", fname, buffer1, gname, buffer2);
		throw std::exception();
	}
	//puts(buffer1);
}

template <typename T>
static void verify(void(*f)(T, char*), void(*g)(T, char*), const char* fname, const char* gname) {
	printf("Verifying %s = %s ... ", fname, gname);

	// Boundary cases
	test<T>(0, f, g, fname, gname);
	test<T>(std::numeric_limits<T>::min(), f, g, fname, gname);
	test<T>(std::numeric_limits<T>::max(), f, g, fname, gname);

	// 2^n - 1, 2^n, 10^n - 1, 10^n until overflow
	for (uint32_t power = 2; power <= 10; power += 8) {
		T i = 1, last;
		do {
			test<T>(i - 1, f, g, fname, gname);
			test<T>(i, f, g, fname, gname);
			if (std::numeric_limits<T>::min() < 0) {
				test<T>(Traits<T>::Negate(i), f, g, fname, gname);
				test<T>(Traits<T>::Negate(i + 1), f, g, fname, gname);
			}
			last = i;
			i *= power;
		} while (last < i);
	}

	printf("OK\n");
}

template <typename T>
void bench(void(*f)(T, char*), const char* type, const char* fname, FILE* fp) {
	printf("Benchmarking %-20s ... ", fname);

	char buffer[Traits<T>::kBufferSize];
	double minDuration = 0.0;
	double maxDuration = 0.0;

	T start = 1;
	for (int digit = 1; digit <= Traits<T>::kMaxDigit; digit++) {
		T end = (digit == Traits<T>::kMaxDigit) ? std::numeric_limits<T>::max() : start * 10;

		T v = start;
		Timer timer;
		timer.Start();
		for (unsigned iteration = 0; iteration < kIterationPerDigit; iteration++) {
			f(v, buffer);
			if (v++ == end)
				v = start;
		}
		timer.Stop();
		//printf("Digit %d Time %fms\n", digit, timer.GetElapsedMilliseconds());
		double duration = timer.GetElapsedMilliseconds();
		if (minDuration > duration || digit == 1)
			minDuration = duration;
		if (maxDuration < duration)
			maxDuration = duration;
		fprintf(fp, "%s,%s,%d,%f\n", type, fname, digit, duration);
		start = end;
	}

	printf("[%8.3fms, %8.3fms]\n", minDuration, maxDuration);
}

#define STRINGIFY(x) #x
#define FUNC_NAME(type, f) STRINGIFY(type) "_" STRINGIFY(f)

#define VERIFY(type, f, g) try { verify(type##_##f, type##_##g, FUNC_NAME(type, f), FUNC_NAME(type, g)); } catch (...) {}

void Verify() {
	VERIFY(u32toa, naive, sprintf);
	VERIFY(u32toa, naive, lut);
	VERIFY(u32toa, naive, count);
	VERIFY(u32toa, naive, countlut);
	VERIFY(u32toa, naive, branchlut);
	VERIFY(u32toa, naive, sse2);

#ifdef _MSC_VER
	VERIFY(u32toa, naive, vc);
#endif

	VERIFY(i32toa, naive, sprintf);
	VERIFY(i32toa, naive, lut);
	VERIFY(i32toa, naive, count);
	VERIFY(i32toa, naive, countlut);
	VERIFY(i32toa, naive, branchlut);
	VERIFY(i32toa, naive, sse2);

#ifdef _MSC_VER
	VERIFY(i32toa, naive, vc);
#endif

	VERIFY(u64toa, naive, sprintf);
	VERIFY(u64toa, naive, lut);
	VERIFY(u64toa, naive, count);
	VERIFY(u64toa, naive, countlut);
	VERIFY(u64toa, naive, branchlut);
	VERIFY(u64toa, naive, sse2);

#ifdef _MSC_VER
	VERIFY(u64toa, naive, vc);
#endif

	VERIFY(i64toa, naive, sprintf);
	VERIFY(i64toa, naive, lut);
	VERIFY(i64toa, naive, count);
	VERIFY(i64toa, naive, countlut);
	VERIFY(i64toa, naive, branchlut);
	VERIFY(i64toa, naive, sse2);

#ifdef _MSC_VER
	VERIFY(i64toa, naive, vc);
#endif

	puts("");
}

#define BENCH(type, f) bench(type##_##f, STRINGIFY(type), STRINGIFY(f), fp)

void Bench() {
	// Try to write to /result path, where template.php exists
	FILE *fp;
	if ((fp = fopen("../../result/template.php", "r")) != NULL) {
		fclose(fp);
		fp = fopen("../../result/result.csv", "w");
	}
	else if ((fp = fopen("../result/template.php", "r")) != NULL) {
		fclose(fp);
		fp = fopen("../result/result.csv", "w");
	}
	else
		fp = fopen("result.csv", "w");

	fprintf(fp, "Type,Function,Digit,Time(ms)\n");

	puts("u32toa");

	BENCH(u32toa, null);
	BENCH(u32toa, sprintf);
	BENCH(u32toa, naive);
	BENCH(u32toa, lut);
	BENCH(u32toa, count);
	BENCH(u32toa, countlut);
    BENCH(u32toa, branchlut);
	BENCH(u32toa, sse2);

#ifdef _MSC_VER
	BENCH(u32toa, vc);
#endif

	puts("");
	puts("i32toa");

	BENCH(i32toa, null);
	BENCH(i32toa, sprintf);
	BENCH(i32toa, naive);
	BENCH(i32toa, lut);
	BENCH(i32toa, count);
	BENCH(i32toa, countlut);
	BENCH(i32toa, branchlut);
	BENCH(i32toa, sse2);

#ifdef _MSC_VER
	BENCH(i32toa, vc);
#endif

	puts("");
	puts("u64toa");

	BENCH(u64toa, null);
	BENCH(u64toa, sprintf);
	BENCH(u64toa, naive);
	BENCH(u64toa, lut);
	BENCH(u64toa, count);
	BENCH(u64toa, countlut);
	BENCH(u64toa, branchlut);
	BENCH(u64toa, sse2);

#ifdef _MSC_VER
	BENCH(u64toa, vc);
#endif

	puts("");
	puts("i64toa");

	BENCH(i64toa, null);
	BENCH(i64toa, sprintf);
	BENCH(i64toa, naive);
	BENCH(i64toa, lut);
	BENCH(i64toa, count);
	BENCH(i64toa, countlut);
	BENCH(i64toa, branchlut);
	BENCH(i64toa, sse2);

#ifdef _MSC_VER
	BENCH(i64toa, vc);
#endif

	puts("");

	fclose(fp);
}

int main() {
	char buffer[32];
	u64toa_sse2(987654321, buffer);
	Verify();
	Bench();
}
