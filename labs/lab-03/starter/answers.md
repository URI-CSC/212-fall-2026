# Lab 03: answers

- **Name:**
- **Group members:**

## Task 1: a minimal class

**Q1.** `main` runs `DynArray a(4);`. Where do the four members of `a` live
(stack or heap)? Where does the block live? What is in its 4 slots? If the
class had no destructor, what would happen to the block when `a` goes away?

>

## Task 2: storing elements

**Q2.** After `DynArray a(4);` and three `push_back` calls, what are
`a.size()` and `a.capacity()`? Slot 3 is allocated memory. Why must
`a.at(3)` throw anyway?

>

## Task 3: growing

**Q3.** Before you run the tests, trace `DynArray a(3);` followed by 10 calls
to `push_back`. One row per push.

| push | size before | full? | copies made | capacity after |
|---|---|---|---|---|
| 1 | 0 | no | 0 | 3 |
| 2 | | | | |
| 3 | | | | |
| 4 | | | | |
| 5 | | | | |
| 6 | | | | |
| 7 | | | | |
| 8 | | | | |
| 9 | | | | |
| 10 | | | | |

Total copies: , total grows: .

## Task 4: pop_back, insert, erase

**Q4.** An array holds $n = 1{,}000{,}000$ elements. How many elements move
for `pop_back()`, for `insert(0, v)`, and for `erase(0)`?

At the end of `test_moving`, the loop removes every 7. Suppose it were written
like this instead:

```cpp
for (size_t i = 0; i < r.size(); i++) {
    if (r.at(i) == 7) {
        r.erase(i);
    }
}
```

What would `r` hold at the end, and why?

>

## Task 5: measure the cost of growing

**Q5.** Fill in the **predicted** column from the formula **before** you run
`./bench 17`. Then fill in the measured copies at $n = 131{,}072$.

| $c$ | predicted copies | measured copies |
|---|---|---|
| 1 | | |
| 10 | | |
| 100 | | |
| 1000 | | |

>

**Q6.** What number does the copies ratio settle on, for every $c$? What
growth does that mean? What did going from $c = 1$ to $c = 1000$ buy you, and
what did it not buy you?

>

## Task 6: copying

**Q7.** When you first ran `test_copying`, before writing the copy functions,
what happened? Which line of the test is the first one that went wrong, and
why?

>

**Q8.** For each line, which function runs: the copy constructor, copy
assignment, or neither?

```cpp
DynArray a(4);
DynArray b = a;      // (1)
DynArray c(a);       // (2)
c = b;               // (3)
DynArray& d = a;     // (4)
```

>
