#include "image.h"

#include <stdexcept>

// ---------------------------------------------------------------------------
// TASK 2a -- the constructor
//
// Store the two sizes, allocate rows * cols ints with new[], and set every
// one of them to 0.
// ---------------------------------------------------------------------------
Image::Image(int n_rows, int n_cols) {
    // TODO: your code here.

    rows = 0;
    cols = 0;
    data = nullptr;  // <-- replace these three lines
    (void) n_rows;
    (void) n_cols;
}

// ---------------------------------------------------------------------------
// TASK 2b -- the destructor
//
// One line.  It is the most important line in the file.
// ---------------------------------------------------------------------------
Image::~Image() {
    // TODO: your code here.
}

int Image::get_rows() const {
    // TODO: your code here.
    return 0;
}

int Image::get_cols() const {
    // TODO: your code here.
    return 0;
}

// ---------------------------------------------------------------------------
// TASK 2c -- at() and set()
//
// Both of them turn (r, c) into one index.  Both of them throw
// std::out_of_range if r or c is outside the image.  Negative counts as
// outside.
//
//     throw std::out_of_range("Image::at: pixel is outside the image");
// ---------------------------------------------------------------------------
int Image::at(int r, int c) const {
    // TODO: your code here.
    (void) r;
    (void) c;
    return 0;
}

void Image::set(int r, int c, int value) {
    // TODO: your code here.
    (void) r;
    (void) c;
    (void) value;
}

// ---------------------------------------------------------------------------
// TASK 3a -- invert
//
// Every pixel becomes 255 minus itself.  Black turns white.
// ---------------------------------------------------------------------------
void Image::invert() {
    // TODO: your code here.
}

// ---------------------------------------------------------------------------
// TASK 3b -- flip_horizontal
//
// Mirror the picture left to right.  Column c trades places with column
// cols - 1 - c.
//
// Careful: walk only HALF of each row.  Walking the whole row swaps every
// pair twice and puts the image back exactly as it was.
// ---------------------------------------------------------------------------
void Image::flip_horizontal() {
    // TODO: your code here.
}

// ---------------------------------------------------------------------------
// TASK 3c -- transpose
//
// Row r becomes column r.  A 160 by 240 image becomes a 240 by 160 image, so
// this one cannot be done by swapping pixels in place.
//
// The recipe, and you will use it again in Assignment 1:
//   1. allocate a NEW array of rows * cols ints
//   2. copy every pixel into its new home: the pixel at (r, c) goes to (c, r)
//   3. delete[] the OLD array
//   4. point `data` at the new one and swap `rows` with `cols`
//
// Step 3 is the one people forget.  The test at the bottom of test_image.cpp
// counts arrays, and it will notice.
// ---------------------------------------------------------------------------
void Image::transpose() {
    // TODO: your code here.
}

// ---------------------------------------------------------------------------
// TASK 4a -- histogram
//
// `counts` points at 256 ints that the caller already allocated.  Set
// counts[v] to the number of pixels whose value is v.
//
// Zero the 256 entries first.  The caller's array holds whatever was in that
// memory before.
//
// Notice what this loop does NOT do: it never searches, and it never
// compares two pixels.  The pixel value IS the index.
// ---------------------------------------------------------------------------
void Image::histogram(int *counts) const {
    // TODO: your code here.
    (void) counts;
}

// ---------------------------------------------------------------------------
// TASK 4b -- most_common_value
//
// The value that appears most often.  On a tie, return the smaller value.
//
// Allocate your own array of 256 ints, call histogram(), find the largest
// count, and delete[] the array before you return.  On every path out.
// ---------------------------------------------------------------------------
int Image::most_common_value() const {
    // TODO: your code here.
    return 0;
}

// ---------------------------------------------------------------------------
// TASK 4c -- auto_contrast
//
// Find the darkest value `lo` and the brightest value `hi` that actually
// appear in the image, then stretch that range to fill 0..255:
//
//     new_value = (old_value - lo) * 255 / (hi - lo)
//
// If hi == lo the image is one flat color.  Leave it alone; do not divide by
// zero.
// ---------------------------------------------------------------------------
void Image::auto_contrast() {
    // TODO: your code here.
}
