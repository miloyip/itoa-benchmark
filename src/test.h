#pragma once

#include <vector>
#include <string.h>
#include <cstdint>

struct Test;
typedef std::vector<const Test *> TestList;
class TestManager {
public:
    static TestManager& Instance() {
        static TestManager singleton;
        return singleton;
    }

    void AddTest(const Test* test) {
        mTests.push_back(test);
    }

    const TestList& GetTests() const {
        return mTests;
    }

    TestList& GetTests() {
        return mTests;
    }

private:
    TestList mTests;
};

struct Test {
    Test(
        const char* fname,
        void (*u32toa)(uint32_t, char*),
        void (*i32toa)(int32_t, char*),
        void (*u64toa)(uint64_t, char*),
        void (*i64toa)(int64_t, char*))
        :
        fname(fname),
        u32toa(u32toa),
        i32toa(i32toa),
        u64toa(u64toa),
        i64toa(i64toa)
    {
        TestManager::Instance().AddTest(this);
    }

    bool operator<(const Test& rhs) const {
        return strcmp(fname, rhs.fname) < 0;
    }

    const char* fname;
    void (*u32toa)(uint32_t, char*);
    void (*i32toa)(int32_t, char*);
    void (*u64toa)(uint64_t, char*);
    void (*i64toa)(int64_t, char*);
};


#define STRINGIFY(x) #x
#define REGISTER_TEST(f) static Test gRegister##f(STRINGIFY(f), u32toa##_##f, i32toa##_##f, u64toa##_##f, i64toa##_##f)

