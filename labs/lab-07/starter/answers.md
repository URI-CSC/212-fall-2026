# Lab 07: answers

- **Name:**
- **Group members:**

## Task 1: build and count

**Q1.** Before you open the viewer: how many calls does `size` make on the
list `3 -5 6 18`? At the moment the most frames are on the stack, how many
calls of `size` are there?

Now step through `size.log` in the viewer. Were you right? Which call
returns first, and what does it return? Why does `size` need a call on
`nullptr` at all?

>

## Task 2: down and up

**Q2.** In the viewer, with the list `1 2 3 4`:

- With `print`, how many frames of recursion are on the stack when the first
  number appears?
- With `print_back`, how many, and which call is running?

Explain the difference in one or two sentences, using "on the way down" and
"on the way back up".

>

## Task 3: return the repaired rest

**Q3.** In the viewer, with `remove_first 6 3 -5 6 18`: at the step where node
`6` turns red, which node still points at it? On which step does that
change, and which line of your `remove_first` makes the change?

Suppose a student writes `clear` with the two lines swapped: `delete p;` first,
then `clear(p->next);`. What goes wrong?

>

## Task 4: the call tree is the cost

**Q4.** Fill in the number of calls, using `./watch ... > /dev/null`:

| function | input | calls |
|---|---|---|
| `power` | `2 10` | |
| `power` | `2 1000` | |
| `power_2` | `2 10` | |
| `power_2` | `2 1000` | |
| `max_slow` | `1..5` | |
| `max_slow` | `1..10` | |
| `max_fast` | `1..10` | |

When $n$ grows 100 times, how do the calls of `power` grow? And of `power_2`?

Look at the call tree of `max_slow 1..5` in the viewer. How many calls does
each call make, on an increasing list? Each extra node does what to the number
of calls? Why does `max_fast` not have this problem?

>
