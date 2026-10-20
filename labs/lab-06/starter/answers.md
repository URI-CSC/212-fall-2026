# Lab 06: answers

- **Name:**
- **Group members:**

## Task 1: a list of tokens

**Q1.** A `Node` holds an `int` (4 bytes) and a pointer (8 bytes). With
padding, one node takes 16 bytes. A `std::string` takes 1 byte per character.
`shakespeare.txt` has 1,115,394 characters. About how many bytes does a
`TokenList` of it take, and how many times more is that than the string?

>

## Task 2: the most frequent pair

**Q2.** Before you run anything, count the pairs of `banana` on paper. Which
pair does `most_frequent_pair()` return, with what count? Two pairs tie: why
does this one win?

| pair | count |
|---|---|
| `b a` | |
| | |
| | |

>

## Task 3: merge

**Q3.** What does `TokenList("banana")` hold after `merge('a', 'n', 256)`?
Write its tokens and the number of nodes removed.

To delete a node from a singly linked list, you need a pointer to the node
**before** it. Why does `merge` never need one, or a `prev` pointer?

>

## Task 4: decode

**Q4.** Look at the table for `decode_token(259)` in the handout. What is the
largest number of tokens on the stack at any time?

Suppose `decode_token` pushed the **left** part first, then the right part.
What would `decode_token(259)` return?

Suppose a bug saves `rules[0] = {256, 'a'}`: token 256 is made of itself. What
does `decode_token(256, rules)` do?

>

## Task 5: train, and watch it

**Q5.** In the viewer, with `tiny.txt` and 30 merges:

- How many nodes does the first merge remove, and what is the new token?
- At which merge does all of `First Citizen:\n` become one token? Its number?
- The merge that makes `Firs` removes 5 nodes, but `tiny.txt` has only 4
  lines that say `First Citizen:`. Find the fifth `Firs` in the text.

>

## Task 6: why a linked list

**Q6.** Paste the table that `./bench shakespeare.txt` printed.

```text

```

When $n$ doubles, about how many times longer does the list take? The vector?
What does that say about the growth of each one, as in Lab 01?

The `removed` column doubles too. One `erase` in the middle of a vector of $n$
tokens moves about $n/2$ of them. Use both facts to explain the vector's
column.

>
