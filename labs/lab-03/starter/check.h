// ===========================================================================
// check.h -- a tiny test harness for Lab 03.  Written for you.
//
//     CHECK(expr)                 records a failure if expr is false
//     CHECK_THROWS(expr, Error)   records a failure if expr does not throw Error
//     section("name")             prints a heading
//     arrays_in_use()             how many new[] blocks are not freed yet
//     summary()                   prints the result; use as `return summary();`
//
// Include it from test_dynarray.cpp ONLY.  It replaces the global new[] and
// delete[], so it must appear in exactly one .cpp file.
// ===========================================================================

#ifndef CHECK_H
#define CHECK_H

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <new>

static int checks_run = 0;
static int checks_failed = 0;

static void record(bool ok, const char* expr, int line) {
    ++checks_run;
    if (!ok) {
        ++checks_failed;
        std::cout << "    FAIL  line " << line << ":  " << expr << '\n';
    }
}

#define CHECK(expr) record((expr), #expr, __LINE__)

#define CHECK_THROWS(expr, Error)                                 \
    do {                                                          \
        bool threw = false;                                       \
        try {                                                     \
            expr;                                                 \
        } catch (const Error&) {                                  \
            threw = true;                                         \
        }                                                         \
        record(threw, "throws " #Error ": " #expr, __LINE__);     \
    } while (0)

static void section(const char* name) { std::cout << "  " << name << '\n'; }

static int summary() {
    std::cout << '\n';
    if (checks_failed == 0) {
        std::cout << "ALL TESTS PASSED (" << checks_run << " checks)\n";
        return 0;
    }
    std::cout << checks_failed << " of " << checks_run << " checks FAILED\n";
    return 1;
}

// Every new[] adds 1, every delete[] subtracts 1.  If the number is higher
// after your objects are gone, something was never freed.
static long long arrays_live = 0;

static long long arrays_in_use() { return arrays_live; }

void* operator new[](std::size_t n) {
    void* p = std::malloc(n == 0 ? 1 : n);
    if (p == nullptr) throw std::bad_alloc();
    ++arrays_live;
    return p;
}

void operator delete[](void* p) noexcept {
    if (p != nullptr) {
        --arrays_live;
        std::free(p);
    }
}

void operator delete[](void* p, std::size_t) noexcept { operator delete[](p); }

#endif  // CHECK_H
