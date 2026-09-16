// ===========================================================================
// bench.cpp -- the measuring instrument for Lab 01.
//
// Read this file.  Every choice in it is a decision the lecture told you to
// make on purpose, and they are labelled.  A benchmark that gets these wrong
// still compiles, still runs, and still prints confident numbers.
//
// Build and run:
//     $ g++ -std=c++17 -Wall -Wextra -O2 two_sum.cpp bench.cpp -o bench
//     $ ./bench 15
//
// Usage:  ./bench [max_exponent]
//     max_exponent defaults to 15, so n runs 2^10 = 1024 up to 2^15 = 32768.
// ===========================================================================

#include "two_sum.h"

#include <iostream>    // std::cin, std::cout
#include <vector>      // std::vector
#include <algorithm>   // std::sort, std::max
#include <chrono>      // steady_clock -- timing
#include <random>      // mt19937, uniform_int_distribution
#include <iomanip>     // setw, setprecision -- table formatting
#include <cstdlib>     // std::atoi

// DECISION 1: a FIXED seed.  Both algorithms must see the same array, and so
// must your lab partner, and so must you tomorrow.  An experiment you cannot
// repeat is not an experiment.
static std::vector<int> make_array(size_t n, unsigned seed) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist(-1000, 1000);
    std::vector<int> a(n);
    for (int& x : a) x = dist(gen);
    return a;
}

// DECISION 2: MEDIAN of several runs, after one untimed warm-up run.
// A single cold run measures your caches and your CPU's clock governor at
// least as much as it measures the algorithm.  The mean would let one
// unlucky run (a background process waking up) dominate the number; the
// median ignores it.
//
// DECISION 3: `sink`.  The return value of a 2-SUM call is otherwise unused,
// and at -O2 the compiler is entitled to notice that and delete the call
// outright -- leaving you timing an empty loop.  Feeding the result into a
// variable that gets printed makes the call observable, so it survives.
static double median_seconds(bool (*fn)(const std::vector<int>&, int),
                             const std::vector<int>& array, int target,
                             int reps, unsigned long long& sink) {
    if (fn(array, target)) ++sink;   // warm-up, deliberately not timed

    std::vector<double> samples;
    samples.reserve(static_cast<size_t>(reps));
    for (int r = 0; r < reps; ++r) {
        // DECISION 4: the array was built OUTSIDE this region.  We are timing
        // 2-SUM, not random number generation.
        const auto start = std::chrono::steady_clock::now();
        const bool found = fn(array, target);
        const auto end   = std::chrono::steady_clock::now();

        if (found) ++sink;
        samples.push_back(std::chrono::duration<double>(end - start).count());
    }
    std::sort(samples.begin(), samples.end());
    return samples[samples.size() / 2];
}

int main(int argc, char* argv[]) {
    const int max_exp = (argc > 1) ? std::atoi(argv[1]) : 15;

    if (max_exp < 10 || max_exp > 20) {
        std::cerr << "usage: " << argv[0] << " [max_exponent]\n"
                  << "max_exponent must be between 10 and 20"
                     " (20 will take a while -- that is the point)\n";
        return 1;
    }

    const int reps = 5;
    unsigned long long sink = 0;

    std::cout << reps
              << " timed runs per cell, median reported, times in milliseconds\n\n";
    std::cout << std::left
              << std::setw(9)  << "n"
              << std::right
              << std::setw(14) << "brute (ms)"
              << std::setw(9)  << "ratio"
              << std::setw(14) << "two-ptr (ms)"
              << std::setw(9)  << "ratio" << '\n';
    std::cout << std::string(55, '-') << '\n';

    double prev_brute = 0.0, prev_tp = 0.0;

    for (int e = 10; e <= max_exp; ++e) {
        const size_t n = static_cast<size_t>(1) << e;
        const std::vector<int> a = make_array(n, 212u);

        // Every value is in [-1000, 1000], so no pair can reach 99999.
        // Neither algorithm can exit early: this is the WORST case for both
        // of them, which is what makes the comparison fair.
        const int target = 99999;

        const double t_brute = median_seconds(two_sum_brute_force, a, target, reps, sink);
        const double t_tp    = median_seconds(two_sum_two_pointer, a, target, reps, sink);

        std::cout << std::left << std::setw(9) << n << std::right
                  << std::setw(14) << std::fixed << std::setprecision(4) << t_brute * 1000.0;
        if (prev_brute > 0.0)
            std::cout << std::setw(9) << std::setprecision(2) << t_brute / prev_brute;
        else
            std::cout << std::setw(9) << "-";
        std::cout << std::setw(14) << std::setprecision(4) << t_tp * 1000.0;
        if (prev_tp > 0.0)
            std::cout << std::setw(9) << std::setprecision(2) << t_tp / prev_tp;
        else
            std::cout << std::setw(9) << "-";
        std::cout << '\n';

        prev_brute = t_brute;
        prev_tp    = t_tp;
    }

    // sink is printed only so that the compiler cannot delete the work above.
    std::cout << "\n(checksum " << sink << " -- ignore the value, see DECISION 3)\n";
    return 0;
}
