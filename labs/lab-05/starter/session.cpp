// ===========================================================================
// session.cpp -- pushes a list of values into your WindowMax, one at a time.
// Written for you.
//
//     $ ./session small.txt 5 > session.log
//
// Reads whole numbers from the file (one per line, or separated by spaces),
// then for each one: writes "> push <index> <value>", pushes it, calls
// print(), and writes "max <value>".
//
// Open the log in viewer/index.html to watch the window and the deque.
// ===========================================================================

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "window.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "usage: ./session small.txt <k> > session.log\n";
        return 1;
    }
    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "cannot open " << argv[1] << '\n';
        return 1;
    }
    int k = std::stoi(argv[2]);

    std::vector<int> values;
    int v;
    while (in >> v) {
        values.push_back(v);
    }
    if (values.size() > 500) {
        std::cerr << values.size() << " values: too many to watch. Use a file "
                     "with 500 or fewer.\n";
        return 1;
    }

    std::cout << "k " << k << '\n';
    std::cout << "data";
    for (int x : values) {
        std::cout << ' ' << x;
    }
    std::cout << '\n';

    try {
        WindowMax w(k);
        for (size_t i = 0; i < values.size(); i++) {
            std::cout << "> push " << i << ' ' << values[i] << '\n';
            w.push(values[i]);
            w.print(std::cout);
            std::cout << "max " << w.max() << '\n';
        }
    } catch (const std::exception& err) {
        std::cout << "! " << err.what() << '\n';
        std::cerr << "error: " << err.what() << '\n';
        return 1;
    }
    return 0;
}
