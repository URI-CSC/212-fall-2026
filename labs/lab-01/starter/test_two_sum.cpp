// ===========================================================================
// test_two_sum.cpp -- the test runner for Lab 01.
//
// You do not need to change anything above the "YOUR TESTS" section, but you
// should READ the harness.  It is about fifteen lines, and it is the whole
// reason a failing test tells you *which* case broke instead of just dying.
//
// Build and run:
//     $ g++ -std=c++17 -Wall -Wextra -g two_sum.cpp test_two_sum.cpp -o test_two_sum
//     $ ./test_two_sum
// ===========================================================================

#include "two_sum.h"

#include <iostream>
#include <random>
#include <vector>

// --------------------------- the tiny harness ------------------------------

static int checks_run = 0;
static int checks_failed = 0;

static void record(bool ok, const char* expr, int line) {
    ++checks_run;
    if (!ok) {
        ++checks_failed;
        std::cout << "    FAIL  line " << line << ":  " << expr << '\n';
    }
}

// The # in #expr is the stringizing operator: it turns the *text* of the
// expression into a string literal, which is how the message above can name
// the check that failed.  __LINE__ is the line it appeared on.
#define CHECK(expr) record((expr), #expr, __LINE__)

// Most of our checks want to assert the same thing about both implementations.
#define CHECK_BOTH(arr, target, expected)                          \
    do {                                                           \
        CHECK(two_sum_brute_force((arr), (target)) == (expected));  \
        CHECK(two_sum_two_pointer((arr), (target)) == (expected));  \
    } while (0)

static void section(const char* name) {
    std::cout << "  " << name << '\n';
}

// ------------------------------- the tests ---------------------------------

static void test_given_examples() {
    section("given examples");
    std::vector<int> a = {-2, 7, 11, 0};
    CHECK_BOTH(a,  9, true);   // 11 + (-2)
    CHECK_BOTH(a, -2, true);   // -2 +   0
    CHECK_BOTH(a, -1, false);
    CHECK_BOTH(a,  0, false);  // NOT true: that would need 0 + 0, one element twice

    std::vector<int> b = {-10, -5, 0, 5, 10, -4, -3, -2, -1, 0};
    CHECK_BOTH(b, -15, true);  // -10 + (-5)
    CHECK_BOTH(b,   0, true);  // -10 +  10
    CHECK_BOTH(b, 500, false);
    CHECK_BOTH(b,  17, false); // largest possible pair here is 10 + 5 = 15
}

static void test_tiny_arrays() {
    section("empty / 1 / 2 elements");
    std::vector<int> empty;
    std::vector<int> one = {5};
    std::vector<int> two = {5, 7};

    CHECK_BOTH(empty, 0, false);
    CHECK_BOTH(empty, 10, false);
    CHECK_BOTH(one, 5, false);
    CHECK_BOTH(one, 10, false);   // 5 + 5 would reuse the only element
    CHECK_BOTH(two, 12, true);
    CHECK_BOTH(two, 13, false);
}

static void test_distinct_positions_not_distinct_values() {
    section("two positions, not two values");
    std::vector<int> dup = {3, 3, 3};
    CHECK_BOTH(dup, 6, true);    // two different 3s: fine
    CHECK_BOTH(dup, 9, false);   // that needs three elements
    CHECK_BOTH(dup, 3, false);

    std::vector<int> zeros = {0, 4, 7};
    CHECK_BOTH(zeros, 0, false); // only one 0 -> cannot make 0
    std::vector<int> two_zeros = {0, 4, 0};
    CHECK_BOTH(two_zeros, 0, true);
}

static void test_negatives() {
    section("negatives");
    std::vector<int> neg = {-1, -2, -3, -4};
    CHECK_BOTH(neg, -5, true);
    CHECK_BOTH(neg, -7, true);
    CHECK_BOTH(neg, -1, false);
    CHECK_BOTH(neg,  0, false);
}

static void test_input_is_not_modified() {
    section("the caller's array survives");
    std::vector<int> a = {9, 1, 8, 2, 7};
    const std::vector<int> before = a;

    two_sum_brute_force(a, 10);
    CHECK(a == before);
    two_sum_two_pointer(a, 10);
    CHECK(a == before);   // fails if you sorted `array` instead of a copy
}

// Differential testing: on small random inputs, a slow-but-obviously-correct
// reference and your two functions must agree on EVERY target.  This finds
// off-by-one bugs that hand-picked cases sail straight past.
static bool reference(const std::vector<int>& a, int target) {
    for (size_t i = 0; i < a.size(); ++i)
        for (size_t j = i + 1; j < a.size(); ++j)
            if (a[i] + a[j] == target) return true;
    return false;
}

static void test_agreement_on_random_inputs() {
    section("agreement on 2000 random inputs");
    std::mt19937 gen(212);                       // fixed seed -> reproducible
    std::uniform_int_distribution<int> value(-9, 9);
    std::uniform_int_distribution<int> length(0, 8);

    int mismatches = 0;
    for (int trial = 0; trial < 2000; ++trial) {
        std::vector<int> a(static_cast<size_t>(length(gen)));
        for (int& x : a) x = value(gen);
        for (int target = -18; target <= 18; ++target) {
            const bool want = reference(a, target);
            if (two_sum_brute_force(a, target) != want) ++mismatches;
            if (two_sum_two_pointer(a, target) != want) ++mismatches;
        }
    }
    CHECK(mismatches == 0);
    if (mismatches != 0) {
        std::cout << "    (" << mismatches << " disagreements with the reference)\n";
    }
}

// --------------------------------------------------------------------------
// YOUR TESTS -- add at least two.  One of them must be a case you got wrong
// at some point, or expected to get wrong.  A test that has never failed has
// never told you anything.
// --------------------------------------------------------------------------
static void test_your_cases() {
    section("your cases");
    // TODO: e.g. CHECK_BOTH(some_vector, some_target, true_or_false);
}

int main() {
    std::cout << "running tests\n";
    test_given_examples();
    test_tiny_arrays();
    test_distinct_positions_not_distinct_values();
    test_negatives();
    test_input_is_not_modified();
    test_agreement_on_random_inputs();
    test_your_cases();

    std::cout << '\n'
              << checks_run - checks_failed << " / " << checks_run
              << " checks passed\n";
    if (checks_failed == 0) {
        std::cout << "ALL TESTS PASSED\n";
    } else {
        std::cout << checks_failed << " CHECK(S) FAILED\n";
    }
    return checks_failed == 0 ? 0 : 1;
}
