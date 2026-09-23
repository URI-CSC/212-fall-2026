// ===========================================================================
// apply.cpp -- runs one operation on one image.  Written for you.
//
//     $ g++ -std=c++17 -Wall -Wextra -Werror image.cpp pgm.cpp apply.cpp -o apply
//     $ ./apply banner.pgm invert inverted.pgm
//     $ ./apply banner.pgm hist
//
// Operations: invert, flip, transpose, contrast, hist
// Everything but `hist` writes a new .pgm for you to open in the viewer.
// ===========================================================================

#include <iostream>
#include <stdexcept>
#include <string>

#include "image.h"
#include "pgm.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "usage: " << argv[0]
                  << " <in.pgm> <invert|flip|transpose|contrast> <out.pgm>\n"
                  << "       " << argv[0] << " <in.pgm> hist\n";
        return 1;
    }
    const std::string filename = argv[1];
    const std::string op = argv[2];

    Image *img = nullptr;
    try {
        img = read_pgm(filename);
    } catch (const std::runtime_error &e) {
        std::cout << "error: " << e.what() << '\n';
        return 1;
    }
    std::cout << filename << ": " << img->get_rows() << " rows by "
              << img->get_cols() << " columns, "
              << img->get_rows() * img->get_cols() << " pixels\n";

    if (op == "hist") {
        int *counts = new int[256];
        img->histogram(counts);
        print_histogram(counts);
        delete[] counts;
        std::cout << "most common value: " << img->most_common_value() << '\n';
        delete img;
        return 0;
    }

    if (argc < 4) {
        std::cout << "error: " << op << " needs an output file, "
                  << "for example: " << argv[0] << " " << filename << " " << op
                  << " out.pgm\n";
        delete img;
        return 1;
    }

    if (op == "invert") {
        img->invert();
    } else if (op == "flip") {
        img->flip_horizontal();
    } else if (op == "transpose") {
        img->transpose();
    } else if (op == "contrast") {
        img->auto_contrast();
    } else {
        std::cout << "unknown operation: " << op << '\n';
        delete img;
        return 1;
    }

    try {
        write_pgm(*img, argv[3]);
    } catch (const std::runtime_error &e) {
        std::cout << "error: " << e.what() << '\n';
        delete img;
        return 1;
    }
    std::cout << "wrote " << argv[3] << ": " << img->get_rows() << " rows by "
              << img->get_cols() << " columns.  Open it in the viewer.\n";
    delete img;
    return 0;
}
