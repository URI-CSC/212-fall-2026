# Assignment 3 report

Name:

Fill in each section, export this file to PDF, and submit it as `report.pdf`.
Each section is worth 5 points. Keep the whole report to about five pages.
Use numbers from your own runs, built with `-O2`. Delete these instructions
before you submit.

## 1. Your reader in action

Three screenshots of the web page, with k = 3:

- a digit of yours that it reads correctly
- a digit of yours that it reads wrong, with its closest training digits
  showing
- the same wrong digit, redrawn so that it reads it correctly

For the wrong one: looking at its closest training digits, why do you think
it got it wrong? What did you change when you redrew it?

## 2. More data, better answers

Fill in the table with `./digits accuracy TRAIN 1000 3`:

| training digits | accuracy | ms per digit |
|---:|---:|---:|
| 1,000 | | |
| 5,000 | | |
| 10,000 | | |
| 30,000 | | |
| 60,000 | | |

How does accuracy change as the training set grows, and how does the time
per digit change? What is the cost of a better answer here? A phone has to
read a digit in about 10 ms: which size would you ship?

## 3. Choosing k

Fill in the table with `./digits accuracy 60000 1000 K`:

| k | accuracy |
|---:|---:|
| 1 | |
| 3 | |
| 5 | |
| 7 | |
| 9 | |

Which k is best on this data? Explain why k = 1 can be fooled by a single
odd training digit, and why a very large k (say 1,000) would also be worse.

## 4. Stopping early

The time of `./digits accuracy 60000 1000 3` before Task 6 and after it:

| | seconds | ms per digit |
|---|---:|---:|
| before | | |
| after | | |

How much faster is it? Explain why the answers cannot change. Early stop
helps more on some digits than on others: on a digit with many very close
training digits (such as a neat 1), or on one with none? Why?

## 5. Where it goes wrong

A screenshot of Part 2 of the page, **Find mistakes**, with k = 3 and the
first 1,000 test digits. Which pair of digits does it confuse most often? Pick
one mistake and explain it in terms of the distance: where does that digit
have ink that the right answer's training digits do not, or the other way
around?

Your reader compares pixels in fixed places. Name one thing a person sees
when reading a digit that a pixel-by-pixel distance cannot see.

## 6. Recursion

With k = 3, start from an empty `NeighborList` and add, in this order, the
distances 30, 80, 10, 30, 5. Draw the list after each `add`. For the fourth
add (the second 30), list the calls to `insert_r` and what each one returns,
and the calls to `trim_r` that follow.

`insert_r` is safe here because the list never holds more than k + 1 nodes,
so the recursion is at most k + 1 calls deep. Suppose instead that you kept
all the training images in one sorted list (k = 60,000). How deep could the
recursion of `insert_r` go, and what would happen? How would you rewrite
`insert_r` so that it works for any k?
