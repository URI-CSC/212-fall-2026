# Lab 05: Double ended queues

You have 20 years of hourly temperatures from Providence airport: 175,320
numbers. For every hour, you want the highest temperature in the 24 hours
before it. Or the last week, or the last year. This is the **sliding window
maximum**. The obvious way checks every window from scratch. Today you will
write it, time it, and then make it hundreds of times faster with one
**deque** and one extra loop. At the end you will watch the deque at work in
a viewer.

> [!CAUTION]
> Turn off AI autocomplete for this lab. How to switch it off,
> in VS Code and in other editors:
> [Turn off AI autocomplete](../lab-01/setup.md#turn-off-ai-autocomplete).

Work in `starter/`. It holds files you do not change: `check.h` (a small test
harness), `bench.cpp` and `hourly.txt` (Tasks 2 and 3), `session.cpp` and
`small.txt` (Task 4), and `answers.md`. You create three files there:
`window.h`, `window.cpp`, and `test_window.cpp`.

Every task ends the same way. Compile and run:

```bash
$ cd starter
$ g++ -std=c++17 -Wall -Wextra -g window.cpp test_window.cpp -o test_window
$ ./test_window
```

A failing check prints its line number. Do not start the next task until you
see `ALL TESTS PASSED`.

## std::deque in one minute

`std::deque<T>` from the STL is the deque shown in lecture: a queue you can
use from both ends.

| call | does | cost |
|---|---|---|
| `d.push_back(x)` | puts `x` at the back | $O(1)$ |
| `d.push_front(x)` | puts `x` at the front | $O(1)$ |
| `d.pop_back()` | removes the back element, returns nothing | $O(1)$ |
| `d.pop_front()` | removes the front element, returns nothing | $O(1)$ |
| `d.back()` | returns the back element | $O(1)$ |
| `d.front()` | returns the front element | $O(1)$ |
| `d.empty()` | `true` when there is nothing in it | $O(1)$ |
| `d.size()` | how many elements | $O(1)$ |

Unlike `std::stack`, you can also walk through every element, front to back:

```cpp
for (const Entry& e : d) {
    // e is each element in turn
}
```

> [!WARNING]
> `front()`, `back()`, `pop_front()` and `pop_back()` on an empty deque are
> **undefined behavior**: no error, just garbage or a crash. Check `empty()`
> first.

## Task 1: Brute force

A window is $k$ values in a row. Slide it one step at a time and write down
the largest value in each:

```text
index    0   1   2   3   4   5   6   7
value    1   3  -1  -3   5   3   6   7        k = 3

        [1   3  -1]                           max 3
            [3  -1  -3]                       max 3
                [-1 -3   5]                   max 5
                    [-3  5   3]               max 5
                         [5  3   6]           max 6
                             [3  6   7]       max 7
```

$n$ values give $n - k + 1$ windows. The brute force way scans each window
from its first value to its last.

### Declare

Create `window.h`:

```cpp
#ifndef WINDOW_H
#define WINDOW_H

#include <cstddef>
#include <vector>

// The maximum of every window of k consecutive values, the slow way.
std::vector<int> max_brute(const std::vector<int>& a, int k);

#endif
```

### Implement

Create `window.cpp`:

```cpp
#include "window.h"

#include <stdexcept>

std::vector<int> max_brute(const std::vector<int>& a, int k) {
    int n = static_cast<int>(a.size());
    // TODO: if k < 1 or k > n,
    //       throw std::invalid_argument("k must be between 1 and the number of values")
    std::vector<int> result;
    for (int start = 0; start + k <= n; start++) {
        // TODO: find the largest value of the current window, values can be negative
        // TODO: push it onto result
    }
    return result;
}
```

### Test

Create `test_window.cpp`:

```cpp
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "check.h"
#include "window.h"

static void test_brute() {
    section("task 1: brute force");
    std::vector<int> a = {1, 3, -1, -3, 5, 3, 6, 7};

    std::vector<int> want = {3, 3, 5, 5, 6, 7};
    CHECK(max_brute(a, 3) == want);
    CHECK(max_brute(a, 1) == a);                       // each value is its own window
    CHECK(max_brute(a, 8) == std::vector<int>{7});     // one window: the whole array

    CHECK_THROWS(max_brute(a, 0), std::invalid_argument);
    CHECK_THROWS(max_brute(a, 9), std::invalid_argument);
}

int main() {
    std::cout << "running tests\n";
    test_brute();
    return summary();
}
```

Compile, run, and answer **Q1** in `answers.md`.

## Task 2: A deque that holds the window

Now take the values one at a time, the way they arrive from a thermometer.
Keep the current window in a deque. Each new value goes on the **back**. The
oldest value leaves from the **front** once it is $k$ steps old.
That is a queue:

```text
                            front       back
k = 3       push  -3:       [ 1   3  -1]         // push -3 on the back
                            [ 1   3  -1  -3]     // 1 is too old:
                            [ 3  -1  -3]         // pop it from the front
```

To know when a value is too old, the deque stores an **entry**: the value and
its index. After the push of index `i`, the window holds the indexes
`i - k + 1` to `i`. An entry with a smaller index has left the window.

A `WindowMax` keeps the window. `push` adds one value. `max` returns the
largest value in the window, which for now means scanning the whole deque.
`max_deque` uses a `WindowMax` to give the same answers as `max_brute`.

### Declare

In `window.h`, add `#include <deque>` at the top, and add below `max_brute`:

```cpp
// The same answers, computed with a WindowMax.
std::vector<int> max_deque(const std::vector<int>& a, int k);

// One value pushed into the window: its position, and the value itself.
struct Entry {
    int index;
    int value;
};

class WindowMax {
    private:
        int m_k;                 // the window size
        int m_count;             // values pushed so far: the next index
        std::deque<Entry> m_dq;  // the values that matter, oldest at the front

    public:
        WindowMax(int k);

        void   push(int value);
        int    max() const;
        size_t size() const;
};
```

### Implement

Add to `window.cpp`:

```cpp
WindowMax::WindowMax(int k) {
    // TODO: if k < 1, throw std::invalid_argument("k must be at least 1")
    // TODO: initialize m_k to k and m_count to 0
}

void WindowMax::push(int value) {
    // TODO: push Entry{m_count, value} on the back, then add 1 to m_count
    // the window now holds the indexes m_count - m_k to m_count - 1
    // TODO: if the front entry's index is less than m_count - m_k, pop it
}

int WindowMax::max() const {
    // TODO: if the deque is empty, throw std::out_of_range("max() of an empty window")
    // TODO: return the largest value in the deque (walk it with a for loop)
}

size_t WindowMax::size() const {
    // TODO: return the number of entries in the deque
}

std::vector<int> max_deque(const std::vector<int>& a, int k) {
    int n = static_cast<int>(a.size());
    // TODO: same check on k as max_brute
    WindowMax w(k);
    std::vector<int> result;
    for (int i = 0; i < n; i++) {
        // TODO: push a[i] onto w
        // TODO: once the first window is full, push the max of the window onto result
    }
    return result;
}
```

### Test

Add this test, and call it from `main`:

```cpp
static void test_window() {
    section("task 2: a deque that holds the window");
    WindowMax w(3);
    CHECK_THROWS(w.max(), std::out_of_range);   // nothing pushed yet

    w.push(1);
    CHECK(w.max() == 1);
    w.push(3);
    CHECK(w.max() == 3);
    w.push(-1);
    CHECK(w.max() == 3);        // window 1 3 -1
    w.push(-3);
    CHECK(w.max() == 3);        // window 3 -1 -3
    w.push(5);
    CHECK(w.max() == 5);        // window -1 -3 5
    CHECK(w.size() <= 3);       // never more than the window

    std::vector<int> a = {1, 3, -1, -3, 5, 3, 6, 7};
    CHECK(max_deque(a, 3) == max_brute(a, 3));

    std::vector<int> b;
    for (int i = 0; i < 500; i++) {
        b.push_back((i * 37) % 101 - 50);   // jumps up and down
    }
    for (int k = 1; k <= 60; k++) {
        CHECK(max_deque(b, k) == max_brute(b, k));
    }
    CHECK_THROWS(max_deque(a, 0), std::invalid_argument);
}
```

Compile and run until everything passes. Then time both functions on the real
data. `bench.cpp` runs them with windows of one day, one week, one month and
one year:

```bash
$ g++ -std=c++17 -Wall -Wextra -g window.cpp bench.cpp -o bench
$ ./bench hourly.txt
```

It takes about 15 seconds. Answer **Q2**.

## Task 3: The staircase

Look at the window as bars. Suppose 52 and 53 are in the window, and 57
arrives:

```text
                  57
             53   ##        52 and 53 are older than 57: they leave the
        52   ##   ##        window first. Until then, 57 is in the window
        ##   ##   ##        too, and beats them. 52 and 53 can never be
        ##   ##   ##        the max again.
```

So throw them away. Before you push a new value, **pop from the back** every
entry whose value is less than or equal to it. What is left in the deque
always goes down from front to back, like a staircase:

```text
front                    back
  58
  ##   56
  ##   ##   54
  ##   ##   ##   50
  ##   ##   ##   ##   45
```

The largest value is now always at the **front**. `max()` does not need to
search anymore.

### Implement

- In `push`, before the `push_back`, add:

```cpp
    // TODO: while the deque is not empty and its back value is <= value,
    //       pop from the back
```

- In `max`, keep the empty check, and replace the loop with one line: return
  the front entry's value.

Nothing changes in `window.h`. Your Task 2 tests must still pass: the answers
are the same, only faster.

### Test

Add this test, and call it from `main`:

```cpp
static void test_staircase() {
    section("task 3: the staircase");
    WindowMax up(3);
    for (int v = 1; v <= 5; v++) {
        up.push(v);             // each new value beats everything before it
    }
    CHECK(up.size() == 1);
    CHECK(up.max() == 5);

    WindowMax down(3);
    for (int v = 5; v >= 1; v--) {
        down.push(v);           // each old value is still a candidate
    }
    CHECK(down.size() == 3);
    CHECK(down.max() == 3);     // 5 and 4 have left the window

    WindowMax ties(4);
    ties.push(7);
    ties.push(7);
    ties.push(7);
    CHECK(ties.size() == 1);    // a newer equal value replaces the older one
    CHECK(ties.max() == 7);
}
```

Fill in the table in **Q3** on paper **first**. Then compile and run. Run the
benchmark again:

```bash
$ g++ -std=c++17 -Wall -Wextra -g window.cpp bench.cpp -o bench
$ ./bench hourly.txt
```

Finish **Q3** and answer **Q4**.

## Task 4: Print and watch

`print` writes the deque, front first, in this exact format (the viewer reads
it):

```text
deque 3
    2 6
    3 4
    5 3
```

The first line is `deque` and the number of entries. Each entry line starts
with 4 spaces, then `index value`, separated by one space.

### Declare

Add `#include <ostream>` at the top of `window.h`, and add to the `public:`
section:

```cpp
        void print(std::ostream& out) const;
```

### Implement

```cpp
void WindowMax::print(std::ostream& out) const {
    // TODO: print "deque", a space, the size, and '\n'
    // TODO: for each entry, front to back: 4 spaces, index, a space, value, '\n'
}
```

### Test

Add this test, and call it from `main`:

```cpp
static void test_print() {
    section("task 4: print");
    WindowMax w(4);
    w.push(9);
    w.push(2);
    w.push(6);
    w.push(4);

    std::ostringstream out;
    w.print(out);
    CHECK(out.str() == "deque 3\n"
                       "    0 9\n"
                       "    2 6\n"
                       "    3 4\n");

    w.push(1);                  // index 0 leaves the window
    std::ostringstream out2;
    w.print(out2);
    CHECK(out2.str() == "deque 3\n"
                        "    2 6\n"
                        "    3 4\n"
                        "    4 1\n");
}
```

`std::ostringstream` is an output stream that writes into a string instead of
the screen. `out.str()` gives you that string.

Compile and run until you see `ALL TESTS PASSED`.

### Watch it

`small.txt` holds 40 hours of real temperatures: January 6 and 7, 2014. It
warms up, a cold front comes through, and it warms up again. `session.cpp`
pushes them into a `WindowMax` with the $k$ you give it, and calls `print`
after each push. Build it and save its output, with a window of 6 hours:

```bash
$ g++ -std=c++17 -Wall -Wextra -g window.cpp session.cpp -o session
$ ./session small.txt 6 > session.log
```

Open `viewer/index.html` in a browser and drop `session.log` on it. Step
through with the arrow keys. Watch the dashed staircase, and watch values
leave from the back (red) and from the front (orange). Answer **Q5**.

> [!TIP]
> Try other windows: `./session small.txt 12 > session.log`. Or write your own
> list of numbers in a file and watch that.

### Your tests

Add a function `test_your_cases` with **at least two** checks of your own, and
call it from `main`. At least one must be a case you got wrong, or expected to.

## Submission

Upload these files to Gradescope:

- `window.h`
- `window.cpp`
- `test_window.cpp`
- `answers.md`

> [!WARNING]
> **Upload those four files and nothing else.** The autograder checks this
> before it looks at your code.

Your code should compile clean under `-Wall -Wextra -Werror`.

Questions: ask the instructor or a TA **in the room**, or post on Ed.
