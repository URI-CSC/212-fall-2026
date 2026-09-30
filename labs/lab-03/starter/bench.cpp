// ===========================================================================
// bench.cpp -- how much does growing by a constant really cost?  Written for
// you.
//
// For each increment c, appends n ints to an empty DynArray(c), for
// n = 2^10, 2^11, ..., each double the last, and reports
//
//     copies   element copies made by grow()   (exact, from your counter)
//     grows    how many times grow() ran        (exact, from your counter)
//     ms       wall clock time                  (depends on your machine)
//
// and the doubling ratio of each, as in Lab 01.  It also writes every row to
// growth.csv, which ../viewer/index.html turns into a plot.
//
// Build and run:
//     $ g++ -std=c++17 -Wall -Wextra -O2 dynarray.cpp bench.cpp -o bench
//     $ ./bench 17
//
// Usage:  ./bench [max_exponent]
//     max_exponent defaults to 17, so n runs 2^10 = 1024 up to 2^17 = 131072.
// ===========================================================================

#include "dynarray.h"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

static const size_t INCREMENTS[] = {1, 10, 100, 1000};

struct Result {
    size_t copies;
    size_t grows;
    double ms;
};

// One run.  The copies are exact, so there is nothing to average.  The time
// is a single measurement: treat its ratio as a sanity check on the copies,
// not as the evidence.
static Result append_n(size_t c, size_t n, long long& sink) {
    const auto start = std::chrono::steady_clock::now();
    DynArray a(c);
    for (size_t i = 0; i < n; i++) {
        a.push_back(static_cast<int>(i));
    }
    const auto end = std::chrono::steady_clock::now();

    // Read something back, so the compiler cannot decide the loop is unused.
    sink += a.at(n / 2);
    return {a.copies(), a.grows(),
            std::chrono::duration<double, std::milli>(end - start).count()};
}

int main(int argc, char* argv[]) {
    const int max_exp = (argc > 1) ? std::atoi(argv[1]) : 17;
    if (max_exp < 10 || max_exp > 18) {
        std::cerr << "usage: " << argv[0] << " [max_exponent]\n"
                  << "max_exponent must be between 10 and 18\n";
        return 1;
    }

    std::ofstream csv("growth.csv");
    csv << "increment,n,copies,grows,ms\n";
    long long sink = 0;

    for (size_t c : INCREMENTS) {
        std::cout << "\ngrow by c = " << c << '\n';
        std::cout << std::left << std::setw(9) << "n" << std::right
                  << std::setw(14) << "copies" << std::setw(8) << "ratio"
                  << std::setw(9) << "grows" << std::setw(12) << "ms"
                  << std::setw(8) << "ratio" << '\n';
        std::cout << std::string(60, '-') << '\n';

        Result prev = {0, 0, 0.0};
        for (int e = 10; e <= max_exp; e++) {
            const size_t n = static_cast<size_t>(1) << e;
            const Result r = append_n(c, n, sink);

            std::cout << std::left << std::setw(9) << n << std::right
                      << std::setw(14) << r.copies;
            if (prev.copies > 0) {
                std::cout << std::setw(8) << std::fixed << std::setprecision(2)
                          << static_cast<double>(r.copies) / prev.copies;
            } else {
                std::cout << std::setw(8) << "-";
            }
            std::cout << std::setw(9) << r.grows << std::setw(12) << std::fixed
                      << std::setprecision(3) << r.ms;
            if (prev.ms > 0.0) {
                std::cout << std::setw(8) << std::setprecision(2) << r.ms / prev.ms;
            } else {
                std::cout << std::setw(8) << "-";
            }
            std::cout << '\n';

            csv << c << ',' << n << ',' << r.copies << ',' << r.grows << ','
                << std::setprecision(4) << r.ms << '\n';
            prev = r;
        }
    }

    std::cout << "\nwrote growth.csv (checksum " << sink << ", ignore it)\n";
    return 0;
}
