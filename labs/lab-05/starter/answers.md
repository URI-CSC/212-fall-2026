# Lab 05: answers

- **Name:**
- **Group members:**

## Task 1: brute force

**Q1.** With $n$ values and a window of $k$, how many windows does
`max_brute` scan, and how many values does it look at in each? Write the total
in terms of $n$ and $k$. `hourly.txt` has $n = 175{,}320$. About how many
values does `max_brute` look at for a window of one year, $k = 8{,}760$?

>

## Task 2: a deque that holds the window

**Q2.** Paste the table that `./bench hourly.txt` printed.

```text

```

Your `max_deque` uses a deque, and it is not faster than `max_brute`. Why?
What does one call to `max()` cost, with a window of $k$?

>

## Task 3: the staircase

**Q3.** Before you run anything, trace this on paper. After each push, write
the deque, front first, as `index:value`, and what `max()` returns.

```cpp
WindowMax w(3);
w.push(5);  // (1)
w.push(3);  // (2)
w.push(4);  // (3)
w.push(1);  // (4)
w.push(6);  // (5)
w.push(2);  // (6)
```

| push | deque, front first | max() |
|---|---|---|
| (1) | `0:5` | 5 |
| (2) | | |
| (3) | | |
| (4) | | |
| (5) | | |
| (6) | | |

Then paste the new benchmark table.

```text

```

One push can pop many entries from the back, so one `push` is not always
$O(1)$. Still, every index is pushed **once** and popped **at most once**.
What is the total cost of `max_deque` on $n$ values? Does it depend on $k$?

>

**Q4.** In `push`, change `<=` to `<` and run your tests. Which checks fail?
Are the answers of `max_deque` still right? What is kept in the deque that
did not need to be? Change it back.

>

## Task 4: print and watch

**Q5.** In the viewer, with `k = 6`:

- Find a step where one push pops two or more entries from the back. Is the
  temperature going up or down around it? When the temperature keeps falling,
  what happens to the size of the deque, and why?
- In Lab 04, `print_stack` had to take a **copy** of the stack to print it.
  Why can `print` here walk through `m_dq` directly?

>
