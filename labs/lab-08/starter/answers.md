# Lab 08: answers

- **Name:**
- **Group members:**

## Task 1: a priority queue of ints

**Q1.** With $n$ elements in your `IntPQ`, what is the cost of `push`, of
`top`, and of `pop`, in $\Theta$?

`pop` moves the last element into the place of the one it removes. Why is
that allowed in `IntPQ`? What would go wrong if `m_data` had to stay sorted?

>

## Task 2: a function template

**Q2.** What does the compiler say about `larger(3, 2.5)`? Which two types
did it find for `T`?

What does `larger<double>(3, 2.5)` print, and why does it compile?

>

## Task 3: a class template

**Q3.** `PQ<Request>` with only `push` compiles. With `requests.top()` it
does not. Which line of `pq.h` does the error point at, and what operation is
missing?

Why does `push` compile, but `top` does not? What must a type `T` have for
every operation of `PQ<T>` to compile?

>

## Task 4: a comparator

**Q4.** `std::priority_queue<int, std::vector<int>, std::greater<int>>` has
three template arguments. For each one, say what it does, and which part of
your `PQ` plays the same role (one of them has no match in your `PQ`).

Why must `operator()` in `ByPriority` be `const`?

>

## Task 5: how fast is it?

**Q5.** Copy your output of `./bench` here.

When $n$ doubles, about how much longer does your `PQ` take? What does that
ratio tell you about how its total time grows with $n$? Explain it from what
each `pop` does.

`std::priority_queue` grows much more slowly. Use your 80,000 row to predict
the time of each queue for $n = 1{,}000{,}000$ (12.5 times more elements).

>
