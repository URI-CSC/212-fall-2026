# Lab 02: Classes, pointers, and memory 

Today you will write a class for representing images (grids of pixels) and a number of functions to manipulate them.

You need a working `g++` and a terminal. Every file in this lab is plain C++ compiled from the command line, plus one web page that runs in a browser.

> [!CAUTION]
> Turn off AI autocomplete for this lab. How to switch it off, 
> in VS Code and in other editors:
> [Turn off AI autocomplete](../lab-01/setup.md#turn-off-ai-autocomplete).

> [!IMPORTANT]
> **Work inside `starter/`.** Everything you write goes there, and every command
> in this handout runs from that one directory. Start by going there and staying
> there:
>
> ```bash
> $ cd starter
> ```

## Task 1: PGM files

A Portable Graymap (PGM) file is a plain text (ASCII) format representing a grayscale image.  It starts with magic number `P2`, followed by image width, height, maximum gray value, and space-separated ASCII decimal pixel values ranging from 0 (black) to the max value (white).

Here is an example of a PGM file:

```bash
$ cat images/example.pgm
```

```
P2
4 3
255
10 100 2 3
4 50 6 70
188 9 13 11
```

The file above contains `P2` followed by **4 columns**, **3 rows**, 
then `255`, the largest value a pixel may have. After that come
$4 \times 3 = 12$ numbers, one per pixel, 0 for black and 255 for white.

The "matrix" of pixels can be viewed as a 1D array of 12 integers. 
The newlines are only there for your eyes. 

```
cols = 4, rows = 3

       |------ row 0 ------|------ row 1 ------|------ row 2 ------|
       +----+----+----+----+----+----+----+----+----+----+----+----+
pixel  | 10 |100 |  2 |  3 |  4 | 50 |  6 | 70 |188 |  9 | 13 | 11 |
       +----+----+----+----+----+----+----+----+----+----+----+----+
index     0    1    2    3    4    5    6    7    8    9   10   11
                                             ^
                                             row 1, column 3
```

To find a value a row `r` and column `c`, using the 1D representation, 
we need to calculate the `index` in the 1D array that corresponds to 
row `r` and column `c` in the 2D view. The steps are: skip `r` whole 
rows, which is `r * cols` numbers. Then walk `c` more:

$$\text{index} = r \cdot \text{cols} + c$$

For the example file above, the pixel value at row 1, column 3 is 70,
and can be found at index $1 \cdot 4 + 3 = 7$.

Now open `../viewer/index.html` in your browser (double-click it) and drop
`images/banner.pgm` onto the page. Hover over the picture: it shows you the
row, the column, the index, and the value under your cursor.

Answer **Q1** and **Q2** in `answers.md`.

> [!NOTE]
> The viewer runs entirely in your browser and never uploads anything. There is
> no server and no install.
>
> You can also open a `.pgm` straight from Preview on macOS, or with any image
> viewer on Linux. Windows has no built-in PGM viewer.

## Task 2: The `Image` class

**Read `image.h` before you write anything.** It is the contract, and two of
the tests depend on details in it.

Open `image.cpp` and fill in the first six methods:

- the **constructor**, which stores the sizes, allocates `rows * cols` ints
  with `new []`, and sets them all to 0
- the **destructor**, which is one line, and is the most important line in the
  file
- `get_rows` and `get_cols`
- `at` and `set`, which turn `(r, c)` into one index, and which throw
  `std::out_of_range` when the pixel is outside the image

> Why `new []`? Because you do not know `rows`
> and `cols` when you write the code. They arrive when the program runs, out
> of a file somebody hands you. An array whose size is decided at runtime has
> to come from the **heap**, and whatever you take from the heap, you give back.

Build and run the tests:

```bash
$ g++ -std=c++17 -Wall -Wextra -g image.cpp pgm.cpp test_image.cpp -o test_image
$ ./test_image
```

You get one line per failing check, with the expression and its line number.
Keep going until you see `ALL TESTS PASSED`.

> **Why three `.cpp` files on one command line?** `image.cpp` and `pgm.cpp`
> have no `main`; `test_image.cpp` has one. `g++` compiles all three and links
> them into a single program.

Read the tests to understand what is expected.

Answer **Q3** in `answers.md`.

## Task 3: Three transformations

Fill in `invert`, `flip_horizontal`, and `transpose`. 
- `invert` replaces every pixel with `255 - pixel`.
- `flip_horizontal` mirrors the picture left to right: 
column `c` trades places with column `cols - 1 - c`.
- `transpose` turns row `r` into column `r`. `banner.pgm` is 160 rows by 240
columns, and its transpose is 240 by 160.
    1. allocate a NEW array of *rows * cols* ints
    2. copy each pixel to its new home: (r, c) goes to (c, r)
    3. delete [] the OLD array
    4. point data at the new array, and swap rows with cols

Now look at what you built:

```bash
$ g++ -std=c++17 -Wall -Wextra -Werror image.cpp pgm.cpp apply.cpp -o apply
$ ./apply images/banner.pgm invert inverted.pgm
$ ./apply images/banner.pgm flip flipped.pgm
$ ./apply images/banner.pgm transpose sideways.pgm
```

Drop all three on the viewer page, next to the original.

Answer **Q4** in `answers.md`.

## Interlude: counting without searching

Here is a question about `banner.pgm`: **which gray value appears most often?**

The obvious approach is to try each of the 256 possible values and count how
many pixels match. That is 256 passes over 38,400 pixels, about 9.8 million
comparisons.

Now do it the other way. Keep 256 counters, walk the image **once**, and for
each pixel do

```cpp
counts[pixel]++;
```

38,400 operations, no searching and no comparing. The pixel value **is** the
index. Nothing is being looked up, because the answer is already sitting at
that address.

> [!IMPORTANT]
> It works whenever your keys are small integers with no gaps: instead of
> searching for the key, you use the key as a subscript. Marking and testing
> cost one memory access each.

## Task 4: The histogram

Fill in the last three.

- `histogram(int *counts)` fills a 256 entry array the caller owns. Zero the
entries first; the caller's memory holds whatever was there before.
- `most_common_value()` allocates its own 256 array, calls `histogram`, finds
the largest count, and **frees the array before returning**. On a tie, return
the smaller value.
- `auto_contrast()` finds the darkest value `lo` and the brightest value `hi`
that actually occur, then stretches that range across the full scale:

$$\text{new} = \frac{(\text{old} - lo) \cdot 255}{hi - lo}$$

If `hi == lo` the image is one flat color. Leave it alone, and do not divide
by zero.

Now the payoff:

```bash
$ ./apply images/banner.pgm hist
$ ./apply images/banner.pgm contrast sharp.pgm
```

Open `sharp.pgm` in the viewer, beside `banner.pgm`.

`banner.pgm` is a gray fog. Every one of its pixels sits between 96 and 152,
a band 57 levels wide out of 256, which is why it looks like nothing. The
histogram shows you exactly that: bars bunched in the middle and empty space
on both sides. Then `contrast` spreads that band across the full 0 to 255, and
the message that was there the whole time comes out of the fog.

Nothing was added. The information was always in the file. It was just
squeezed into a range too narrow for your eyes.

Answer **Q5** in `answers.md`.

## Task 5: Bug hunt

`leaky.cpp` is somebody else's code. It compiles clean, it prints the right
answers, and it leaks. It counts arrays for you and reports the damage:

```bash
$ g++ -std=c++17 -Wall -Wextra -Werror leaky.cpp -o leaky
$ ./leaky
```

```
end: 5 arrays alive
```

Five arrays leaked, from **three** distinct mistakes. Find all three, fix
them, and get that last line to `0`. Do not change `main`.

> **Where to look.** Every `new []` in the file has to be matched by exactly
> one `delete []`, on every path out of the function. Read each function and
> ask: if I take this branch, who frees this?

Answer **Q6** in `answers.md`, and submit your fixed `leaky.cpp`.

## If you have time: copy an image and watch it break

Add this to `test_your_cases` and run it:

```cpp
Image original(4, 4);
original.set(0, 0, 200);
Image copy = original;        // no error, no warning
copy.set(0, 0, 7);
CHECK(original.at(0, 0) == 200);   // is it?
```

Your class holds a raw pointer, and you never told C++ how to copy it, so it
guessed: it copied the **pointer**, not the pixels. Both objects now share one
array, both destructors will free it, and the second free is a crash you will
not enjoy debugging.

> Your test program will die at the end of that function instead of printing
> its summary. That **is** the demonstration. Take the snippet back out before
> you submit.

```bash
$ g++ -std=c++17 -g -fsanitize=address image.cpp pgm.cpp test_image.cpp -o test_asan
$ ./test_asan
```

The fix is called the **rule of three**, and it is Task 6 of Assignment 1.
Today, just make sure you have seen it break.

## Submission

Upload the following files to Gradescope:

- `image.cpp`
- `test_image.cpp` (with the two checks you added to `test_your_cases`)
- `leaky.cpp` (fixed)
- `answers.md`

> [!WARNING]
> **Upload the four files themselves, and nothing else.** The autograder
> checks this before it looks at your code.

Your code should compile clean under `-Wall -Wextra -Werror` and be readable.

Questions: ask the instructor or a TA **in the room**, or post on Ed.
