# Lab 01: answers

- **Name:**
- **Group members:**
- **Machine (CPU, OS, compiler, `g++ --version`):**

## Task 2: before you implemented anything

**How many unordered pairs does an array of $n$ elements have?** (You were asked
to write this down *before* measuring.)

>

## Task 3: the doubling-ratio table

Paste your output of `./bench 15`:

```
(paste here)
```

**Q1.** What number does the brute-force `ratio` column settle on, and what
growth does that name? Does it match the pair count you wrote above? Show the
connection in one line.

>

**Q2.** The two-pointer `ratio` column does **not** settle on a clean 2. It
starts noticeably above 2 and drifts down toward it as $n$ grows. That is not
noise; it is the method telling you something true about the algorithm. What?

> *Hint: the two-pointer function does two things. One of them is the scan.*

>  

**Q3.** Pick **one** of the four `DECISION` comments in `bench.cpp`. Describe
concretely what the table would have looked like if that decision had gone the
other way: a specific wrong number or a specific wrong conclusion, not "it
would be less accurate."

>

## Task 4: predict, then measure

Fill the **predicted** column in completely **before** running `./bench 16`.

| | predicted at $n = 65{,}536$ | actual | off by |
|---|---|---|---|
| brute force (ms) | | | |
| two-pointer (ms) | | | |

**Q4.** One line of reasoning for each prediction. If you were off by more than
about 10%, say what you think happened.

>

## Task 5: the compiler

| $n$ | brute `-O0` (ms) | brute `-O2` (ms) | `-O0` / `-O2` |
|---|---|---|---|
| 8192 | | | |
| 16384 | | | |
| 32768 | | | |

**Q5.** What did `-O2` buy you, and what did it leave untouched? Why can't a
compiler turn the brute-force version into the two-pointer version: what would
it have to understand in order to do that?

>
