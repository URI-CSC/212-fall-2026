// ===========================================================================
// detect.cpp -- runs your count_failures and suspects on a log file.
// Written for you.
//
//     $ g++ -std=c++17 -Wall -Wextra -g security.cpp detect.cpp -o detect
//     $ ./detect auth.log 5
//
// The first argument is the log file. The second is the limit: an IP address
// with at least that many failed logins is a suspect.
// ===========================================================================

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include "security.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] << " <log file> <limit>\n";
        return 1;
    }
    std::ifstream log(argv[1]);
    if (!log) {
        std::cerr << "cannot open " << argv[1] << '\n';
        return 1;
    }
    int limit = std::atoi(argv[2]);

    std::map<std::string, int> failures = count_failures(log);
    std::cout << "failed logins per IP address:\n";
    for (const auto& entry : failures) {
        std::cout << "  " << entry.first << "  " << entry.second << '\n';
    }

    std::set<std::string> found = suspects(failures, limit);
    std::cout << "\nsuspects (at least " << limit << " failures):\n";
    for (const std::string& ip : found) {
        std::cout << "  " << ip << '\n';
    }
    return 0;
}
