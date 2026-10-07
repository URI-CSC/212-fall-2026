// ===========================================================================
// session.cpp -- replays a drawing session on your Canvas.  Written for you.
//
//     $ ./session script.txt > session.log
//
// Reads one command per line from the script:
//
//     paint <row> <col> <color>    paint one pixel
//     row <row> <col> <colors>     paint pixels left to right, starting at
//                                  (row, col), one paint per pixel.
//                                  A '-' skips a pixel.
//     undo [n]                     undo n times (default 1)
//     redo [n]                     redo n times (default 1)
//
// A color is one character: k black, w white, r red, o orange, y yellow,
// g green, b blue, p purple, n brown, '.' empty.
// Lines that are empty or start with # are skipped.
//
// After every command it writes the command, then calls print().  Open the
// log in viewer/index.html to watch the canvas and the two stacks.
// ===========================================================================

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "canvas.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: ./session script.txt > session.log\n";
        return 1;
    }
    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "cannot open " << argv[1] << '\n';
        return 1;
    }

    Canvas cv;
    std::cout << "> start\n";
    cv.print(std::cout);

    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::cout << "> " << line << '\n';

        std::istringstream words(line);
        std::string cmd;
        words >> cmd;
        try {
            if (cmd == "undo" || cmd == "redo") {
                int n = 1;
                words >> n;
                for (int i = 0; i < n; i++) {
                    bool ok = (cmd == "undo") ? cv.undo() : cv.redo();
                    if (!ok) {
                        std::cout << "! nothing to " << cmd << '\n';
                        break;
                    }
                }
            } else if (cmd == "paint") {
                int row = 0, col = 0;
                char color = '.';
                if (!(words >> row >> col >> color)) {
                    throw std::invalid_argument("expected: paint <row> <col> <color>");
                }
                cv.paint(row, col, color);
            } else if (cmd == "row") {
                int row = 0, col = 0;
                std::string colors;
                if (!(words >> row >> col >> colors)) {
                    throw std::invalid_argument("expected: row <row> <col> <colors>");
                }
                for (size_t i = 0; i < colors.size(); i++) {
                    if (colors[i] != '-') {
                        cv.paint(row, col + static_cast<int>(i), colors[i]);
                    }
                }
            } else {
                throw std::invalid_argument("unknown command '" + cmd + "'");
            }
        } catch (const std::exception& err) {
            std::cout << "! " << err.what() << '\n';
        }
        cv.print(std::cout);
    }
    return 0;
}
