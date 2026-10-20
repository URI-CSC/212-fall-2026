// ===========================================================================
// bench.cpp -- times merge() on your TokenList against a std::vector.
// Written for you.
//
//     $ ./bench shakespeare.txt
//
// shakespeare.txt holds about 40,000 lines from Shakespeare's plays: 1,115,394
// characters.  Source: the "tiny Shakespeare" data set (A. Karpathy), taken
// from Project Gutenberg.
//
// For the first n characters, with n doubling each row, it runs 10 rounds of
// BPE.  Each round finds the most frequent pair with your
// most_frequent_pair(), then merges it in your TokenList and in a TokenVector
// (vector_tokens.h).  Only the merges are timed.  At the end it checks that
// both hold the same tokens.
// ===========================================================================

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "tokens.h"
#include "vector_tokens.h"

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
        std::cerr << "usage: ./bench shakespeare.txt\n";
        return 1;
    }
    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "cannot open " << argv[1] << '\n';
        return 1;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    const std::string all = buffer.str();

    const int rounds = 10;
    std::printf("%10s  %10s  %12s  %12s  %s\n", "chars", "removed",
                "list (ms)", "vector (ms)", "same tokens");
    for (size_t n = 31250; n <= all.size(); n *= 2) {
        std::string text = all.substr(0, n);
        TokenList list(text);
        TokenVector vec(text);

        double t_list = 0, t_vec = 0;
        size_t removed = 0;
        for (int i = 0; i < rounds; i++) {
            Pair best = list.most_frequent_pair();
            if (best.count < 2) break;
            int new_token = 256 + i;
            t_list += time_ms([&] { removed += list.merge(best.left, best.right, new_token); });
            t_vec += time_ms([&] { vec.merge(best.left, best.right, new_token); });
        }
        std::printf("%10zu  %10zu  %12.1f  %12.1f  %s\n", n, removed, t_list,
                    t_vec, list.tokens() == vec.tokens() ? "yes" : "NO");
        std::fflush(stdout);
    }
    return 0;
}
