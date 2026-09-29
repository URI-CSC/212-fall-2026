# Lecture Style Guide (CSC 212)

This file collects the style rules for lecture files written in Markdown.

## Goal of a lecture

- The lecture must be **stunning, focused, and based on Computer Science principles, coherent**: teach
  the idea behind the mechanism, not a tour of features. 
- The textbook reference is Data Structures and Algorithms in C++ by Michael T. Goodrich (Author), Roberto Tamassia (Author), David M. Mount (Author) 

### Structure of the Markdown

- Organize with **sections**: the title slide is `#`, every slide is `##`.
- Use **enumerated lists** (`1.`) and **itemized lists** (`-`), with
  **sublists** nested 4 spaces per level.
- **Figures** are referenced only by image file name, stored in an `images/`
  folder next to the Markdown:
  `![slide04_01.jpg](images/slide04_01.jpg)`
- Images are small: JPEG, max 800 px on the longest side, quality about 60.
- **Math is written in LaTeX**: `$...$` inline, `$$...$$` for display.
- Code goes in fenced code blocks.
- Tables are Markdown tables.
- **Speaker notes are included**, as `> [NOTES]` blockquotes after the slide content.
- **Hidden slides** are always included, with `(HIDDEN)` after the title:
  `## Additional slides (HIDDEN)`
- Text boxes contribute **only their text**. Decorative images (shadows,
  backdrops, empty boxes) are never saved or referenced.
- A human will convert the MD into a keynote file, so please all MATH latex should be compatible with apple keynote

## 2. Standing rules for all course materials

- **Plain ASCII only.** No character above U+007F.
    - no em dashes: rewrite with a comma, period, or colon
    - `->` for arrows, `x` for multiplication, `...` for ellipsis
    - straight quotes `'` and `"` only
    - no emoji
    - math symbols (approx, <=, >=, Theta) are written in LaTeX, which is
      ASCII source
- **Terminal commands** are prefixed with `$ ` (PowerShell uses `PS> `).
  Program output and comment lines are not prefixed.
- **Callouts, Mermaid, and LaTeX** may be used freely in lab handouts and
  in lectures (`> [!NOTE]`, `> [!TIP]`, `> [!IMPORTANT]`, `> [!WARNING]`,
  `> [!CAUTION]`, ` ```mermaid `). Keep Mermaid uncolored so it reads in
  light and dark themes.
    - in lectures, use callouts for named principles (`> [!IMPORTANT]`) and
      common pitfalls (`> [!WARNING]`)
    - use Mermaid when the idea is structural: memory layouts, pointers,
      before/after states of an operation

## 3. Conventions

### Graphics and illustrations

- **Illustrate whenever possible.** If an idea has a shape (a layout, a
  sequence of states, a growth curve), show it; do not only describe it.
  Most content slides should carry a graphic.
- Pick the tool by the kind of idea:

| idea | tool |
|---|---|
| memory layout, pointers, ownership | Mermaid `flowchart` or `block-beta` |
| step-by-step state change (before -> after) | Mermaid, or aligned text diagrams, one per step |
| array cells with indices, size vs capacity | aligned text diagram in a plain code block |
| growth curves, cost vs n, measured timings | Mermaid `xychart-beta` for a few points; matplotlib JPEG for real data |
| control flow, decision logic | Mermaid `flowchart` |
| a formula or derivation | LaTeX display math |
| a photo or screenshot | JPEG in `images/` |

- Aligned text diagrams use a plain fenced block, ASCII only, e.g.:

  ```
  index:     0   1   2   3   4   5   6   7
           +---+---+---+---+---+---+---+---+
  data:    | 4 | 3 | 9 | 1 |   |   |   |   |
           +---+---+---+---+---+---+---+---+
                           ^ size = 4       ^ capacity = 8
  ```

- Mermaid: uncolored (default theme), short labels, one idea per diagram,
  so it reads in light and dark themes.
- Generated plots (matplotlib, already installed) are saved as small JPEGs in
  `images/` with descriptive names (`growth-doubling.jpg`), following the
  image rules above.
- A graphic replaces text, not duplicates it: keep only the bullets the
  picture cannot say.

### Opening

- Title slide: `# NN: Title`, followed by `- Fall 2026`.
- Second slide bridges from the previous lecture: `## From last lecture ...`
  or `## Where we are?`, ending with a one-line **Today:** goal.
    - example: "Last lecture we learned to count; today we learn what to
      throw away"

### Teaching arc

- One **running example** carries the lecture and is solved several ways
  (lecture 04: 2-sum by brute force, sort + two pointers, hash table).
- Each idea is motivated by a problem before it is named.
- Claims are made **quantitative**: concrete sizes, times, and ratios
    - "brute force is **25,000x slower** than sorting here"
    - "$2^{100}$ operations at 1 ns each is about $4 \times 10^{13}$ years"
- **Named principles.** Each section builds to one explicitly stated
  principle, written as a callout:

  ```
  > [!IMPORTANT]
  > **Principle: grow geometrically, never arithmetically.**
  ```

    - a principle is one sentence, general enough to outlive the example
    - e.g. "No hardware improvement rescues an exponential algorithm."
- A closing slide, `## Principles`, lists every principle from the lecture in
  order, one line each.

### In-class questions

- A question follows **each idea**, about every 3-5 content slides
  (lecture 04 has 12).
- Numbered `## QN-k` (N is the lecture number, k counts up), e.g. `## Q4-3`.
- Multiple choice, usually 4 options plus `None of the above`, as an
  enumerated sublist under the question.
- Each question is followed by an `## Answer` slide:
    - the letter and the option text first, e.g. `- C) About 8 minutes`
    - then the reason, as sub-bullets
    - then the supporting calculation in display math

### Content slides

- Bullets are short phrases, lower-case after the first level, no periods.
- **Bold** marks the key term the first time it appears.
- Code is C++ (`std::` qualified), short, one idea per listing.
- Comparisons are tables (e.g. the seven growth functions, with names and
  example algorithms).
- Speaker notes appear **only where needed**: context the slide cannot show,
  such as a caveat, an anecdote, or the reason behind a number.

### Closing

- `## Principles` slide (see Teaching arc).
- Extra or backup material goes at the end under
  `## Additional slides (HIDDEN)`, with each extra slide marked `(HIDDEN)`.
