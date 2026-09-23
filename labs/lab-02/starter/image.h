#ifndef IMAGE_H
#define IMAGE_H

// ---------------------------------------------------------------------------
// Image -- a grayscale picture.
//
// Every pixel is an int in 0..255.  0 is black, 255 is white.
//
// The pixels live in ONE array, stored row by row.  The pixel at row r and
// column c sits at index
//
//     r * cols + c
//
// There is no second dimension anywhere in memory.  There is one run of
// numbers and one multiplication.  That is the whole idea of this lab.
//
// The contract (two of the tests depend on it):
//   * rows and cols are known only when the program runs, so `data` must be
//     allocated with new[] and freed with delete[].
//   * at() and set() throw std::out_of_range if r or c is outside the image.
//   * No std::vector, and no other standard library container.  A container
//     would write the destructor for you, and the destructor is the point.
// ---------------------------------------------------------------------------

class Image {
    private:
        int rows;
        int cols;
        int *data;  // rows * cols pixels, row by row

    public:
        // Allocates rows * cols pixels and sets every one of them to 0.
        Image(int n_rows, int n_cols);

        // Frees everything the constructor allocated.
        ~Image();

        int get_rows() const;
        int get_cols() const;

        // The pixel at row r, column c.  Both throw std::out_of_range.
        int at(int r, int c) const;
        void set(int r, int c, int value);

        // Task 3.
        void invert();
        void flip_horizontal();
        void transpose();

        // Task 4.  `counts` points at 256 ints that the CALLER owns.
        void histogram(int *counts) const;
        int most_common_value() const;
        void auto_contrast();
};

#endif  // IMAGE_H
