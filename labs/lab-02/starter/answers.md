# Lab 02: answers

- **Name:**
- **Group members:**

## Task 1: PGM files

**Q1.** In `example.pgm` (4 columns, 3 rows), which index of the pixel array
holds row 2, column 2? What value is stored there? Show the arithmetic.

>

**Q2.** `banner.pgm` has 240 columns and 160 rows. How many numbers follow the
header? Now suppose you wrote your index as `c * rows + r` instead of
`r * cols + c`. Describe what the picture would look like.

>

## Task 2: the class

**Q3.** `test_no_leaks` is the only test in the file that can fail because of a
line you did **not** write. What does it check that no other test can see, and
what exactly would go wrong in a long running program if it failed?

>

## Task 3: three transformations

**Q4.** `flip_horizontal` swaps pixels where they sit. `transpose` cannot. Why
not? Name the one thing about `transpose` that makes the difference.

>

## Task 4: the histogram

Paste the output of `./apply images/banner.pgm hist`:

```
(paste here)
```

**Q5.** Every pixel of `banner.pgm` sits between 96 and 152. How many levels
wide is that band, out of the 256 available? After `auto_contrast`, was any
information added to the picture? Answer in one line, and say what did change.

>

## Task 5: bug hunt

**Q6.** Name the three mistakes in `leaky.cpp`. For each one, give the function
it is in and the line you added or changed.

> 1.
> 2.
> 3.

## If you got to the stretch task

**Q7.** What did `Image copy = original;` actually copy, and what did you
expect it to copy?

>
