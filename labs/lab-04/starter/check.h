// ===========================================================================
// check.h -- a tiny test harness for Lab 04.  Written for you.
//
//     CHECK(expr)                 records a failure if expr is false
//     CHECK_THROWS(expr, Error)   records a failure if expr does not throw Error
//     section("name")             prints a heading
//     summary()                   prints the result; use as `return summary();`
// ===========================================================================

#ifndef CHECK_H
#define CHECK_H

#include <iostream>

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

#endif  // CHECK_H
