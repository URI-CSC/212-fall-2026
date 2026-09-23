// ===========================================================================
// pgm.cpp -- reading a PGM file, writing one, and drawing a histogram.
//
// Written for you.  Read the first function if you want to know exactly what
// a PGM file is; the rest is printing.
// ===========================================================================

#include "pgm.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

// A PGM file is text.  Tokens are separated by any amount of whitespace, and
// a '#' starts a comment that runs to the end of the line.  This reads one
// token and skips over any comments in the way.
static std::string next_token(std::ifstream &in) {
    std::string token;
    while (in >> token) {
        if (token[0] != '#') {
            return token;
        }
        std::string rest;
        std::getline(in, rest);  // throw away the rest of the comment line
    }
    throw std::runtime_error("PGM file ended too early");
}

Image *read_pgm(const std::string &filename) {
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("cannot open " + filename);
    }

    // header: the magic number P2, then width, height, and the largest value
    if (next_token(in) != "P2") {
        throw std::runtime_error(filename + " is not an ASCII PGM (P2) file");
    }
    const int cols = std::stoi(next_token(in));
    const int rows = std::stoi(next_token(in));
    const int maxval = std::stoi(next_token(in));
    if (cols <= 0 || rows <= 0 || maxval <= 0) {
        throw std::runtime_error(filename + " has a bad header");
    }

    // then rows * cols pixel values, and nothing else
    Image *img = new Image(rows, cols);
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int value = std::stoi(next_token(in));
            if (maxval != 255) {
                value = value * 255 / maxval;
            }
            img->set(r, c, value);
        }
    }
    return img;
}

void write_pgm(const Image &img, const std::string &filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("cannot write " + filename);
    }
    out << "P2\n" << img.get_cols() << " " << img.get_rows() << "\n255\n";
    for (int r = 0; r < img.get_rows(); r++) {
        for (int c = 0; c < img.get_cols(); c++) {
            out << img.at(r, c) << (c + 1 == img.get_cols() ? '\n' : ' ');
        }
    }
}

void print_histogram(const int *counts) {
    // 256 levels is too many bars to read, so group them into 16 buckets
    int buckets[16] = {0};
    int tallest = 1;
    for (int v = 0; v < 256; v++) {
        buckets[v / 16] += counts[v];
    }
    for (int b = 0; b < 16; b++) {
        if (buckets[b] > tallest) {
            tallest = buckets[b];
        }
    }
    for (int b = 0; b < 16; b++) {
        std::string label =
            std::to_string(b * 16) + "-" + std::to_string(b * 16 + 15);
        while (label.size() < 7) {
            label = " " + label;
        }
        std::cout << label << " |";
        const int bar = buckets[b] * 50 / tallest;
        for (int i = 0; i < bar; i++) {
            std::cout << '#';
        }
        std::cout << " " << buckets[b] << '\n';
    }
}
