#pragma once

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Timer {
public:
	Timer() : start_(), end_() {
	}

	void Start() {
		QueryPerformanceCounter(&start_);
	}

	void Stop() {
		QueryPerformanceCounter(&end_);
	}

	double GetElapsedMilliseconds() {
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		return (end_.QuadPart - start_.QuadPart) * 1000.0 / freq.QuadPart;
	}

private:
	LARGE_INTEGER start_;
	LARGE_INTEGER end_;
};

// Undefine Windows bad macros
#undef min
#undef max

#else

#Error need platform implementation of timer

#endif
