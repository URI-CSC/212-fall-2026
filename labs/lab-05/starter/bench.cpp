// ===========================================================================
// bench.cpp -- times max_brute against max_deque on real data.  Written for
// you.
//
//     $ ./bench hourly.txt
//
// hourly.txt holds the temperature at Providence airport (T. F. Green), in
// degrees Fahrenheit, every hour from 2005 to 2024: 175,320 values.  Source:
// NOAA Integrated Surface Database.
//
// For windows of one day, one week, one month and one year, it runs both
// functions on all of the data, checks that they give the same answers, and
// prints how long each one took.
// ===========================================================================

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

#include "window.h"

// Calls f() once and returns how long it took, in milliseconds.
template <typename F>
static double time_ms(F f) {
    auto start = std::chrono::steady_clock::now();
    f();
    auto stop = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: ./bench hourly.txt\n";
        return 1;
    }
    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "cannot open " << argv[1] << '\n';
        return 1;
    }
    std::vector<int> a;
    int v;
    while (in >> v) {
        a.push_back(v);
    }
    std::cout << a.size() << " values\n\n";

    const int ks[] = {24, 168, 720, 8760};
    const char* names[] = {"1 day", "1 week", "1 month", "1 year"};

    std::printf("%6s  %-8s  %12s  %12s  %s\n", "k", "window", "brute (ms)",
                "deque (ms)", "same answers");
    for (int i = 0; i < 4; i++) {
        std::vector<int> slow, fast;
        double t_brute = time_ms([&] { slow = max_brute(a, ks[i]); });
        double t_deque = time_ms([&] { fast = max_deque(a, ks[i]); });
        std::printf("%6d  %-8s  %12.1f  %12.1f  %s\n", ks[i], names[i], t_brute,
                    t_deque, slow == fast ? "yes" : "NO");
        std::fflush(stdout);
    }
    return 0;
}
