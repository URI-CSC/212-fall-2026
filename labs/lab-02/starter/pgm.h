#ifndef PGM_H
#define PGM_H

#include <string>

#include "image.h"

// ---------------------------------------------------------------------------
// Reading, writing and looking at images.  All of this is written for you.
// You do not need to change anything in pgm.cpp, but Task 1 asks you to read
// the top of it.
// ---------------------------------------------------------------------------

// Reads an ASCII PGM (a "P2" file).  Returns a new Image, and YOU delete it.
// Throws std::runtime_error if the file will not open or is not a P2 file.
Image *read_pgm(const std::string &filename);

// Writes an ASCII PGM.  Throws std::runtime_error if it cannot write.
void write_pgm(const Image &img, const std::string &filename);

// Draws a 256 entry histogram as 16 buckets of bars.
void print_histogram(const int *counts);

#endif  // PGM_H
