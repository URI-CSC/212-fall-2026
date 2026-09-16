#ifndef TWO_SUM_H
#define TWO_SUM_H

#include <vector>

// ---------------------------------------------------------------------------
// 2-SUM
//
// Both functions answer the same question:
//
//     Do there exist two DISTINCT positions i != j in `array`
//     such that array[i] + array[j] == target ?
//
// Notes on the contract (read these -- two of the tests depend on them):
//   * "distinct positions", not "distinct values".  {3, 3} with target 6
//     is TRUE (two different positions), but {3} with target 6 is FALSE
//     (you may not pair an element with itself).
//   * Neither function may modify the caller's array.  That is what `const`
//     buys you in the brute-force version, and it is why the two-pointer
//     version has to make a copy before sorting.
//   * Values are assumed to fit comfortably in `int`, so array[i] + array[j]
//     cannot overflow.  (Ask yourself what would happen if they didn't.)
// ---------------------------------------------------------------------------

bool two_sum_brute_force(const std::vector<int>& array, int target);
bool two_sum_two_pointer(const std::vector<int>& array, int target);

#endif  // TWO_SUM_H
