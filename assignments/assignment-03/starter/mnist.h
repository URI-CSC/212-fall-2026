// ===========================================================================
// mnist.h -- reads the MNIST files.  Written for you.  Do not change it.
//
// MNIST is 70,000 handwritten digits, 0 to 9, collected from American census
// workers and high school students: 60,000 to learn from, and 10,000 to test
// on.  Each digit is a 28 x 28 grayscale image, centered in its box.
//
// A pixel is one byte: 0 is the white background, 255 is full black ink, and
// values in between are the soft edges of a pen stroke.  Pixels are stored
// row by row, so pixel (row, col) is at index row * SIDE + col.
//
//     MnistReader reader("train-images.idx", "train-labels.idx");
//     unsigned char pixels[PIXELS];
//     int label;
//     while (reader.next(pixels, label)) { ... }      // 60,000 times
//
// The data is by Yann LeCun, Corinna Cortes and Christopher Burges, and is
// shared under the Creative Commons Attribution-Share Alike 3.0 license.
// ===========================================================================
#ifndef MNIST_H
#define MNIST_H

#include <fstream>
#include <stdexcept>
#include <string>

const int SIDE = 28; // an image is SIDE x SIDE pixels
const int PIXELS = SIDE * SIDE; // 784

class MnistReader {
    private:
        std::ifstream m_images;
        std::ifstream m_labels;
        int m_count;
        int m_read;

        static int read_int(std::ifstream& in) {
            unsigned char b[4];
            in.read(reinterpret_cast<char*>(b), 4);
            return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
        }

    public:
        // Opens both files.  Throws std::runtime_error if either cannot be
        // opened, or they do not hold the same number of digits.
        MnistReader(const std::string& images_file, const std::string& labels_file)
            : m_images(images_file, std::ios::binary),
              m_labels(labels_file, std::ios::binary), m_count(0), m_read(0) {
            if (!m_images || !m_labels) {
                throw std::runtime_error("cannot open " + images_file + " or " + labels_file);
            }
            int magic_images = read_int(m_images);
            int count = read_int(m_images);
            int rows = read_int(m_images);
            int cols = read_int(m_images);
            int magic_labels = read_int(m_labels);
            int labels = read_int(m_labels);
            if (magic_images != 2051 || magic_labels != 2049 || rows != SIDE
                || cols != SIDE || count != labels) {
                throw std::runtime_error("not MNIST files: " + images_file + ", " + labels_file);
            }
            m_count = count;
        }

        // How many digits the files hold.
        int count() const { return m_count; }

        // Puts the next digit's PIXELS pixels in `pixels` and its value in
        // `label`, and returns true.  Returns false when there are none left.
        bool next(unsigned char pixels[], int& label) {
            if (m_read >= m_count) return false;
            char value = 0;
            m_images.read(reinterpret_cast<char*>(pixels), PIXELS);
            m_labels.read(&value, 1);
            if (!m_images || !m_labels) return false;
            label = static_cast<unsigned char>(value);
            m_read++;
            return true;
        }
};

#endif
