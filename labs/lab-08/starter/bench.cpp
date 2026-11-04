// ===========================================================================
// bench.cpp -- times your PQ<int> against std::priority_queue<int>.
// Written for you.
//
//     $ g++ -std=c++17 -O2 bench.cpp -o bench
//     $ ./bench
//
// For each n, with n doubling each row, it pushes the same n random ints into
// both queues, then pops them all.  Pushes and pops are timed together.  It
// also checks that both queues popped the same numbers in the same order.
// ===========================================================================

#include <chrono>
#include <cstdio>
#include <queue>
#include <random>
#include <vector>

#include "pq.h"

// Calls f() once and returns how long it took, in milliseconds.
template <typename F>
static double time_ms(F f) {
    auto start = std::chrono::steady_clock::now();
    f();
    auto stop = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

// Pushes every value into q, then pops them all.  Works for both queues,
// because both have push, top, pop and empty.  Returns the popped values.
template <typename Queue>
static std::vector<int> push_then_pop(Queue& q, const std::vector<int>& values) {
    for (int x : values) {
        q.push(x);
    }
    std::vector<int> out;
    out.reserve(values.size());
    while (!q.empty()) {
        out.push_back(q.top());
        q.pop();
    }
    return out;
}

int main() {
    std::printf("%8s %12s %8s %14s %8s\n", "n", "PQ (ms)", "ratio", "std (ms)", "ratio");
    double prev_mine = 0, prev_std = 0;
    std::mt19937 rng(212);
    for (int n = 10000; n <= 80000; n *= 2) {
        std::vector<int> values(n);
        for (int& x : values) {
            x = static_cast<int>(rng() % 1000000);
        }
        std::vector<int> a, b;
        PQ<int> mine;
        std::priority_queue<int> theirs;
        double t_mine = time_ms([&] { a = push_then_pop(mine, values); });
        double t_std = time_ms([&] { b = push_then_pop(theirs, values); });
        if (a != b) {
            std::printf("n = %d: your PQ popped a different order than std::priority_queue\n", n);
            return 1;
        }
        if (prev_mine > 0) {
            std::printf("%8d %12.1f %8.1f %14.2f %8.1f\n", n, t_mine, t_mine / prev_mine, t_std, t_std / prev_std);
        } else {
            std::printf("%8d %12.1f %8s %14.2f %8s\n", n, t_mine, "", t_std, "");
        }
        prev_mine = t_mine;
        prev_std = t_std;
    }
    return 0;
}
