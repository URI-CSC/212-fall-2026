#include "two_sum.h"  // includes the function headers
#include <algorithm>  // needed by std::sort

// ---------------------------------------------------------------------------
// TASK 2a -- brute force
//
// Check every unordered pair of positions.  Return as soon as one of them
// sums to `target`; return false if none does.
//
// Think before you type: how many pairs are there in an array of n elements?
// Write that number down -- you will compare it to your measurements later.
// ---------------------------------------------------------------------------
bool two_sum_brute_force(const std::vector<int>& array, int target) {
    // TODO: your code here.

    (void) array;   // <-- delete this line once you use `array`
    (void) target;  // <-- delete this line once you use `target`
    return false;
}

// ---------------------------------------------------------------------------
// TASK 2b -- sort, then two pointers
//
// After sorting, walk one index in from each end:
//   sum == target  ->  found it
//   sum <  target  ->  the smallest element left is too small, move `left` up
//   sum >  target  ->  the largest element left is too big, move `right` down
//
// Trace this on paper with copy = {-5, -2, 0, 3, 8} and target = 1 BEFORE
// you write any code.  It takes two minutes and saves twenty.
// ---------------------------------------------------------------------------
bool two_sum_two_pointer(const std::vector<int>& array, int target) {
    const size_t n = array.size();

    // fewer than 2 elements -> there is no pair at all
    if (n < 2) {
        return false;
    }

    // copy, because we promised not to modify the caller's array
    std::vector<int> copy = array;
    std::sort(copy.begin(), copy.end());

    // TODO: your code here.

    (void) target;  // <-- delete this line once you use `target`
    return false;
}
