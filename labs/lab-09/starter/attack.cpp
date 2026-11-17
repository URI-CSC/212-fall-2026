// ===========================================================================
// attack.cpp -- attacks your StrSet with the order of its input.
// Written for you.
//
//     $ g++ -std=c++17 -O2 security.cpp attack.cpp -o attack
//     $ ./attack
//
// For each n, with n doubling each row, it makes n user names, "user000001",
// "user000002", ..., and inserts them three times:
//
//     random   into your StrSet, shuffled (a normal day)
//     sorted   into your StrSet, in alphabetical order (the attacker)
//     std::set into a std::set, in alphabetical order (the same attack)
//
// It prints the height of your tree and the time of each run.
// ===========================================================================

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "security.h"

// Calls f() once and returns how long it took, in milliseconds.
template <typename F>
static double time_ms(F f) {
    auto start = std::chrono::steady_clock::now();
    f();
    auto stop = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

// "user" followed by i, padded to 6 digits: 42 gives "user000042".
static std::string user_name(int i) {
    char buf[16];
    std::snprintf(buf, sizeof buf, "user%06d", i);
    return buf;
}

int main() {
    std::printf("%8s | %8s %10s | %8s %10s | %14s\n",
                "n", "height", "random ms", "height", "sorted ms", "std::set ms");
    std::mt19937 rng(212);
    for (int n = 2500; n <= 20000; n *= 2) {
        std::vector<std::string> sorted;
        for (int i = 1; i <= n; ++i) {
            sorted.push_back(user_name(i));
        }
        std::vector<std::string> shuffled = sorted;
        std::shuffle(shuffled.begin(), shuffled.end(), rng);

        StrSet a;
        double t_random = time_ms([&] {
            for (const std::string& name : shuffled) {
                a.insert(name);
            }
        });
        StrSet b;
        double t_sorted = time_ms([&] {
            for (const std::string& name : sorted) {
                b.insert(name);
            }
        });
        std::set<std::string> c;
        double t_std = time_ms([&] {
            for (const std::string& name : sorted) {
                c.insert(name);
            }
        });

        if (a.size() != static_cast<std::size_t>(n) || b.size() != static_cast<std::size_t>(n)) {
            std::printf("n = %d: your StrSet has the wrong size\n", n);
            return 1;
        }
        std::printf("%8d | %8d %10.2f | %8d %10.2f | %14.2f\n",
                    n, a.height(), t_random, b.height(), t_sorted, t_std);
    }
    return 0;
}
