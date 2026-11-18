# Lab 09: Ordered sets and maps

Every month, software vendors ship fixes for hundreds of security bugs, and
some of those bugs are used by attackers before the fix is out. Many of them
have the same cause: the code trusted its input. Today you build a *set* of
strings backed by a **binary search tree**, and then you attack it, with nothing but
the **order** of its input. At the end you use `std::set` and `std::map` to
catch an attacker who is guessing passwords.

> [!CAUTION]
> Turn off AI autocomplete for this lab. How to switch it off,
> in VS Code and in other editors:
> [Turn off AI autocomplete](../lab-01/setup.md#turn-off-ai-autocomplete).

**What you write.** You work in `starter/` and write three files there:
`security.h`, `security.cpp` and `test_security.cpp`. Do not change the other
files. They are tools for you:

- `check.h`: a small test harness, the same as in Lab 08
- `attack.cpp`: attacks your tree (Task 2)
- `detect.cpp` and `auth.log`: run your detector on a log file (Task 3)
- `answers.md`: the questions of this lab

**How you work.** Each task adds a little to `security.h` and
`security.cpp`, and a test to `test_security.cpp`. Every task ends the same
way. Compile and run the tests:

```bash
$ cd starter
$ g++ -std=c++17 -Wall -Wextra -g security.cpp test_security.cpp -o test_security
$ ./test_security
```

A failing check prints its line number. Do not start the next task until you
see `ALL TESTS PASSED`.

## Task 1: A set of strings

### Idea

A **set** is a collection that holds each key at most once. Its main question is: is this key in
the set? A website asks it every time a user picks a new password: is it on
the list of passwords that attackers try first?

You keep the keys in a **binary search tree** (BST). For every node, the keys
in its left subtree are smaller, and the keys in its right subtree are larger.
For strings, "smaller" means "comes first in alphabetical order", which is
what `<` does on `std::string`. Digits come before letters, so `"123456" <
"admin"`.

Here is the tree you get when you insert `password`, `123456`, `qwerty`,
`letmein`, `admin`, `monkey`, `sunshine`, in this order:

```text
                 password
                /        \
           123456        qwerty
                \             \
              letmein       sunshine
              /     \
          admin    monkey
```

To find a key, start at the root. If the key is smaller, go left; if it is
larger, go right; if it is equal, you found it. If you fall off the tree
(`nullptr`), the key is not there. Insert walks the same path and hangs a new
node where the walk falls off. If the key is already there, insert does
nothing.

Each operation has a public **wrapper** that calls a private recursive **helper**
on the root. The helper works on the subtree rooted at `p`.

### Declare

Create `security.h`:

```cpp
#ifndef SECURITY_H
#define SECURITY_H

#include <cstddef>
#include <string>

// Task 1: one node of the tree.
struct Node {
    std::string key;
    Node* left;
    Node* right;
    Node(const std::string& k) : key(k), left(nullptr), right(nullptr) {}
};

// Task 1: a set of strings, kept in a binary search tree.
class StrSet {
    private:
        Node* m_root; // nullptr when the set is empty
        std::size_t m_size; // the number of keys

        static bool contains_helper(Node* p, const std::string& key);
        Node* insert_helper(Node* p, const std::string& key);
        static void clear_helper(Node* p);

    public:
        StrSet() : m_root(nullptr), m_size(0) {}
        ~StrSet() { clear_helper(m_root); }

        void insert(const std::string& key);
        bool contains(const std::string& key) const;
        std::size_t size() const { return m_size; }
};

#endif
```

### Implement

Create `security.cpp`:

```cpp
#include "security.h"

// ------------------------------- Task 1 ------------------------------------

void StrSet::insert(const std::string& key) {
    m_root = insert_helper(m_root, key);
}

Node* StrSet::insert_helper(Node* p, const std::string& key) {
    // TODO 1: if p is nullptr, add 1 to m_size and return a new Node(key)
    // TODO 2: if key < p->key, insert into the left subtree:
    //         p->left = insert_helper(p->left, key);
    // TODO 3: else if key > p->key, do the same on the right
    // TODO 4: return p (a duplicate gets here and changes nothing)
}

bool StrSet::contains(const std::string& key) const {
    return contains_helper(m_root, key);
}

bool StrSet::contains_helper(Node* p, const std::string& key) {
    // TODO 1: if p is nullptr, return false
    // TODO 2: if key < p->key, return the answer of the left subtree
    // TODO 3: if key > p->key, return the answer of the right subtree
    // TODO 4: otherwise key == p->key: return true
}

void StrSet::clear_helper(Node* p) {
    // TODO 1: if p is nullptr, return
    // TODO 2: clear the left subtree, then the right subtree
    // TODO 3: delete p (last: after this line, p->left is gone)
}
```

`insert_helper` returns the root of the subtree after the insert, and the
caller stores it back. Only the empty link at the bottom really changes. The
empty tree needs no special case: `m_root` is stored back the same way.

### Test

Create `test_security.cpp`. The includes at the top are all you need for the
whole lab.

```cpp
#include <map>
#include <set>
#include <sstream>
#include <string>

#include "check.h"
#include "security.h"

static void test_strset() {
    section("task 1: StrSet");
    StrSet blocked;
    CHECK(blocked.size() == 0);
    CHECK(!blocked.contains("password"));

    blocked.insert("password");
    blocked.insert("123456");
    blocked.insert("qwerty");
    blocked.insert("letmein");
    blocked.insert("admin");
    blocked.insert("monkey");
    blocked.insert("sunshine");
    CHECK(blocked.size() == 7);
    CHECK(blocked.contains("password"));
    CHECK(blocked.contains("123456"));
    CHECK(blocked.contains("monkey"));
    CHECK(blocked.contains("sunshine"));
    CHECK(!blocked.contains("Password")); // upper and lower case differ
    CHECK(!blocked.contains("correct horse battery staple"));

    blocked.insert("qwerty"); // already there: nothing changes
    CHECK(blocked.size() == 7);
}

int main() {
    std::cout << "running tests\n";
    test_strset();
    return summary();
}
```

Compile and run until you see `ALL TESTS PASSED`. Then answer **Q1** in
`answers.md`.

## Task 2: Attack your tree

### Idea

The **height** of a tree is the number of links on its longest path from the
root down. The tree of Task 1 has height 3: `password`, `123456`, `letmein`,
`admin`. The empty tree has height -1, and a single node has height 0.

`contains` and `insert` walk one path from the root, so they cost
$O(h)$, where $h$ is the height. When keys arrive in random order, $h$ stays
close to $\log_2 n$. But the tree does not choose the order. **Whoever sends
the keys does.**

Insert `a`, `b`, `c`, `d`, `e`, in sorted order. Each key is larger than all
the others, so it always goes right:

```text
   a
    \
     b
      \
       c
        \
         d
          \
           e
```

The tree is now a linked list of height $n - 1$, and each insert walks all of
it. An attacker who signs up 20,000 accounts named `user000001`,
`user000002`, ... in this order does exactly that to a site that keeps its
user names in your `StrSet`. The server slows down for everyone. This is a
**denial of service** attack, and the input was 100% valid.

The height of a tree is 1 plus the larger height of its two subtrees:

```text
height(p) = -1                                          if p is nullptr
height(p) = 1 + max(height(p->left), height(p->right))  otherwise
```

### Declare

In `security.h`, add to the private part of `StrSet`:

```cpp
        // Task 2
        static int height_helper(Node* p);
```

and to the public part:

```cpp
        // Task 2
        int height() const;
```

### Implement

Add `#include <algorithm>` (for `std::max`) below `#include "security.h"` in
`security.cpp`, then add:

```cpp
// ------------------------------- Task 2 ------------------------------------

int StrSet::height() const {
    return height_helper(m_root);
}

int StrSet::height_helper(Node* p) {
    // TODO 1: if p is nullptr, return -1
    // TODO 2: return 1 + the larger of the heights of p->left and p->right
}
```

### Test

Add this test, and call `test_height();` in `main`, after `test_strset();`:

```cpp
static void test_height() {
    section("task 2: height");
    StrSet s;
    CHECK(s.height() == -1); // empty
    s.insert("m");
    CHECK(s.height() == 0); // one node
    s.insert("f");
    s.insert("t");
    CHECK(s.height() == 1); // a balanced tree of 3

    StrSet chain;
    for (std::string k : {"a", "b", "c", "d", "e"}) {
        chain.insert(k); // sorted order
    }
    CHECK(chain.height() == 4); // a chain of 5
}
```

Compile and run until you see `ALL TESTS PASSED`.

### Run the attack

`attack.cpp` makes $n$ user names and inserts them three times: into your
`StrSet` in random order, into your `StrSet` in sorted order, and into a
`std::set<std::string>` in sorted order. `-O2` tells the compiler to optimize
the code. Always use it when you measure speed:

```bash
$ g++ -std=c++17 -O2 security.cpp attack.cpp -o attack
$ ./attack
```

It takes a few seconds. On one laptop it printed this. Your times will differ,
but the heights should be close:

```text
       n |   height  random ms |   height  sorted ms |    std::set ms
    2500 |       26       0.71 |     2499      53.94 |           0.83
    5000 |       26       1.39 |     4999     208.87 |           1.60
   10000 |       28       3.41 |     9999     797.03 |           4.16
   20000 |       36       7.08 |    19999    3466.85 |           7.63
```

> [!NOTE]
> On a slow computer the last row can take 10 seconds or more. If it takes
> too long, stop it with `Ctrl+C`, open `attack.cpp`, change `n <= 20000` to
> `n <= 10000` in the `for` loop of `main`, then compile and run again.

`std::set` got the same sorted names and did not slow down. It is a BST too,
but a **balanced** one (a red-black tree): after each insert it reshapes
itself, so its height is $O(\log n)$ for **any** order of input. The attacker
cannot pick a bad order, because there is none.

Answer **Q2** with **your** numbers.

## Task 3: Catch a password-guessing attack

### Idea

`auth.log` records every login attempt on a server, one per line: the time,
the IP address it came from, the user name, and `OK` or `FAIL`:

```text
2026-11-17T09:30:00 203.0.113.7 admin FAIL
2026-11-17T09:30:02 203.0.113.7 admin FAIL
2026-11-17T09:30:04 203.0.113.7 admin FAIL
```

A person who mistypes a password fails once or twice. A program that guesses
passwords fails again and again, from the same address. To find it, count
the failures of each IP address, and flag every address with too many.

You do this with the two ordered containers of the standard library. Both are
balanced BSTs, so they are safe from the attack of Task 2:

- `std::set<K>`: each key at most once, like your `StrSet`
- `std::map<K, V>`: each key at most once, and each key has a **value**

```text
std::map<std::string, int> failures

  key (IP address)    value (failures)
  ----------------    ----------------
  192.0.2.15          1
  203.0.113.250       9
  203.0.113.7         14
```

The operations you need:

| Code | What it does |
|------|--------------|
| `m[key]` | the value of `key`; if `key` is not there, it is **added** first, with value `0` |
| `m.at(key)` | the value of `key`; throws `std::out_of_range` if `key` is not there |
| `m.count(key)` | `1` if `key` is there, `0` if not |
| `for (const auto& entry : m)` | every entry, **in key order**; `entry.first` is the key, `entry.second` the value |
| `s.insert(key)` | adds `key` to a set; does nothing if it is already there |

Because `m[key]` adds a missing key with value `0`, counting is one line:
`++failures[ip];`. The first failure of an address creates it with `0`, then
makes it `1`.

### Declare

In `security.h`, add `#include <istream>`, `#include <map>` and
`#include <set>` to the includes. Then add, below `class StrSet`:

```cpp
// Task 3: the number of FAIL lines for each IP address in a log.
std::map<std::string, int> count_failures(std::istream& log);

// Task 3: the IP addresses with at least `limit` failures.
std::set<std::string> suspects(const std::map<std::string, int>& failures, int limit);
```

`count_failures` takes an `std::istream&`, so it can read a file or, in a
test, a string.

### Implement

Add to `security.cpp`:

```cpp
// ------------------------------- Task 3 ------------------------------------

std::map<std::string, int> count_failures(std::istream& log) {
    std::map<std::string, int> failures;
    std::string time, ip, user, result;
    while (log >> time >> ip >> user >> result) {
        // TODO: if result is "FAIL", add 1 to the count of ip
    }
    return failures;
}

std::set<std::string> suspects(const std::map<std::string, int>& failures, int limit) {
    std::set<std::string> found;
    // TODO: for each entry of failures, if its value is at least limit,
    //       insert its key into found
    return found;
}
```

### Test

Add this test, and call `test_logins();` in `main`:

```cpp
static void test_logins() {
    section("task 3: count_failures and suspects");
    std::istringstream log(
        "09:00:01 203.0.113.7 alice FAIL\n"
        "09:00:02 198.51.100.4 bob OK\n"
        "09:00:03 203.0.113.7 alice FAIL\n"
        "09:00:04 192.0.2.15 carol FAIL\n"
        "09:00:05 203.0.113.7 admin FAIL\n"
        "09:00:06 198.51.100.4 bob FAIL\n");
    std::map<std::string, int> failures = count_failures(log);
    CHECK(failures.size() == 3);
    CHECK(failures.at("203.0.113.7") == 3);
    CHECK(failures.at("198.51.100.4") == 1); // the OK line is not counted
    CHECK(failures.at("192.0.2.15") == 1);

    std::set<std::string> bad = suspects(failures, 3);
    CHECK(bad == std::set<std::string>({"203.0.113.7"}));
    bad = suspects(failures, 1);
    CHECK(bad.size() == 3);
    CHECK(*bad.begin() == "192.0.2.15"); // a set is in sorted order
    CHECK(suspects(failures, 4).empty());
}
```

Compile and run until you see `ALL TESTS PASSED`.

### Try it

Run your detector on `auth.log`, with a limit of 5 failures:

```bash
$ g++ -std=c++17 -Wall -Wextra -g security.cpp detect.cpp -o detect
$ ./detect auth.log 5
```

You should find two suspects. Open `auth.log` and look at what each of them
did. Then answer **Q3**.

> [!WARNING]
> `m[key]` **adds** `key` when it is missing. Use it to count. To only
> **look**, use `m.count(key)` or `m.at(key)`. A check like
> `if (failures[ip] > 5)` adds one entry for every new address it sees, so an
> attacker with many addresses can make your map, and your memory, grow
> without end.

## Submission

Upload these files to Gradescope:

- `security.h`
- `security.cpp`
- `answers.md`

> [!WARNING]
> **Upload those three files and nothing else.** The autograder checks this
> before it looks at your code.

Your code should compile clean under `-Wall -Wextra -Werror`.

Questions: ask the instructor or a TA **in the room**, or post on Ed.
