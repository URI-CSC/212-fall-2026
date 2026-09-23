// ===========================================================================
// leaky.cpp -- somebody else's code.  It runs, it prints the right answers,
// and it leaks.
//
//     $ g++ -std=c++17 -Wall -Wextra -Werror leaky.cpp -o leaky
//     $ ./leaky
//
// There are THREE missing or misplaced lines.  Find them, fix them, and get
// the last line of the output down to 0.  Do not change main.
// ===========================================================================

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <new>

// the same array counter you have in test_image.cpp
static long long arrays_live = 0;

void *operator new[](std::size_t n) {
    void *p = std::malloc(n == 0 ? 1 : n);
    if (p == nullptr) {
        throw std::bad_alloc();
    }
    ++arrays_live;
    return p;
}

void operator delete[](void *p) noexcept {
    if (p != nullptr) {
        --arrays_live;
        std::free(p);
    }
}

void operator delete[](void *p, std::size_t) noexcept { operator delete[](p); }

// ---------------------------------------------------------------------------

class Canvas {
    private:
        int *pixels;
        int count;

    public:
        Canvas(int n) {
            count = n;
            pixels = new int[count];
            for (int i = 0; i < count; i++) {
                pixels[i] = 0;
            }
        }

        ~Canvas() {
            std::cout << "  a canvas of " << count << " pixels goes away\n";
        }

        void set(int i, int value) { pixels[i] = value; }

        // Make the canvas bigger, keeping what was already in it.
        void resize(int n) {
            int *bigger = new int[n];
            for (int i = 0; i < n; i++) {
                bigger[i] = (i < count) ? pixels[i] : 0;
            }
            pixels = bigger;
            count = n;
        }

        // Does any pixel value appear more than `limit` times?
        bool has_a_popular_value(int limit) const {
            int *counts = new int[256];
            for (int v = 0; v < 256; v++) {
                counts[v] = 0;
            }
            for (int i = 0; i < count; i++) {
                counts[pixels[i]]++;
            }
            for (int v = 0; v < 256; v++) {
                if (counts[v] > limit) {
                    return true;
                }
            }
            delete[] counts;
            return false;
        }
};

int main() {
    std::cout << "start: " << arrays_live << " arrays alive\n\n";
    {
        Canvas small(10);
        small.set(0, 42);
        small.resize(20);
        std::cout << "  popular value? " << small.has_a_popular_value(5) << '\n';

        Canvas big(1000);
        std::cout << "  popular value? " << big.has_a_popular_value(5) << '\n';
    }
    std::cout << "\nend: " << arrays_live << " arrays alive\n";
    return 0;
}
