# Lab 07: Recursion, step by step

A recursive function calls itself. That sentence is easy; what happens when
the program runs is not. Where does the first call wait while the second one
runs? How does an answer get back to the *caller*? Today you write recursive
functions on a linked list, and then **watch them run**: every call, every
frame on the call stack, and every return, one step at a time.

> [!CAUTION]
> Turn off AI autocomplete for this lab. How to switch it off,
> in VS Code and in other editors:
> [Turn off AI autocomplete](../lab-01/setup.md#turn-off-ai-autocomplete).

**What you write.** You work in `starter/` and write three files there:
`list.h`, `list.cpp`, and `test_list.cpp`. Do not change the other files. They
are tools for you:

- `check.h`: a small test harness, the same as in Lab 06
- `trace.h`: the `WATCH` line that makes your recursion visible (see below)
- `watch.cpp`: a program that runs one of your functions and records every
  call, for the viewer
- `answers.md`: the questions of this lab

**How you work.** Each task adds a few small functions and a test. Every task
ends the same way. Compile and run the tests:

```bash
$ cd starter
$ g++ -std=c++17 -Wall -Wextra -g list.cpp test_list.cpp -o test_list
$ ./test_list
```

A failing check prints its line number. Do not start the next task until you
see `ALL TESTS PASSED`.

## A list is recursive

A linked list is **either**:

- **empty**: `nullptr`, **or**
- **a node, followed by a smaller list**: `p->data`, then the list that starts
  at `p->next`

There is no list class today: just a `Node` struct (Task 1) and plain
functions that take a `Node*`. Without a class there are no wrappers or
helpers to write, so all your attention goes to the recursion itself.

Because a list is defined in terms of a smaller list, recursion fits it well.
Every list function in Tasks 1 to 3 has the same shape (Task 4 changes the
base case):

```cpp
ANSWER f(Node* p) {
    WATCH(p);
    if (p == nullptr) {
        return ...; // the empty list: answer directly. This is the base case
    }
    ... f(p->next) ... // the rest of the list: let the recursive call answer it
}
```

When you write the last line, **do not trace the whole list in your head**.
Assume that `f(p->next)` returns the right answer for the rest of the list.
Then ask only one question: with that answer in hand, how do I answer for the
whole list, which is `p` plus the rest? This is the **recursive leap of
faith**, and it is the key to writing recursive functions.

## Watching your recursion

`trace.h` gives you one line, `WATCH(p);`. Put it as the **first line** of
every recursive function you write today. It does two things:

- it counts every call in `trace::calls`. Some tests use that count, so a
  missing `WATCH` makes those tests fail
- when `watch.cpp` runs your function, it records every call and every return
  in a log

`viewer/index.html` plays that log in your browser, one step at a time. You
will see:

- the **call stack**: one frame per call that has started and not returned
  yet. The top frame is the one running; every frame below it is paused,
  waiting for the call above it to return
- the **list**, with the node each paused call is looking at
- the **call tree**: one circle per call, drawn as each call starts, and green
  with its answer once it returns

## Task 1: Build and count

### Idea

Two functions. Each one answers for the list that starts at `p` (or, for
`build`, at index `i`).

**`build(values, i)`** makes a list from the numbers `values[i]`,
`values[i + 1]`, ..., up to the end of the vector. It is recursive too:

- base case: if `i == values.size()`, no numbers are left. Return `nullptr`,
  the empty list
- otherwise: the list is a new node holding `values[i]`, followed by the list
  that `build(values, i + 1)` makes from the rest

```text
build({3, -5, 6}, 0)  = new Node(3,  build({3, -5, 6}, 1))
build({3, -5, 6}, 1)  = new Node(-5, build({3, -5, 6}, 2))
build({3, -5, 6}, 2)  = new Node(6,  build({3, -5, 6}, 3))
build({3, -5, 6}, 3)  = nullptr                              base case
```

**`size(p)`** is the number of nodes. The empty list has 0. Otherwise the
list is one node plus the rest: `1 + size(p->next)`.

### Declare

Create `list.h`. `Node` has a constructor, so `new Node(3, rest)` makes a node
holding 3 whose `next` is `rest`.

```cpp
#ifndef LIST_H
#define LIST_H

#include <cstddef>
#include <ostream>
#include <vector>

// A list is either empty (nullptr), or a node followed by a smaller list.
struct Node {
    int data;
    Node* next;
    Node(int d, Node* n) : data(d), next(n) {}
};

// Task 1
Node* build(const std::vector<int>& values, std::size_t i);
int size(Node* p);

#endif
```

### Implement

Create `list.cpp`. Each function starts with `WATCH`, then the base case, then
the recursive case:

```cpp
#include "list.h"

#include "trace.h"

Node* build(const std::vector<int>& values, std::size_t i) {
    WATCH(i);
    // TODO: base case: i is past the last value. Return nullptr
    // TODO: return a new Node holding values[i], followed by build(values, i + 1)
}

int size(Node* p) {
    WATCH(p);
    // TODO: base case: the empty list has 0 nodes
    // TODO: one node, plus the size of the rest
}
```

### Test

Create `test_list.cpp`. Until Task 3, a small loop frees the lists the tests
make. The test also uses `nodes_in_use()` from `check.h`: it counts the
objects made with `new` and not deleted yet, so it shows whether `build` made
one node per value, and whether every node was freed.

```cpp
#include <sstream>
#include <string>
#include <vector>

#include "check.h"
#include "list.h"
#include "trace.h"

// Frees a list with a loop. Task 3 writes clear, the recursive way.
static void free_list(Node* p) {
    while (p != nullptr) {
        Node* next = p->next;
        delete p;
        p = next;
    }
}

static void test_build_size() {
    section("task 1: build, size");
    const long long before = nodes_in_use();
    Node* list = build({3, -5, 6, 18}, 0);
    CHECK(nodes_in_use() == before + 4); // one node per value
    CHECK(list != nullptr && list->data == 3);
    CHECK(list->next->data == -5 && list->next->next->data == 6);
    CHECK(list->next->next->next->data == 18);
    CHECK(list->next->next->next->next == nullptr);
    CHECK(build({}, 0) == nullptr); // no values: the empty list

    CHECK(size(list) == 4);
    CHECK(size(nullptr) == 0);
    CHECK(size(list->next->next) == 2); // the list that starts at 6

    trace::calls = 0;
    size(list);
    CHECK(trace::calls == 5); // one call per node, and one for nullptr

    free_list(list);
    CHECK(nodes_in_use() == before);
}

int main() {
    std::cout << "running tests\n";
    test_build_size();
    return summary();
}
```

Compile and run until you see `ALL TESTS PASSED`.

### Watch it

Build `watch` with your code. `-DTASK=1` tells it that you have finished
Task 1, so it only calls what you have written. Then run `size` on the list
`3 -5 6 18`, and save the log in a file:

```bash
$ g++ -std=c++17 -Wall -Wextra -g -DTASK=1 list.cpp watch.cpp -o watch
$ ./watch size 3 -5 6 18 > size.log
size = 4   (5 calls)
```

Open `viewer/index.html` in your browser, and drop `size.log` on it. Step
forward with the right arrow key. Watch the stack grow as each call starts,
then shrink as the answers come back, one call at a time. Answer **Q1** in
`answers.md`.

## Task 2: Down and up

### Idea

A recursive call splits a function in two: the code **before** the call and
the code **after** it. Code before the call runs on the way down, while the
calls are starting. Code after the call runs on the way back up, while they
return, in the opposite order.

```text
print(3)                    prints 3     on the way down
  print(-5)                 prints -5
    print(6)                prints 6
      print(nullptr)        base case: nothing to print, return

print_back(3)                            on the way down: nothing printed yet
  print_back(-5)
    print_back(6)
      print_back(nullptr)   base case: nothing to print, return
    print_back(6)           prints 6     on the way back up
  print_back(-5)            prints -5
print_back(3)               prints 3
```

`print(p, out)` writes each value followed by a space, front to back:
`3 -5 6 `. `print_back(p, out)` writes them back to front: `6 -5 3 `. The two
functions are the same, except for **where** the line that writes is.

Both write to `out`, an `std::ostream`: `std::cout`, or a string stream in
the tests.

### Declare

Add to `list.h`, below `size`:

```cpp
// Task 2
void print(Node* p, std::ostream& out);
void print_back(Node* p, std::ostream& out);
```

### Implement

```cpp
void print(Node* p, std::ostream& out) {
    WATCH(p);
    // TODO: base case: the empty list prints nothing. Return
    // TODO: write p->data and a space to out
    // TODO: print the rest
}

void print_back(Node* p, std::ostream& out) {
    WATCH(p);
    // TODO: base case: the empty list prints nothing. Return
    // TODO: print the rest, backwards
    // TODO: then write p->data and a space to out
}
```

### Test

Add these to `test_list.cpp`, and call `test_print` from `main`. `text(p)`
returns what `print` writes, as a string; the tests of Task 3 use it too.

```cpp
// The text that print writes, as a string.
static std::string text(Node* p) {
    std::ostringstream out;
    print(p, out);
    return out.str();
}

static std::string text_back(Node* p) {
    std::ostringstream out;
    print_back(p, out);
    return out.str();
}

static void test_print() {
    section("task 2: print, print_back");
    Node* list = build({3, -5, 6, 18}, 0);
    CHECK(text(list) == "3 -5 6 18 ");
    CHECK(text_back(list) == "18 6 -5 3 ");
    CHECK(text(nullptr) == "");
    CHECK(text_back(nullptr) == "");

    Node* one = build({7}, 0);
    CHECK(text(one) == "7 ");
    CHECK(text_back(one) == "7 ");

    free_list(list);
    free_list(one);
}
```

Compile and run until you see `ALL TESTS PASSED`.

### Watch it

Rebuild `watch` with `-DTASK=2`, and record both functions on the same list:

```bash
$ g++ -std=c++17 -Wall -Wextra -g -DTASK=2 list.cpp watch.cpp -o watch
$ ./watch print 1 2 3 4 > print.log
$ ./watch print_back 1 2 3 4 > print_back.log
```

Drop each log on the viewer. Watch the **Printed so far** panel, and the
stack, at the moment each number appears. Answer **Q2**.

## Task 3: Return the repaired rest

### Idea

To **change** a list, a recursive function returns the first node of the
changed list, and its caller links that node in. This is the pattern:

```cpp
p->next = f(p->next); // change the rest, then link in whatever comes back
return p; // and this node stays first
```

**`remove_first(p, value)`** deletes the first node that holds `value`, and
returns the first node of the list without it:

- the empty list: nothing to remove. Return `nullptr`
- `p` holds `value`: remember `p->next`, delete `p`, and return the node you
  remembered. The rest of the list is now the whole list
- otherwise: remove from the rest, link in what comes back, and return `p`

```text
remove_first(3 -> -5 -> 6 -> 18, 6)

remove_first(3):   3->next  = remove_first(-5)
remove_first(-5):  -5->next = remove_first(6)
remove_first(6):   6 holds the value: delete it, return 18
                   -5->next = 18,    the list is 3 -> -5 -> 18
```

Whoever calls it keeps the result: `list = remove_first(list, 6);`. If the
first node is the one removed, the list now starts at the second node.

**`clear(p)`** deletes every node. It must clear the rest **before** it
deletes `p`: once `p` is deleted, `p->next` can no longer be read.

### Declare

Add to `list.h`:

```cpp
// Task 3
Node* remove_first(Node* p, int value);
void clear(Node* p);
```

### Implement

```cpp
Node* remove_first(Node* p, int value) {
    WATCH(p);
    // TODO: base case: the empty list. Return nullptr
    // TODO: if p holds value: remember p->next, delete p, return what you remembered
    // TODO: otherwise: p->next = remove_first(p->next, value), and return p
}

void clear(Node* p) {
    WATCH(p);
    // TODO: base case: the empty list. Nothing to free
    // TODO: clear the rest first, then delete p
}
```

### Test

Add this test, and call it from `main`:

```cpp
static void test_remove_clear() {
    section("task 3: remove_first, clear");
    Node* list = build({3, -5, 6, -5, 18}, 0);
    const long long before = nodes_in_use();

    list = remove_first(list, 6); // a node in the middle
    CHECK(text(list) == "3 -5 -5 18 ");
    CHECK(nodes_in_use() == before - 1); // the removed node was deleted

    list = remove_first(list, -5); // only the first -5
    CHECK(text(list) == "3 -5 18 ");

    list = remove_first(list, 3); // the first node
    CHECK(text(list) == "-5 18 ");

    list = remove_first(list, 99); // not there: nothing changes
    CHECK(text(list) == "-5 18 ");
    CHECK(nodes_in_use() == before - 3);

    clear(list);
    CHECK(nodes_in_use() == before - 5); // every node is gone
    clear(nullptr); // nothing to free
}
```

Compile and run until you see `ALL TESTS PASSED`.

### Watch it

```bash
$ g++ -std=c++17 -Wall -Wextra -g -DTASK=3 list.cpp watch.cpp -o watch
$ ./watch remove_first 6 3 -5 6 18 > remove.log
```

The first number after `remove_first` is the value to remove; the rest is the
list. In the viewer, a deleted node turns red. Watch which node still points
at it, and on which step that changes. Answer **Q3**.

> [!TIP]
> If you have time, watch `clear` too: `./watch clear 1 2 3 > clear.log`.
> In which order are the nodes deleted?

## Task 4: The call tree is the cost

### Idea

Every call costs time. To know how fast a recursive function is, count its
calls: the circles of its call tree.

**`power(b, n)`** computes $b^n$ with one call for each $n$:
$b^n = b \cdot b^{n-1}$, and $b^0 = 1$.

**`power_2(b, n)`** computes the same number by **halving** $n$. It computes
`half` $= b^{n/2}$ once (with integer division, so $n/2$ rounds down), then:

- $n$ even: $b^n$ is `half * half`
- $n$ odd: $b^n$ is `b * half * half`

**`max_slow(p)`** returns the largest value in a non-empty list. It is written
for you below, and it is correct, but look at it closely: the same recursive
call can happen **twice**.

**`max_fast(p)`** does the same job, but calls itself **once**: store the
answer for the rest in a variable, then compare.

### Declare

Add to `list.h`:

```cpp
// Task 4
double power(double b, int n);
double power_2(double b, int n);
int max_slow(Node* p);
int max_fast(Node* p);
```

### Implement

`max_slow` is given; type it in as it is.

```cpp
double power(double b, int n) {
    WATCH(n);
    // TODO: base case: n == 0. Return 1
    // TODO: return b times power(b, n - 1)
}

double power_2(double b, int n) {
    WATCH(n);
    // TODO: base case: n == 0. Return 1
    // TODO: half = power_2(b, n / 2), called ONCE and kept in a variable
    // TODO: n even: return half * half. n odd: return b * half * half
}

int max_slow(Node* p) {
    WATCH(p);
    if (p->next == nullptr) {
        return p->data; // one node: it is the largest
    }
    if (p->data > max_slow(p->next)) {
        return p->data;
    }
    return max_slow(p->next); // the same call, again
}

int max_fast(Node* p) {
    WATCH(p);
    // TODO: base case: one node (p->next == nullptr). Return its data
    // TODO: rest = max_fast(p->next), called once
    // TODO: return the larger of p->data and rest
}
```

### Test

Add this test, and call it from `main`. From here on, the tests free their
lists with your `clear`.

```cpp
static void test_cost() {
    section("task 4: the call tree is the cost");
    CHECK(power(2, 10) == 1024);
    CHECK(power(3, 0) == 1);
    CHECK(power_2(2, 10) == 1024);
    CHECK(power_2(3, 5) == 243);
    CHECK(power_2(5, 0) == 1);

    trace::calls = 0;
    power(2, 1000);
    CHECK(trace::calls == 1001); // n, n - 1, ..., 1, 0
    trace::calls = 0;
    power_2(2, 1000);
    CHECK(trace::calls == 11); // 1000, 500, 250, 125, 62, 31, 15, 7, 3, 1, 0

    Node* list = build({4, 9, 2, 7}, 0);
    CHECK(max_slow(list) == 9);
    CHECK(max_fast(list) == 9);

    std::vector<int> values;
    for (int v = 1; v <= 20; v++) {
        values.push_back(v);
    }
    Node* up = build(values, 0); // 1 2 3 ... 20
    trace::calls = 0;
    CHECK(max_fast(up) == 20);
    CHECK(trace::calls == 20); // one call per node
    trace::calls = 0;
    CHECK(max_slow(up) == 20);
    CHECK(trace::calls == 1048575); // 2^20 - 1

    clear(list);
    clear(up);
}
```

Compile and run until you see `ALL TESTS PASSED`.

### Watch it

Build `watch` with `-DTASK=4`. For large inputs you only need the number of
calls, which `watch` prints at the end, so send the log to `/dev/null`, a file
that throws away whatever is written to it:

```bash
$ g++ -std=c++17 -Wall -Wextra -g -DTASK=4 list.cpp watch.cpp -o watch
$ ./watch power 2 100 > /dev/null
power = 1.26765e+30   (101 calls)
$ ./watch max_slow 1..5 > max_slow.log
```

`1..5` means the list `1 2 3 4 5`. Drop `max_slow.log` on the viewer, and look
at the shape of the call tree. Then fill in the table of **Q4**.

## If you have time: insert_sorted

`insert_sorted(p, value)` inserts `value` into a list kept in increasing
order, and returns the first node of the new list: `1 4 9` with 5 inserted is
`1 4 5 9`. It is another "return the repaired rest": if `value` belongs before
`p` (or the list is empty), the new node goes first; otherwise insert into the
rest and link it in.

Add `Node* insert_sorted(Node* p, int value);` to `list.h`, write it with
`WATCH(p)`, and add this test:

```cpp
static void test_insert_sorted() {
    section("if you have time: insert_sorted");
    Node* list = build({1, 4, 9}, 0);
    list = insert_sorted(list, 5);
    CHECK(text(list) == "1 4 5 9 ");
    list = insert_sorted(list, 0); // a new first node
    CHECK(text(list) == "0 1 4 5 9 ");
    list = insert_sorted(list, 12); // a new last node
    CHECK(text(list) == "0 1 4 5 9 12 ");
    Node* empty = insert_sorted(nullptr, 3);
    CHECK(text(empty) == "3 ");
    clear(list);
    clear(empty);
}
```

Watch it with `-DTASK=5`: `./watch insert_sorted 5 1 4 9 > insert.log`.

## Submission

Upload these files to Gradescope:

- `list.h`
- `list.cpp`
- `answers.md`

> [!WARNING]
> **Upload those three files and nothing else.** The autograder checks this
> before it looks at your code.

Your code should compile clean under `-Wall -Wextra -Werror`.

Questions: ask the instructor or a TA **in the room**, or post on Ed.
