# Lab 04: answers

- **Name:**
- **Group members:**

## Task 1: painting

**Q1.** `m_pixels` is a plain 2D array. What would `get(0, 0)` return if the
constructor did not set every pixel to `'.'`? Which of these calls throw?

```cpp
cv.get(15, 15);       // (1)
cv.get(16, 0);        // (2)
cv.paint(0, -1, 'r'); // (3)
cv.paint(3, 16, 'r'); // (4)
```

>

## Task 2: undo

**Q2.** Pixel `(4, 7)` is painted `'r'`, then `'g'`, then `'b'`. After one
`undo()`, what color is it, and where does `undo()` find that color? Why would
an `Edit` with only `row`, `col` and `after` not be enough?

>

## Task 3: redo

**Q3.** Before you run anything, trace this on paper. After each line, write
the color of pixels `(0, 0)` and `(1, 1)`, and both stacks, top first. Write
an edit as `row col before after`, the way `print` will.

```cpp
Canvas cv;
cv.paint(0, 0, 'r');  // (1)
cv.paint(0, 0, 'g');  // (2)
cv.paint(1, 1, 'b');  // (3)
cv.undo();            // (4)
cv.undo();            // (5)
cv.paint(0, 0, 'y');  // (6)
```

| line | (0, 0) | (1, 1) | undo stack, top first | redo stack, top first |
|---|---|---|---|---|
| (1) | `r` | `.` | `0 0 . r` | empty |
| (2) | | | | |
| (3) | | | | |
| (4) | | | | |
| (5) | | | | |
| (6) | | | | |

Line (6) clears the redo stack. Suppose it did not. What would one `redo()`
after line (6) do to `(0, 0)`? And one `undo()` after that? Why is that
wrong?

>

## Task 4: print and watch

**Q4.** `print_stack` takes its stack **by value**. What happens if you change
the parameter to `const std::stack<Edit>& s`? `push`, `pop` and `top` cost
$O(1)$. What does one call to `print()` cost, with $n$ edits on the stacks?

>

**Q5.** In the viewer, step through the script:

- Just before `paint 5 5 b`, the redo stack held the two red nose paints.
  What happened to them? Why does the `redo` a few steps later print
  `nothing to redo`?
- `row 10 3 yykkkkkkyy` pushed one edit per pixel. How many `undo` calls does
  it take to remove that row? A real drawing program removes a whole stroke
  with one undo. What would you push on the stacks instead of single edits?

>
