// ===========================================================================
// bpe.cpp -- trains BPE on a text file with your code.  Written for you.
//
//     $ ./bpe tiny.txt 30 > bpe.log
//
// Then drop bpe.log on viewer/index.html to watch every merge.
//
// It builds a TokenList from the file and calls your train() for up to the
// given number of merges.  Then it replays the rules train() learned, one at
// a time, on a new TokenList, with your merge(), and prints the list after
// each one.  It ends with the round trip: decoding the final tokens with your
// decode() must give back the file, exactly.
//
// The output, one line each:
//
//     start <n> <token> <token> ...           the list before any merge
//     rule <id> <left> <right> "<text>"       one line per rule learned
//     merge <id> removed <r> size <size()>    one line per replayed merge
//     tokens <token> <token> ...              the list after that merge
//     <merges> merges: <chars> characters -> <tokens> tokens
//     round trip: OK
// ===========================================================================

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "tokens.h"

// The text with newlines written as \n, between quotes.
static std::string quoted(const std::string& s) {
    std::string out = "\"";
    for (char c : s) {
        out += (c == '\n') ? std::string("\\n") : std::string(1, c);
    }
    return out + "\"";
}

static void print_tokens(const char* label, const TokenList& list) {
    std::cout << label;
    for (int t : list.tokens()) {
        std::cout << ' ' << t;
    }
    std::cout << '\n';
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "usage: ./bpe tiny.txt <merges> > bpe.log\n";
        return 1;
    }
    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "cannot open " << argv[1] << '\n';
        return 1;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    const std::string text = buffer.str();
    const int merges = std::stoi(argv[2]);
    if (text.size() > 5000) {
        std::cerr << text.size() << " characters: too many to watch. Use a "
                     "file with 5,000 or fewer.\n";
        return 1;
    }

    try {
        TokenList list(text);
        std::cout << "start " << list.size();
        print_tokens("", list);

        std::vector<Rule> rules = train(list, merges);
        for (size_t i = 0; i < rules.size(); i++) {
            int token = 256 + static_cast<int>(i);
            std::cout << "rule " << token << ' ' << rules[i].left << ' '
                      << rules[i].right << ' '
                      << quoted(decode_token(token, rules)) << '\n';
        }

        TokenList replay(text);
        for (size_t i = 0; i < rules.size(); i++) {
            int token = 256 + static_cast<int>(i);
            size_t removed = replay.merge(rules[i].left, rules[i].right, token);
            std::cout << "merge " << token << " removed " << removed
                      << " size " << replay.size() << '\n';
            print_tokens("tokens", replay);
        }

        std::cout << rules.size() << " merges: " << text.size()
                  << " characters -> " << list.size() << " tokens\n";
        bool same = decode(list.tokens(), rules) == text;
        std::cout << "round trip: " << (same ? "OK" : "WRONG, decode does not "
                                                      "give back the text")
                  << '\n';
        return same ? 0 : 1;
    } catch (const std::exception& err) {
        std::cout << "error: " << err.what() << '\n';
        std::cerr << "error: " << err.what() << '\n';
        return 1;
    }
}
