# Setting up your C++ environment

You need three things for this course, and nothing else:

1. a **terminal**
2. the **`g++`** compiler
3. an **editor**, VS Code is recommended, but any editor works.

Pick your platform below.

> All course code is plain C++ compiled from the command line. There is no
> build system, no IDE project file, and nothing to configure per assignment.

> [!IMPORTANT]
> **A `$` at the start of a line is the prompt, not part of the command.** Your
> terminal prints it; you type what comes after it and press **Return**. Lines
> without a `$` are output the computer printed back at you.
>
> On Windows PowerShell the prompt looks like `PS>` instead. Same idea.

## Windows

**Use WSL.** Do not use MinGW, Cygwin, or MSVC. WSL gives you a real Linux
system running alongside Windows, which is the same kind of environment the
graders and the autograder use.

### 1. Install WSL

Open **PowerShell as Administrator** and run:

```powershell
PS> wsl --install
```

Reboot when it asks. On first launch your Linux system will ask you to create a
**username and password**: this is your Linux account, unrelated to Windows.
Write the password down; `sudo` asks for it.

> **Which Linux do you get?** WSL can run many distributions. `wsl --install`
> with no arguments gives you the current default, which is **Ubuntu**, and that
> is what this course assumes. To see the rest, or to pick a different one:
>
> ```powershell
> PS> wsl --list --online
> PS> wsl --install -d Debian
> ```
>
> **Stay on Ubuntu unless you have a reason not to.** The autograder runs
> Ubuntu, every `apt` command in the course handouts assumes Debian or Ubuntu,
> and the TAs can help you fastest there. If you already run something else and
> know your way around it, that is fine: install the compiler with your own
> distribution's package manager (see the [Linux](#linux) section) and
> everything after that is identical.

Docs: [Install WSL](https://learn.microsoft.com/en-us/windows/wsl/install)
and [basic WSL commands](https://learn.microsoft.com/en-us/windows/wsl/basic-commands)

### 2. Install the compiler

Inside the **Linux** terminal (not PowerShell). On Ubuntu or Debian:

```bash
$ sudo apt update
$ sudo apt install build-essential gdb
```

`build-essential` pulls in `g++`, `make`, and the standard headers.

### 3. Keep your files on the Linux side

Work in your Linux home directory (`~`, which is `/home/<you>`), **not** in
`/mnt/c/Users/...`. Cross-filesystem access is slow and causes permission
oddities.

```bash
$ mkdir -p ~/csc212/lab01
$ cd ~/csc212/lab01
```

Docs: [Working across file systems](https://learn.microsoft.com/en-us/windows/wsl/filesystems)

### 4. Connect VS Code to WSL

Install [VS Code](https://code.visualstudio.com/download) **on Windows**, then
install the [WSL extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-wsl).

From your Linux terminal:

```bash
$ code .
```

The first run installs a small server inside WSL. When it opens, the bottom-left
corner must read **`WSL: Ubuntu`** (or the name of whichever distribution you
installed). If it says nothing about WSL, you are editing Windows files with a
Windows compiler and nothing in this course will work as described.

Docs: [Using C++ and WSL in VS Code](https://code.visualstudio.com/docs/cpp/config-wsl)

---

## macOS

### 1. Open the Terminal

macOS has a terminal built in. You just have to find it. Any of these work:

- **Spotlight:** press **Cmd + Space**, type `terminal`, press **Return**.
- **Finder:** Applications -> Utilities -> **Terminal**.
- **Launchpad:** the **Other** folder -> **Terminal**.

A window opens showing a line that ends in `$`. That is the prompt. Everything
below is typed there, one line at a time, pressing **Return** after each.

> **Keep it handy.** While Terminal is running, right-click its Dock icon ->
> **Options** -> **Keep in Dock**. You will open it every week this semester.

### 2. Install the compiler

At the `$` prompt, type:

```bash
$ xcode-select --install
```

A dialog box appears. Click **Install**, then **Agree**, and wait: it takes a
few minutes and downloads a few gigabytes. That is the whole installation.

> If it answers `command line tools are already installed`, you are already set.
> Move on to step 3.

### 3. Verify

```bash
$ g++ --version
```

You will see something like `Apple clang version 17.0.0`. **This is expected.**
On macOS, `g++` is a front end for Clang rather than GNU GCC. For everything in
this course they are interchangeable: the command is still `g++`, the flags in
every handout are the same, and your code will compile identically.

> **If you want genuine GNU GCC** (you almost certainly do not need it):
> install [Homebrew](https://brew.sh), run `brew install gcc`, and the real
> compiler is then `g++-15`, note the version suffix. Plain `g++` still points
> at Apple Clang.

Docs: [Using Clang in VS Code on macOS](https://code.visualstudio.com/docs/cpp/config-clang-mac)

---

## Linux

Open a terminal (**Ctrl + Alt + T** on most desktops, or search for "terminal"
in your application launcher), then:

```bash
# Debian / Ubuntu / Mint / Pop!_OS
$ sudo apt update && sudo apt install build-essential gdb

# Fedora
$ sudo dnf install gcc-c++ gdb make

# Arch
$ sudo pacman -S base-devel gdb
```

Docs: [Using GCC with VS Code on Linux](https://code.visualstudio.com/docs/cpp/config-linux)

---

## Check that it works

Every platform, same three commands. Create `hello.cpp`:

```cpp
#include <iostream>

int main() {
    std::cout << "toolchain is alive\n";
    return 0;
}
```

then:

```bash
$ g++ --version                                    # the compiler exists
$ g++ -std=c++17 -Wall -Wextra hello.cpp -o hello  # it compiles
$ ./hello                                          # it runs
```

If you see `toolchain is alive`, you are done. **Windows users:** run these in
your Linux terminal, with your file under `~`.

### The flags, once

| flag | what it does |
|---|---|
| `-std=c++17` | use the 2017 language standard (the default varies by compiler version) |
| `-Wall` | turn on the common warnings |
| `-Wextra` | turn on the ones `-Wall` leaves out. Several of them catch real bugs |
| `-Werror` | treat every warning as an error. Use it before you submit |
| `-g` | keep debug symbols, so a debugger can show you line numbers |
| `-O2` | optimize. Use it for benchmarking, never for debugging |

Full list: [GCC warning options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html)

---

## VS Code (recommended on macOS and Windows)

1. Download from [code.visualstudio.com/download](https://code.visualstudio.com/download).
2. Install the **C/C++** extension
   ([ms-vscode.cpptools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)).
   This gives you `#include` resolution, go-to-definition, and error squiggles.
3. Windows only: install the **WSL** extension as described above.
4. Open the integrated terminal with **Ctrl+`** (backtick). Compile there. You do
   not need to configure `tasks.json` or a debugger for this course.

Docs: [C/C++ for Visual Studio Code](https://code.visualstudio.com/docs/languages/cpp)

---

## Turn off AI autocomplete

Turn off AI code completion before your first lab, and leave it off for the
semester. The course policy on AI is in the syllabus and it is not restrictive:
use a model to explain a concept, decode a compiler error, or review code you
wrote. Autocomplete is a different thing. It writes for you continuously,
silently, and without being asked, finishing your loop before you have decided
what the loop does, and deciding what the loop does is the part this course is
grading. VS Code documents how to switch it off, both from the Copilot menu in
the status bar and through your settings:

**[Enable or disable inline suggestions](https://code.visualstudio.com/docs/editing/ai-powered-suggestions#_enable-or-disable-inline-suggestions)**
(VS Code documentation)

To confirm it worked, open a `.cpp` file, type `for (int i = 0;` and wait three
seconds. No grey text should appear. If you use a different editor, the same
requirement applies: find your completion plugin and switch it off, and ask on
Ed if you are not sure how.

---

## Troubleshooting

| symptom | fix |
|---|---|
| `g++: command not found` | You skipped the install step for your platform, or (Windows) you are in PowerShell instead of your Linux terminal. |
| `wsl --install` fails | Enable virtualization in your BIOS/UEFI, then retry. See [WSL install docs](https://learn.microsoft.com/en-us/windows/wsl/install). |
| VS Code bottom-left does not mention WSL | Close the window, go back to the Linux terminal, `cd` to your folder, run `code .` again. |
| `error: 'std::sort' was not declared` | A missing `#include`. C++ does not include transitively in a guaranteed way. Add `<algorithm>`. |
| Warnings you do not understand | Read them top to bottom. **The first error is the real one**; the rest are usually fallout. |
| `permission denied` running `./prog` | `chmod +x prog`, or you are on a Windows-mounted path (`/mnt/c/...`), move to `~`. |

Still stuck? Post on **Ed** with your OS, the exact command you ran, and the
**full** error text. Not a screenshot of part of it.
