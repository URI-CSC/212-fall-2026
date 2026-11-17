# Lab 09: answers

- **Name:**
- **Group members:**

## Task 1: a set of strings

**Q1.** Look at the tree of Task 1. How many nodes does `contains("monkey")`
compare with? And `contains("zebra")`, which is not in the tree?

Draw the tree you get if you insert the same seven passwords in a different
order: `admin`, `letmein`, `monkey`, `password`, `qwerty`, `sunshine`,
`123456`. What is its height?

>

## Task 2: attack your tree

**Q2.** Copy your output of `./attack` here.

In the sorted column, when $n$ doubles, about how much longer does the run
take? Explain that ratio from what each insert does in a chain.

Who chooses the order of the keys that go into a tree on a real server? Why
is `std::set` safe from this attack?

>

## Task 3: catch a password-guessing attack

**Q3.** Which two IP addresses did `./detect auth.log 5` find? Look at their
lines in `auth.log`: what is each of them doing? Which one would a limit on
failures **per user name**, instead of per IP address, miss?

`192.0.2.9` is printed after `192.0.2.88`. Why?

>
