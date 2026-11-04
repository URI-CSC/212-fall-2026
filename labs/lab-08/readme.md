# Lab 08: Templates, and your own priority queue

You have already used `std::vector<int>`. The types in
angle brackets are **template arguments**. Today you find out what they do by
building your own priority queue, one step at a time: first for `int` only,
then for any type, and then with any rule for "highest priority". At the end
you race it against `std::priority_queue`.

> [!CAUTION]
> Turn off AI autocomplete for this lab. How to switch it off,
> in VS Code and in other editors:
> [Turn off AI autocomplete](../lab-01/setup.md#turn-off-ai-autocomplete).

**What you write.** You work in `starter/` and write two files there: `pq.h`
and `test_pq.cpp`. All of your priority queue goes in the header, `pq.h`.
There is no `pq.cpp` today; Task 3 explains why. Do not change the other
files. They are tools for you:

- `check.h`: a small test harness
- `bench.cpp`: times your priority queue against `std::priority_queue` (Task 5)
- `answers.md`: the questions of this lab

**How you work.** Each task adds a little to `pq.h` and a test to
`test_pq.cpp`. Every task ends the same way. Compile and run the tests:

```bash
$ cd starter
$ g++ -std=c++17 -Wall -Wextra -g test_pq.cpp -o test_pq
$ ./test_pq
```

A failing check prints its line number. Do not start the next task until you
see `ALL TESTS PASSED`.

## Task 1: A priority queue of ints

### Idea

A priority queue has five operations: `push`, `top`, `pop`, `size` and
`empty`. Here the largest `int` has the highest priority.

Keep the elements in a `std::vector<int>`, in **no particular order**:

- `push(x)`: append `x` at the end
- `top()`: scan the whole vector for the largest element, and return it
- `pop()`: scan for the largest element, then remove it

Removing from the middle of a vector shifts everything after it. You can
avoid that here, because the order inside the vector does not matter: swap
the largest element with the **last** one, then `pop_back`.

```text
m_data:              [ 4 | 9 | 1 | 6 ]

push(7): append      [ 4 | 9 | 1 | 6 | 7 ]

pop():
  max_index() = 1    [ 4 | 9 | 1 | 6 | 7 ]
                           ^ the largest
  swap with last     [ 4 | 7 | 1 | 6 | 9 ]
  pop_back           [ 4 | 7 | 1 | 6 ]
```

`top` and `pop` both need the scan, so it goes in a private helper,
`max_index()`. On an empty queue, `top` and `pop` throw
`std::underflow_error`.

### Declare

Create `pq.h`. Today every function is written **inside** the class, in the
header:

```cpp
#ifndef PQ_H
#define PQ_H

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

// Task 1: a priority queue of ints, on an unsorted vector.
// The largest int has the highest priority.
class IntPQ {
    private:
        std::vector<int> m_data; // the elements, in no particular order

        // The index of the largest element. m_data must not be empty.
        std::size_t max_index() const {
            // TODO
        }

    public:
        void push(int value) {
            // TODO
        }

        int top() const {
            // TODO
        }

        void pop() {
            // TODO
        }

        std::size_t size() const {
            // TODO
        }

        bool empty() const {
            // TODO
        }
};

#endif
```

### Implement

Replace each `// TODO` with these steps:

- `max_index()`: return the index of the largest element in `m_data`
- `push(value)`: append `value` to `m_data`
- `top()`: if the queue is empty, `throw std::underflow_error("top of an empty PQ");`, if not return `m_data[max_index()]`
- `pop()`: if the queue is empty, `throw std::underflow_error("pop of an empty PQ");`, otherwise, swap the largest element with the last one, and then pop_back the last element
- `size()` and `empty()`: one line each, trivial

### Test

Create `test_pq.cpp`. The includes at the top are all you need for the whole
lab.

```cpp
#include <functional>
#include <queue>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "check.h"
#include "pq.h"

static void test_intpq() {
    section("task 1: IntPQ");
    IntPQ pq;
    CHECK(pq.empty());
    CHECK(pq.size() == 0);
    CHECK_THROWS(pq.top(), std::underflow_error);
    CHECK_THROWS(pq.pop(), std::underflow_error);

    pq.push(4);
    pq.push(9);
    pq.push(1);
    pq.push(9);
    pq.push(6);
    CHECK(!pq.empty());
    CHECK(pq.size() == 5);
    CHECK(pq.top() == 9);

    std::vector<int> order;
    while (!pq.empty()) {
        order.push_back(pq.top());
        pq.pop();
    }
    CHECK(order == std::vector<int>({9, 9, 6, 4, 1})); // largest first, both 9s
    CHECK(pq.size() == 0);
}

int main() {
    std::cout << "running tests\n";
    test_intpq();
    return summary();
}
```

Compile and run until you see `ALL TESTS PASSED`. Then answer **Q1** in
`answers.md`.

## Task 2: A function template

### Idea

Suppose you need `larger(a, b)`, which returns the larger of two values, for
`int`s, for `std::string`s and for pairs. Without templates you write three
functions, with the same body and different types.

A **function template** is written once, with a placeholder for the data type:

```cpp
template <typename T>
const T& larger(const T& a, const T& b)
```

`template <typename T>` says: `T` stands for some type, to be chosen later.
When you call `larger(3, 8)`, the compiler sees two `int`s, decides that
`T = int`, and writes the `int` version of the function for you. This is
called **instantiating** the template.

The body compares `a < b`, so `larger` works for any type that has `<`.
`std::pair` has one: it compares `first`, then `second`.

It takes and returns `const T&`, a reference, so a long string or object is never
copied.

### Declare

Add this to `pq.h`, above `class IntPQ`:

```cpp
// Task 2: one function, any type that has operator <
template <typename T>
const T& larger(const T& a, const T& b) {
    // TODO
}
```

### Implement

If `a < b`, return `b`. Otherwise return `a`.

### Test

Add this test to `test_pq.cpp`, and call `test_larger();` in `main`, after
`test_intpq();`:

```cpp
static void test_larger() {
    section("task 2: larger");
    CHECK(larger(3, 8) == 8);
    CHECK(larger(8, 3) == 8);
    CHECK(larger(-2.5, -7.0) == -2.5);
    CHECK(larger(std::string("pear"), std::string("apple")) == "pear");
    std::pair<int, std::string> a = {9, "chat: hello"};
    std::pair<int, std::string> b = {9, "chat: help"};
    CHECK(larger(a, b) == b); // same first, so second decides
}
```

Compile and run until you see `ALL TESTS PASSED`.

### Try it

Add this line to `main`, compile, and read the error:

```cpp
std::cout << larger(3, 2.5) << '\n';
```

Now change it to `larger<double>(3, 2.5)`, compile and run. Answer **Q2**,
then delete the line.

## Task 3: A class template

### Idea

A priority queue of strings would be `IntPQ` again, with `std::string` in
place of every `int` that is an element. A **class template** writes the class
once, with `T` for the element type:

```cpp
template <typename T>
class PQ {
    ...
};
```

To use it, you name `T` in angle brackets: `PQ<std::string>`, `PQ<double>`,
`PQ<std::pair<int, std::string>>`. Each one is a separate class that the
compiler writes from your template, the first time you use it.

Change only the `int`s that are **elements**. Positions and sizes stay
`std::size_t`:

```text
IntPQ                        PQ<T>
-----------------------      -------------------------
std::vector<int> m_data      std::vector<T> m_data
void push(int value)         void push(const T& value)     no copy of a big T
int top() const              const T& top() const          no copy of a big T
std::size_t max_index()      std::size_t max_index()       a position: unchanged
std::size_t size()           std::size_t size()            a count: unchanged
```

**Why everything is in the header.** A template is not code yet. It is a
recipe. The compiler writes `PQ<std::string>` in the `.cpp` file that uses it,
so that file must see the whole recipe, bodies included. That is why `PQ`
lives entirely in `pq.h`, and why the STL's `std::vector` and
`std::priority_queue` live entirely in headers too.

Once `PQ<T>` works, you no longer need `IntPQ`. One line gives the name
`IntPQ` to `PQ<int>`, so the tests of Task 1 keep working without a change:

```cpp
using IntPQ = PQ<int>;
```

### Declare

In `pq.h`, replace the whole `class IntPQ { ... };` with:

```cpp
// Task 3: a priority queue of any type T, on an unsorted vector.
// The largest element has the highest priority.
template <typename T>
class PQ {
    private:
        std::vector<T> m_data; // the elements, in no particular order

        // The index of the largest element. m_data must not be empty.
        std::size_t max_index() const {
            // TODO
        }

    public:
        void push(const T& value) {
            // TODO
        }

        const T& top() const {
            // TODO
        }

        void pop() {
            // TODO
        }

        std::size_t size() const {
            // TODO
        }

        bool empty() const {
            // TODO
        }
};

// The name IntPQ now means PQ<int>.
using IntPQ = PQ<int>;
```

### Implement

Each body is the body you wrote for `IntPQ`. Copy it in. None of them names
`int`, so nothing else changes.

Compile and run **before** you add a test: the Task 1 and Task 2 tests must
still pass. `IntPQ` is now a class that the compiler writes from your
template.

### Test

Add this test, and call `test_pq_types();` in `main`:

```cpp
static void test_pq_types() {
    section("task 3: PQ<T>");
    PQ<std::string> words;
    words.push("pear");
    words.push("apple");
    words.push("zebra");
    words.push("mango");
    CHECK(words.size() == 4);
    CHECK(words.top() == "zebra");
    words.pop();
    CHECK(words.top() == "pear");

    PQ<std::pair<int, std::string>> server;
    server.push({2, "summarize 500 PDFs"});
    server.push({9, "chat: hello"});
    server.push({5, "autocomplete"});
    CHECK(server.top().second == "chat: hello");
    server.pop();
    CHECK(server.top().second == "autocomplete");
    server.pop();
    server.pop();
    CHECK(server.empty());
    CHECK_THROWS(server.top(), std::underflow_error);
}
```

Compile and run until you see `ALL TESTS PASSED`.

### Try it

A server request is easier to read as a struct than as a pair. Add this to
`test_pq.cpp`, above `main`. You will use it in Task 4 too:

```cpp
struct Request {
    int priority;
    std::string text;
};
```

Add these two lines to `main`, then compile:

```cpp
PQ<Request> requests;
requests.push({2, "summarize 500 PDFs"});
```

It compiles. Now add a third line, `requests.top();`, and compile again. Read
the error, from its first line to the line that names your `main`. Answer
**Q3**, then delete the three lines (keep `struct Request`).

## Task 4: A comparator

### Idea

`PQ<Request>` failed because `Request` has no operator `<`. The fix is to let the user
of `PQ` say how to compare two elements. That is what the last argument of
`std::priority_queue<int, std::vector<int>, std::greater<int>>` does.

A **comparator** is an object that you call like a function. `std::less<int>`
and `std::greater<int>` are two of them, from `<functional>`:

```cpp
std::less<int> lt;
std::greater<int> gt;
lt(3, 8) // true, because 3 < 8
gt(3, 8) // false, because 3 > 8 is false
```

`PQ` gets a second template argument, `Compare`, and keeps one comparator,
`m_less`. The rule: `m_less(a, b)` is true when `a` has a **lower priority**
than `b`. The scan in `max_index()` calls `m_less` instead of `<`, so the
comparator decides which element is "largest":

```text
Compare               m_less(a, b) is             top() is
-----------------     -----------------------     ------------------------
std::less<int>        a < b                       the largest int
std::greater<int>     a > b                       the smallest int
ByPriority            a.priority < b.priority     the most urgent Request
```

`typename Compare = std::less<T>` gives `Compare` a **default**. When you
write `PQ<int>`, you get `PQ<int, std::less<int>>`, the same max-queue as
before. So every test so far keeps passing.

You can write your own comparator: a struct with an `operator()` that takes
two elements and returns `bool`. Mark it `const`, because `top()` is `const`
and calls it.

### Declare

In `pq.h`, add `#include <functional>` to the includes. Then change the
first lines of `PQ`:

```cpp
// Task 4: a priority queue of any type T, on an unsorted vector.
// m_less(a, b) is true when a has a LOWER priority than b.
template <typename T, typename Compare = std::less<T>>
class PQ {
    private:
        std::vector<T> m_data; // the elements, in no particular order
        Compare m_less; // the rule for "lower priority"
```

### Implement

One line changes, in `max_index()`. Replace

```cpp
if (m_data[best] < m_data[i]) {
```

with

```cpp
if (m_less(m_data[best], m_data[i])) {
```

Compile and run before you add a test: everything must still pass.

### Test

Add the comparator below `struct Request`, then this test. Call
`test_compare();` in `main`:

```cpp
struct ByPriority {
    bool operator()(const Request& a, const Request& b) const {
        return a.priority < b.priority;
    }
};

static void test_compare() {
    section("task 4: Compare");
    PQ<int, std::greater<int>> low_first;
    for (int x : {4, 9, 1, 9, 6}) {
        low_first.push(x);
    }
    std::vector<int> order;
    while (!low_first.empty()) {
        order.push_back(low_first.top());
        low_first.pop();
    }
    CHECK(order == std::vector<int>({1, 4, 6, 9, 9})); // smallest first

    PQ<Request, ByPriority> server;
    server.push({2, "summarize 500 PDFs"});
    server.push({9, "chat: hello"});
    server.push({5, "autocomplete"});
    CHECK(server.top().text == "chat: hello");
    server.pop();
    CHECK(server.top().text == "autocomplete");

    // the same answers as std::priority_queue, on 1000 random numbers
    std::priority_queue<int, std::vector<int>, std::greater<int>> theirs;
    PQ<int, std::greater<int>> mine;
    std::mt19937 rng(212);
    for (int i = 0; i < 1000; ++i) {
        int x = static_cast<int>(rng() % 100);
        theirs.push(x);
        mine.push(x);
    }
    bool same = mine.size() == theirs.size();
    while (same && !theirs.empty()) {
        same = mine.top() == theirs.top();
        mine.pop();
        theirs.pop();
    }
    CHECK(same);
    CHECK(mine.empty());
}
```

Compile and run until you see `ALL TESTS PASSED`. Then answer **Q4**.

## Task 5: How fast is it?

### Idea

With $n$ elements, `push` is one append, but `pop` scans all of them. Push
$n$ elements, then pop them all: the pops scan $n$, then $n - 1$, then
$n - 2$, ... elements.

`bench.cpp` does exactly that with your `PQ<int>` and with
`std::priority_queue<int>`, for $n$ = 10,000, 20,000, 40,000 and 80,000. It
uses the same random numbers for both, and checks that both pop the same
numbers in the same order. The `ratio` column is each time divided by the
time on the row above, when $n$ doubles.

### Run

`-O2` tells the compiler to optimize the code. Always use it when you measure
speed:

```bash
$ g++ -std=c++17 -O2 bench.cpp -o bench
$ ./bench
```

It takes a few seconds. On one laptop it printed this. Your times will differ,
but the ratios should be close:

```text
       n      PQ (ms)    ratio       std (ms)    ratio
   10000         33.3                    0.50
   20000        137.4      4.1           0.95      1.9
   40000        574.2      4.2           2.07      2.2
   80000       2259.7      3.9           4.94      2.4
```

Answer **Q5** with **your** numbers.

## Submission

Upload these files to Gradescope:

- `pq.h`
- `answers.md`

> [!WARNING]
> **Upload those two files and nothing else.** The autograder checks this
> before it looks at your code.

Your code should compile clean under `-Wall -Wextra -Werror`.

Questions: ask the instructor or a TA **in the room**, or post on Ed.
