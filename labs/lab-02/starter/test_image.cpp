// ===========================================================================
// test_image.cpp -- the test runner for Lab 02.
//
// Read the harness.  It is about twenty lines, and it is the reason a failing
// test tells you WHICH pixel broke instead of just crashing.
//
// Build and run:
//     $ g++ -std=c++17 -Wall -Wextra -g image.cpp pgm.cpp test_image.cpp -o test_image
//     $ ./test_image
// ===========================================================================

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <new>
#include <stdexcept>

#include "image.h"
#include "pgm.h"

// --------------------------- the tiny harness ------------------------------

static int checks_run = 0;
static int checks_failed = 0;

static void record(bool ok, const char *expr, int line) {
    ++checks_run;
    if (!ok) {
        ++checks_failed;
        std::cout << "    FAIL  line " << line << ":  " << expr << '\n';
    }
}

// The # in #expr is the stringizing operator: it turns the TEXT of the
// expression into a string, which is how the message above can name the check
// that failed.  __LINE__ is the line it appeared on.
#define CHECK(expr) record((expr), #expr, __LINE__)

// Several checks want "this line must throw std::out_of_range".
#define CHECK_THROWS(expr)                                    \
    do {                                                      \
        bool threw = false;                                   \
        try {                                                 \
            expr;                                             \
        } catch (const std::out_of_range &) {                 \
            threw = true;                                     \
        }                                                     \
        record(threw, "throws out_of_range: " #expr, __LINE__); \
    } while (0)

static void section(const char *name) {
    std::cout << "  " << name << '\n';
}

// ------------------------- counting the arrays -----------------------------
//
// Nothing else in this file notices a missing delete[].  A leaking Image
// gives every right answer until the machine runs out of memory.  So we
// replace the global new[] and delete[] with versions that move a counter.
//
// `new Image(...)` does NOT touch this counter; only arrays do.  So the
// number below is exactly the number of pixel arrays you are holding.

static long long arrays_live = 0;

void *operator new[](std::size_t n) {
    void *p = std::malloc(n == 0 ? 1 : n);
    if (p == nullptr) {
        throw std::bad_alloc();
    }
    ++arrays_live;
    return p;
}

void operator delete[](void *p) noexcept {
    if (p != nullptr) {
        --arrays_live;
        std::free(p);
    }
}

void operator delete[](void *p, std::size_t) noexcept {
    operator delete[](p);
}

static long long arrays_in_use() { return arrays_live; }

// ------------------------------- the tests ---------------------------------

static void test_construction() {
    section("construction");
    Image img(3, 4);
    CHECK(img.get_rows() == 3);
    CHECK(img.get_cols() == 4);

    bool all_zero = true;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 4; c++) {
            if (img.at(r, c) != 0) {
                all_zero = false;
            }
        }
    }
    CHECK(all_zero);
}

static void test_set_and_at() {
    section("set and at");
    Image img(3, 4);
    img.set(0, 0, 7);
    img.set(2, 3, 99);
    img.set(1, 2, 128);
    CHECK(img.at(0, 0) == 7);
    CHECK(img.at(2, 3) == 99);
    CHECK(img.at(1, 2) == 128);
    CHECK(img.at(0, 1) == 0);  // untouched neighbours stay 0
    CHECK(img.at(1, 3) == 0);

    img.set(0, 0, 200);
    CHECK(img.at(0, 0) == 200);  // set overwrites
}

static void test_row_major_layout() {
    section("row by row layout (tiny.pgm)");
    // every pixel of tiny.pgm holds 10 * row + column, so this checks that
    // your index arithmetic puts pixels where the file says they are
    Image *img = nullptr;
    try {
        img = read_pgm("images/tiny.pgm");
    } catch (const std::runtime_error &e) {
        std::cout << "    SKIPPED: " << e.what() << '\n';
        return;
    }
    CHECK(img->get_rows() == 6);
    CHECK(img->get_cols() == 8);

    bool ok = true;
    for (int r = 0; r < img->get_rows(); r++) {
        for (int c = 0; c < img->get_cols(); c++) {
            if (img->at(r, c) != 10 * r + c) {
                ok = false;
            }
        }
    }
    CHECK(ok);
    delete img;
}

static void test_bounds() {
    section("out of range");
    Image img(3, 4);
    CHECK_THROWS(img.at(-1, 0));
    CHECK_THROWS(img.at(0, -1));
    CHECK_THROWS(img.at(3, 0));   // rows are 0, 1, 2
    CHECK_THROWS(img.at(0, 4));   // columns are 0, 1, 2, 3
    CHECK_THROWS(img.set(3, 0, 1));
    CHECK_THROWS(img.set(0, 4, 1));
    CHECK_THROWS(img.set(-1, -1, 1));
}

static void test_invert() {
    section("invert");
    Image img(2, 2);
    img.set(0, 0, 0);
    img.set(0, 1, 255);
    img.set(1, 0, 100);
    img.set(1, 1, 7);
    img.invert();
    CHECK(img.at(0, 0) == 255);
    CHECK(img.at(0, 1) == 0);
    CHECK(img.at(1, 0) == 155);
    CHECK(img.at(1, 1) == 248);

    img.invert();  // twice gets you back where you started
    CHECK(img.at(1, 0) == 100);
}

static void test_flip_horizontal() {
    section("flip_horizontal");
    Image img(2, 3);
    int value = 1;
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < 3; c++) {
            img.set(r, c, value++);
        }
    }
    // rows are 1 2 3 and 4 5 6
    img.flip_horizontal();
    CHECK(img.at(0, 0) == 3);
    CHECK(img.at(0, 1) == 2);
    CHECK(img.at(0, 2) == 1);
    CHECK(img.at(1, 0) == 6);
    CHECK(img.at(1, 2) == 4);
    CHECK(img.get_rows() == 2);  // flipping does not change the shape
    CHECK(img.get_cols() == 3);

    img.flip_horizontal();  // twice gets you back where you started
    CHECK(img.at(0, 0) == 1);
    CHECK(img.at(1, 2) == 6);
}

static void test_transpose() {
    section("transpose");
    Image img(2, 3);
    int value = 1;
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < 3; c++) {
            img.set(r, c, value++);
        }
    }
    img.transpose();
    CHECK(img.get_rows() == 3);  // the shape has to change
    CHECK(img.get_cols() == 2);
    CHECK(img.at(0, 0) == 1);
    CHECK(img.at(0, 1) == 4);
    CHECK(img.at(1, 0) == 2);
    CHECK(img.at(2, 0) == 3);
    CHECK(img.at(2, 1) == 6);

    img.transpose();  // twice gets you back where you started
    CHECK(img.get_rows() == 2);
    CHECK(img.at(0, 2) == 3);
}

static void test_histogram() {
    section("histogram");
    Image img(2, 3);
    img.set(0, 0, 10);
    img.set(0, 1, 10);
    img.set(0, 2, 200);
    img.set(1, 0, 10);
    img.set(1, 1, 0);
    img.set(1, 2, 0);

    int *counts = new int[256];
    for (int v = 0; v < 256; v++) {
        counts[v] = -1;  // your histogram has to zero these itself
    }
    img.histogram(counts);
    CHECK(counts[10] == 3);
    CHECK(counts[200] == 1);
    CHECK(counts[0] == 2);
    CHECK(counts[11] == 0);

    int total = 0;
    for (int v = 0; v < 256; v++) {
        total += counts[v];
    }
    CHECK(total == 6);  // every pixel is counted exactly once
    delete[] counts;
}

static void test_most_common_value() {
    section("most_common_value");
    Image img(2, 3);
    img.set(0, 0, 10);
    img.set(0, 1, 10);
    img.set(0, 2, 200);
    img.set(1, 0, 10);
    img.set(1, 1, 0);
    img.set(1, 2, 0);
    CHECK(img.most_common_value() == 10);

    Image tie(1, 4);
    tie.set(0, 0, 5);
    tie.set(0, 1, 5);
    tie.set(0, 2, 9);
    tie.set(0, 3, 9);
    CHECK(tie.most_common_value() == 5);  // a tie returns the smaller value
}

static void test_auto_contrast() {
    section("auto_contrast");
    Image img(1, 3);
    img.set(0, 0, 100);
    img.set(0, 1, 120);
    img.set(0, 2, 140);
    img.auto_contrast();
    CHECK(img.at(0, 0) == 0);    // the darkest value becomes 0
    CHECK(img.at(0, 2) == 255);  // the brightest becomes 255
    CHECK(img.at(0, 1) == 127);

    Image flat(1, 3);  // one flat colour: nothing to stretch, no dividing by 0
    flat.set(0, 0, 42);
    flat.set(0, 1, 42);
    flat.set(0, 2, 42);
    flat.auto_contrast();
    CHECK(flat.at(0, 0) == 42);
    CHECK(flat.at(0, 2) == 42);
}

static void test_no_leaks() {
    section("no leaks");
    const long long before = arrays_in_use();
    {
        Image a(40, 30);
        Image b(7, 7);
        b.transpose();  // this one allocates a second array: free the first
        Image *c = new Image(5, 5);
        c->auto_contrast();
        c->most_common_value();
        delete c;
    }
    CHECK(arrays_in_use() == before);
}

// ------------------------------- YOUR TESTS --------------------------------
//
// Add at least TWO checks of your own.  At least one of them must be a case
// you actually got wrong, or expected to get wrong.

static void test_your_cases() {
    section("your cases");

    // TODO: your checks here.
}

// --------------------------------- main ------------------------------------

int main() {
    std::cout << "running tests\n";
    test_construction();
    test_set_and_at();
    test_row_major_layout();
    test_bounds();
    test_invert();
    test_flip_horizontal();
    test_transpose();
    test_histogram();
    test_most_common_value();
    test_auto_contrast();
    test_no_leaks();
    test_your_cases();

    std::cout << '\n';
    if (checks_failed == 0) {
        std::cout << "ALL TESTS PASSED (" << checks_run << " checks)\n";
        return 0;
    }
    std::cout << checks_failed << " of " << checks_run << " checks FAILED\n";
    return 1;
}
