#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <exception>
#include <limits>
#include <string>
#include <stdint.h>
#include <stdlib.h>
#include "resultfilename.h"
#include "timer.h"
#include "test.h"

const unsigned kIterationPerDigit = 100000;
const unsigned kIterationForRandom = 100;
const unsigned kTrial = 10;

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
static void VerifyValue(T value, void(*f)(T, char*), void(*g)(T, char*), const char* fname, const char* gname) {
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
static void Verify(void(*f)(T, char*), void(*g)(T, char*), const char* fname, const char* gname) {
    printf("Verifying %s = %s ... ", fname, gname);

    // Boundary cases
    VerifyValue<T>(0, f, g, fname, gname);
    VerifyValue<T>(std::numeric_limits<T>::min(), f, g, fname, gname);
    VerifyValue<T>(std::numeric_limits<T>::max(), f, g, fname, gname);

    // 2^n - 1, 2^n, 10^n - 1, 10^n until overflow
    for (uint32_t power = 2; power <= 10; power += 8) {
        T i = 1, last;
        do {
            VerifyValue<T>(i - 1, f, g, fname, gname);
            VerifyValue<T>(i, f, g, fname, gname);
            if (std::numeric_limits<T>::min() < 0) {
                VerifyValue<T>(Traits<T>::Negate(i), f, g, fname, gname);
                VerifyValue<T>(Traits<T>::Negate(i + 1), f, g, fname, gname);
            }
            last = i;
            i *= power;
        } while (last < i);
    }

    printf("OK\n");
}

void VerifyAll() {
    const TestList& tests = TestManager::Instance().GetTests();

    // Find naive for verification
    const Test* naive = 0;
    for (TestList::const_iterator itr = tests.begin(); itr != tests.end(); ++itr)
    if (strcmp((*itr)->fname, "naive") == 0) {
        naive = *itr;
        break;
    }

    assert(naive != 0);

    for (TestList::const_iterator itr = tests.begin(); itr != tests.end(); ++itr) {
        if (strcmp((*itr)->fname, "null") != 0) {   // skip null
            try {
                Verify(naive->u32toa, (*itr)->u32toa, "naive_u32toa", (*itr)->fname);
                Verify(naive->i32toa, (*itr)->i32toa, "naive_i32toa", (*itr)->fname);
                Verify(naive->u64toa, (*itr)->u64toa, "naive_u64toa", (*itr)->fname);
                Verify(naive->i64toa, (*itr)->i64toa, "naive_i64toa", (*itr)->fname);
            }
            catch (...) {
            }
        }
    }
}

template <typename T>
void BenchSequential(void(*f)(T, char*), const char* type, const char* fname, FILE* fp) {
    printf("Benchmarking sequential %-20s ... ", fname);

    char buffer[Traits<T>::kBufferSize];
    double minDuration = std::numeric_limits<double>::max();
    double maxDuration = 0.0;

    T start = 1;
    for (int digit = 1; digit <= Traits<T>::kMaxDigit; digit++) {
        T end = (digit == Traits<T>::kMaxDigit) ? std::numeric_limits<T>::max() : start * 10;

        double duration = std::numeric_limits<double>::max();
        for (unsigned trial = 0; trial < kTrial; trial++) {
            T v = start;
            T sign = 1;
            Timer timer;
            timer.Start();
            for (unsigned iteration = 0; iteration < kIterationPerDigit; iteration++) {
                f(v * sign, buffer);
                sign = Traits<T>::Negate(sign);
                if (++v == end)
                    v = start;
            }
            timer.Stop();
            duration = std::min(duration, timer.GetElapsedMilliseconds());
        }

        duration *= 1e6 / kIterationPerDigit; // convert to nano second per operation

        minDuration = std::min(minDuration, duration);
        maxDuration = std::max(maxDuration, duration);
        fprintf(fp, "%s_sequential,%s,%d,%f\n", type, fname, digit, duration);
        start = end;
    }

    printf("[%8.3fns, %8.3fns]\n", minDuration, maxDuration);
}

template <class T>
class RandomData {
public:
    static T* GetData() {
        static RandomData singleton;
        return singleton.mData;
    }

    static const size_t kCountPerDigit = 1000;
    static const size_t kCount = kCountPerDigit * Traits<T>::kMaxDigit;

private:
    RandomData() :
        mData(new T[kCount])
    {
        T* p = mData;
        T start = 1;
        for (int digit = 1; digit <= Traits<T>::kMaxDigit; digit++) {
            T end = (digit == Traits<T>::kMaxDigit) ? std::numeric_limits<T>::max() : start * 10;
            T v = start;
            T sign = 1;
            for (size_t i = 0; i < kCountPerDigit; i++) {
                *p++ = v * sign;
                sign = Traits<T>::Negate(sign);
                if (++v == end)
                    v = start;
            }
            start = end;
        }
        std::random_shuffle(mData, mData + kCount);
    }

    ~RandomData() {
        delete[] mData;
    }

    T* mData;
};

template <typename T>
void BenchRandom(void(*f)(T, char*), const char* type, const char* fname, FILE* fp) {
    printf("Benchmarking     random %-20s ... ", fname);

    char buffer[Traits<T>::kBufferSize];
    T* data = RandomData<T>::GetData();
    size_t n = RandomData<T>::kCount;

    double duration = std::numeric_limits<double>::max();
    for (unsigned trial = 0; trial < kTrial; trial++) {
        Timer timer;
        timer.Start();

        for (unsigned iteration = 0; iteration < kIterationForRandom; iteration++)
        for (size_t i = 0; i < n; i++)
            f(data[i], buffer);

        timer.Stop();
        duration = std::min(duration, timer.GetElapsedMilliseconds());
    }
    duration *= 1e6 / (kIterationForRandom * n); // convert to nano second per operation
    fprintf(fp, "%s_random,%s,0,%f\n", type, fname, duration);

    printf("%8.3fns\n", duration);
}

template <typename T>
void Bench(void(*f)(T, char*), const char* type, const char* fname, FILE* fp) {
    BenchSequential(f, type, fname, fp);
    BenchRandom(f, type, fname, fp);
}


void BenchAll() {
    // Try to write to /result path, where template.php exists
    FILE *fp;
    if ((fp = fopen("../../result/template.php", "r")) != NULL) {
        fclose(fp);
        fp = fopen("../../result/" RESULT_FILENAME, "w");
    }
    else if ((fp = fopen("../result/template.php", "r")) != NULL) {
        fclose(fp);
        fp = fopen("../result/" RESULT_FILENAME, "w");
    }
    else
        fp = fopen(RESULT_FILENAME, "w");

    fprintf(fp, "Type,Function,Digit,Time(ns)\n");

    const TestList& tests = TestManager::Instance().GetTests();

    puts("u32toa");
    for (TestList::const_iterator itr = tests.begin(); itr != tests.end(); ++itr)
        Bench((*itr)->u32toa, "u32toa", (*itr)->fname, fp);

    puts("");
    puts("i32toa");
    for (TestList::const_iterator itr = tests.begin(); itr != tests.end(); ++itr)
        Bench((*itr)->i32toa, "i32toa", (*itr)->fname, fp);

    puts("");
    puts("u64toa");
    for (TestList::const_iterator itr = tests.begin(); itr != tests.end(); ++itr)
        Bench((*itr)->u64toa, "u64toa", (*itr)->fname, fp);

    puts("");
    puts("i64toa");
    for (TestList::const_iterator itr = tests.begin(); itr != tests.end(); ++itr)
        Bench((*itr)->i64toa, "i64toa", (*itr)->fname, fp);

    fclose(fp);
}

int main() {
    // sort tests
    TestList& tests = TestManager::Instance().GetTests();
    std::sort(tests.begin(), tests.end(),
              [](const Test* a, const Test* b) {
                  return std::string{a->fname} < std::string{b->fname};
              });

    VerifyAll();
    BenchAll();
}
