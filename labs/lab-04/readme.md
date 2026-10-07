# Lab 04: Undo and redo

Today you will build the undo and redo of a small pixel art program. The
program keeps two **stacks** of paints. Every pixel you paint goes on the undo
stack. `undo()` takes the most recent paint off that stack, puts the old color
back, and keeps the paint on the redo stack in case you want it again. At the
end you will replay a whole drawing session and watch the canvas and the two
stacks in a viewer.

> [!CAUTION]
> Turn off AI autocomplete for this lab. How to switch it off,
> in VS Code and in other editors:
> [Turn off AI autocomplete](../lab-01/setup.md#turn-off-ai-autocomplete).

Work in `starter/`. It holds four files you do not change: `check.h` (a small
test harness), `session.cpp` and `script.txt` (for Task 4), and `answers.md`.
You create three files there: `canvas.h`, `canvas.cpp`, and `test_canvas.cpp`.

Every task ends the same way. Compile and run:

```bash
$ cd starter
$ g++ -std=c++17 -Wall -Wextra -g canvas.cpp test_canvas.cpp -o test_canvas
$ ./test_canvas
```

A failing check prints its line number. Do not start the next task until you
see `ALL TESTS PASSED`.

## std::stack in one minute

`std::stack<T>` from the STL is the stack shown in lecture, ready to use:

| call | does | cost |
|---|---|---|
| `s.push(x)` | puts `x` on top | $O(1)$ |
| `s.top()` | returns the top element | $O(1)$ |
| `s.pop()` | removes the top element, returns nothing | $O(1)$ |
| `s.empty()` | `true` when there is nothing in it | $O(1)$ |
| `s.size()` | how many elements | $O(1)$ |

> [!WARNING]
> `top()` and `pop()` on an empty stack are **undefined behavior**: no error,
> no exception, just garbage or a crash. Always check `empty()` first.

To read the top **and** remove it, use two calls:

```cpp
Edit e = s.top();  // copy the top
s.pop();           // then remove it
```

## Task 1: Painting

The canvas is a 16 x 16 grid of pixels. Each pixel holds one color, written
as one character: `'.'` is empty, `'r'` is red, `'y'` is yellow, and so on.

```text
        col 0 1 2 3 4 5 ...
row 0       . . . . . .
row 1       . . . . . .
row 2       . . . r . .      <- pixel (2, 3) is red
row 3       . . . . . .
```

Every paint is an **edit**. An edit remembers which pixel changed, its color
before, and its color after:

```c++
paint(2, 3, 'b')    on a red pixel   ->   Edit{ 2, 3, 'r', 'b' }
                                               row col before after
```

The class has three data members:

- `m_pixels`: the colors, a `SIZE x SIZE` 2D array of `char`
- `m_undo`: the edits that `undo()` can reverse (Task 2)
- `m_redo`: the edits that `redo()` can put back (Task 3)

The constructor sets every pixel to `'.'`. Both stacks start empty on their
own. The class calls `new` nowhere, so it needs no destructor.

The private method `check(row, col)` throws `std::out_of_range` when the
pixel is not on the canvas. `get` and `paint` call it first.

### Declare

Create `canvas.h`:

```cpp
#ifndef CANVAS_H
#define CANVAS_H

#include <cstddef>
#include <stack>

const int SIZE = 16;  // the canvas is SIZE x SIZE pixels

// One paint: which pixel, its color before, and its color after.
struct Edit {
    int  row;
    int  col;
    char before;
    char after;
};

class Canvas {
    private:
        char m_pixels[SIZE][SIZE]; // one color per pixel
        std::stack<Edit> m_undo;   // paints that undo() can reverse (Task 2)
        std::stack<Edit> m_redo;   // paints that redo() can put back (Task 3)

        void check(int row, int col) const;

    public:
        Canvas();

        char get(int row, int col) const;
        void paint(int row, int col, char color);
};

#endif
```

> [!NOTE]
> `Edit e{2, 3, 'r', 'b'};` builds an `Edit` with its four fields in order:
> `e.row` is 2, `e.col` is 3, `e.before` is `'r'`, `e.after` is `'b'`.

### Implement

Create `canvas.cpp`:

```cpp
#include "canvas.h"

#include <stdexcept>

Canvas::Canvas() {
    // TODO: set every pixel to '.' (two nested loops)
}

void Canvas::check(int row, int col) const {
    // TODO: if row or col is less than 0 or at least SIZE,
    //       throw std::out_of_range("pixel outside the canvas")
}

char Canvas::get(int row, int col) const {
    // TODO: check the position, then return the pixel's color
}

void Canvas::paint(int row, int col, char color) {
    // TODO: check the position
    // TODO: if the pixel already has this color, return: nothing changes
    // TODO: build Edit e{row, col, <color now>, color}
    // TODO: set the pixel to e.after
}
```

### Test

Create `test_canvas.cpp`:

```cpp
#include <sstream>
#include <stdexcept>
#include <string>

#include "canvas.h"
#include "check.h"

static void test_painting() {
    section("task 1: painting");
    Canvas cv;
    CHECK(cv.get(0, 0) == '.');
    CHECK(cv.get(15, 15) == '.');

    cv.paint(3, 5, 'r');
    CHECK(cv.get(3, 5) == 'r');
    CHECK(cv.get(5, 3) == '.');   // row 5, col 3 is a different pixel
    cv.paint(3, 5, 'b');          // paint over it
    CHECK(cv.get(3, 5) == 'b');

    CHECK_THROWS(cv.get(16, 0), std::out_of_range);
    CHECK_THROWS(cv.get(0, -1), std::out_of_range);
    CHECK_THROWS(cv.paint(-1, 4, 'r'), std::out_of_range);
    CHECK_THROWS(cv.paint(4, 16, 'r'), std::out_of_range);
}

int main() {
    std::cout << "running tests\n";
    test_painting();
    return summary();
}
```

Compile, run, and answer **Q1** in `answers.md`.

## Task 2: Undo

Every paint goes on the undo stack. The most recent paint is on top, so it is
the first one undone:

```
call                  pixel (0, 0) after     undo stack after the 3 calls

paint(0, 0, 'r')      r                      | 1 2 . b |  <- top
paint(0, 0, 'g')      g                      | 0 0 r g |
paint(1, 2, 'b')      g                      | 0 0 . r |
                                             +---------+
```

To undo a paint, set its pixel back to its **before** color:

```
undo:  pop  1 2 . b    set (1, 2) to '.'
undo:  pop  0 0 r g    set (0, 0) to 'r'     not '.': the color it had before
undo:  pop  0 0 . r    set (0, 0) to '.'
```

`undo()` returns `true` when it undid something, and `false` when the undo
stack is empty.

### Declare

Add to the `public:` section:

```cpp
        bool   undo();
        size_t undo_size() const;
```

### Implement

At the end of `paint`, add one line:

```cpp
    m_undo.push(e);
```

Then add:

```cpp
bool Canvas::undo() {
    // TODO: if the undo stack is empty, return false
    // TODO: copy the top edit, then pop it
    // TODO: set its pixel to its before color
    // TODO: return true
}

size_t Canvas::undo_size() const {
    // TODO: return the number of edits on the undo stack
}
```

### Test

Add this test, and call it from `main`:

```cpp
static void test_undo() {
    section("task 2: undo");
    Canvas cv;
    cv.paint(0, 0, 'r');
    cv.paint(0, 0, 'g');          // same pixel, painted over
    cv.paint(1, 2, 'b');
    CHECK(cv.undo_size() == 3);

    CHECK(cv.undo());             // (1, 2) goes back to '.'
    CHECK(cv.get(1, 2) == '.');
    CHECK(cv.undo());             // (0, 0) goes back to 'r', not '.'
    CHECK(cv.get(0, 0) == 'r');
    CHECK(cv.undo());
    CHECK(cv.get(0, 0) == '.');
    CHECK(cv.undo_size() == 0);

    CHECK(!cv.undo());            // nothing left to undo

    cv.paint(4, 4, '.');          // same color: nothing changes,
    CHECK(cv.undo_size() == 0);   // nothing is recorded
}
```

Compile, run, and answer **Q2**.

## Task 3: Redo

An undone paint is not thrown away. `undo()` pushes it onto the redo stack.
`redo()` pops it from there, sets the pixel to its **after** color, and pushes
it back onto the undo stack. The two stacks pass edits back and forth:

```
            undo()
         ----------->
  m_undo              m_redo
         <-----------
            redo()
```

One more rule. When you undo, then paint something **new**, the edits on the
redo stack belong to a drawing that no longer exists. A new paint clears the
redo stack.

`std::stack` has no `clear()`. Replace the whole stack with an empty one:

```cpp
m_redo = std::stack<Edit>();
```

### Declare

Add to the `public:` section:

```cpp
        bool   redo();
        size_t redo_size() const;
```

### Implement

- In `undo()`, after setting the pixel, push the edit onto `m_redo`.
- In `paint`, after `m_undo.push(e);`, clear `m_redo`.

Then add:

```cpp
bool Canvas::redo() {
    // TODO: if the redo stack is empty, return false
    // TODO: copy the top edit, then pop it
    // TODO: set its pixel to its after color
    // TODO: push it onto the undo stack
    // TODO: return true
}

size_t Canvas::redo_size() const {
    // TODO: return the number of edits on the redo stack
}
```

### Test

Fill in the table in **Q3** on paper **first**. Then add this test, and call
it from `main`:

```cpp
static void test_redo() {
    section("task 3: redo");
    Canvas cv;
    cv.paint(0, 0, 'r');
    cv.paint(0, 1, 'g');
    cv.paint(0, 2, 'b');
    CHECK(cv.redo_size() == 0);
    CHECK(!cv.redo());

    cv.undo();
    cv.undo();
    CHECK(cv.get(0, 1) == '.');
    CHECK(cv.undo_size() == 1);
    CHECK(cv.redo_size() == 2);

    CHECK(cv.redo());             // (0, 1) comes back first
    CHECK(cv.get(0, 1) == 'g');
    CHECK(cv.get(0, 2) == '.');
    CHECK(cv.undo_size() == 2);
    CHECK(cv.redo_size() == 1);

    cv.paint(5, 5, 'y');          // a new paint: (0, 2) can not come back
    CHECK(cv.redo_size() == 0);
    CHECK(!cv.redo());

    cv.undo();
    cv.undo();
    cv.undo();
    cv.redo();
    cv.redo();
    cv.redo();
    CHECK(cv.get(0, 0) == 'r');
    CHECK(cv.get(0, 1) == 'g');
    CHECK(cv.get(5, 5) == 'y');
}
```

Compile, run, and finish **Q3**.

## Task 4: Print and watch

A stack only shows its top. To see everything in it, pop it until it is
empty. You must not destroy the canvas's stacks to print them, so pop a
**copy**: a parameter passed by value is a copy.

`print` writes the 16 rows of pixels, then each stack with the top first, in
this exact format (the viewer reads it):

```
................
.....r..........
................      (16 rows of 16 characters)
...
undo 2
    1 5 . r
    0 0 . g
redo 0
```

Each edit line starts with 4 spaces, then `row col before after`, separated
by single spaces.

### Declare

Add `#include <ostream>` at the top of `canvas.h`, and add to the `public:`
section:

```cpp
        void print(std::ostream& out) const;
```

### Implement

```cpp
// s is a copy: popping it does not touch the canvas's stacks.
static void print_stack(std::ostream& out, const char* name, std::stack<Edit> s) {
    out << name << ' ' << s.size() << '\n';
    while (!s.empty()) {
        // TODO: print the top edit as:    1 5 . r
        //       4 spaces, then row, col, before, after with one space between
        // TODO: pop
    }
}

void Canvas::print(std::ostream& out) const {
    // TODO: print the pixels: one line per row, no spaces, '\n' after each row
    print_stack(out, "undo", m_undo);
    print_stack(out, "redo", m_redo);
}
```

### Test

Add this test, and call it from `main`:

```cpp
static void test_print() {
    section("task 4: print");
    Canvas cv;
    cv.paint(0, 0, 'r');
    cv.paint(0, 0, 'g');
    cv.paint(15, 15, 'b');
    cv.undo();

    std::ostringstream out;
    cv.print(out);

    std::string want = "g...............\n";
    for (int r = 1; r < 16; r++) {
        want += "................\n";
    }
    want += "undo 2\n"
            "    0 0 r g\n"
            "    0 0 . r\n"
            "redo 1\n"
            "    15 15 . b\n";
    CHECK(out.str() == want);

    CHECK(cv.undo_size() == 2);  // print did not change the stacks
    CHECK(cv.redo_size() == 1);
}
```

`std::ostringstream` is an output stream that writes into a string instead of
the screen. `out.str()` gives you that string.

Compile and run until you see `ALL TESTS PASSED`.

### Watch it

`session.cpp` reads `script.txt`, one command per line, runs each command on
your `Canvas`, and calls `print` after each one. The script draws a smiley,
changes its mind a few times, and undoes and redoes. Build it and save its
output:

```bash
$ g++ -std=c++17 -Wall -Wextra -g canvas.cpp session.cpp -o session
$ ./session script.txt > session.log
```

Open `viewer/index.html` in a browser and drop `session.log` on it. Step
through with the arrow keys. Watch the cards move from one stack to the other
on `undo` and `redo`, and watch what happens to the redo stack on a new paint.
Answer **Q4** and **Q5**.

> [!TIP]
> Draw your own picture: edit `script.txt` and run `./session` again. The
> commands are listed at the top of `session.cpp`.

### Your tests

Add a function `test_your_cases` with **at least two** checks of your own, and
call it from `main`. At least one must be a case you got wrong, or expected to.

## Submission

Upload these files to Gradescope:

- `canvas.h`
- `canvas.cpp`
- `test_canvas.cpp`
- `answers.md`

> [!WARNING]
> **Upload those four files and nothing else.** The autograder checks this
> before it looks at your code.

Your code should compile clean under `-Wall -Wextra -Werror`.

Questions: ask the instructor or a TA **in the room**, or post on Ed.
