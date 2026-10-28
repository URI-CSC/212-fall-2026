// ===========================================================================
// watch.cpp -- runs one of your functions and logs every call and return,
// for viewer/index.html.  Written for you.  Do not change it.
//
// Build it with your list.cpp.  TASK says how far you are, so that it only
// calls what you have written:
//
//     $ g++ -std=c++17 -Wall -Wextra -g -DTASK=1 list.cpp watch.cpp -o watch
//
// Run it with a function and its input.  The log goes to the screen, so save
// it in a file with >.  A short summary goes to the screen as well:
//
//     $ ./watch size 3 -5 6 18 > size.log
//     $ ./watch max_slow 1..5 > max_slow.log         1..5 means 1 2 3 4 5
//
//     Task 1:  size LIST
//     Task 2:  print LIST    print_back LIST
//     Task 3:  remove_first VALUE LIST    clear LIST
//     Task 4:  power B N     power_2 B N     max_slow LIST     max_fast LIST
//     Task 5:  insert_sorted VALUE LIST
// ===========================================================================

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <new>
#include <sstream>
#include <string>
#include <vector>

#include "list.h"
#include "trace.h"

#ifndef TASK
#define TASK 5
#endif

namespace {

// --------------------------------------------------------------------------
// Memory: in this program a deleted node is never given back, so its address
// is never reused, and the log can show it as deleted.  The list is small,
// and the program ends right away, so this costs nothing.
const std::size_t MAX_FREED = 100000;
const void* freed[MAX_FREED];
std::size_t num_freed = 0;

bool was_freed(const void* p) {
    for (std::size_t i = 0; i < num_freed; i++) {
        if (freed[i] == p) return true;
    }
    return false;
}

// --------------------------------------------------------------------------
// The log.

const long long MAX_EVENTS = 20000; // more than this is too much to watch
long long events = 0;
Node* watched = nullptr; // the first node of the list being watched

// Writes the list that starts at `watched`: "list #1:3 #2:-5 #3:6".  A node
// that was deleted is written with ":x" at the end, and the walk stops there.
void log_list() {
    std::cout << "list";
    const Node* p = watched;
    for (int steps = 0; p != nullptr && steps < 1000; steps++) {
        std::cout << " #" << trace::id_of(p) << ':';
        if (was_freed(p)) {
            std::cout << "?:x";
            break;
        }
        std::cout << p->data;
        p = p->next;
    }
    std::cout << '\n';
}

void after_return() {
    if (++events > MAX_EVENTS) {
        trace::out = nullptr; // stop logging, but keep running
        std::cout << "truncated\n";
        return;
    }
    if (watched != nullptr || num_freed > 0) log_list();
}

// Output written by print and print_back: each piece becomes an "out" line.
class OutBuf : public std::streambuf {
    protected:
        int overflow(int c) override {
            if (c != EOF) std::cout << "out " << static_cast<char>(c) << '\n';
            return c;
        }
        std::streamsize xsputn(const char* s, std::streamsize n) override {
            std::cout << "out " << std::string(s, static_cast<std::size_t>(n)) << '\n';
            return n;
        }
};

// --------------------------------------------------------------------------
// Arguments.

[[noreturn]] void usage() {
    std::cerr <<
        "usage: ./watch FUNCTION INPUT > file.log\n"
        "  Task 1:  size LIST\n"
        "  Task 2:  print LIST             print_back LIST\n"
        "  Task 3:  remove_first VALUE LIST    clear LIST\n"
        "  Task 4:  power B N    power_2 B N    max_slow LIST    max_fast LIST\n"
        "  Task 5:  insert_sorted VALUE LIST\n"
        "A LIST is numbers separated by spaces, like 3 -5 6 18.  1..5 means 1 2 3 4 5.\n";
    std::exit(1);
}

[[maybe_unused]] [[noreturn]] void needs(int task) {
    std::cerr << "this needs Task " << task << ". Build with -DTASK=" << task
              << " once its tests pass.\n";
    std::exit(1);
}

int number(const std::string& text) {
    char* end = nullptr;
    long v = std::strtol(text.c_str(), &end, 10);
    if (text.empty() || *end != '\0') {
        std::cerr << "not a number: " << text << '\n';
        usage();
    }
    return static_cast<int>(v);
}

// The numbers in argv[from], argv[from + 1], ...  "1..5" is 1 2 3 4 5.
std::vector<int> numbers(int argc, char** argv, int from) {
    std::vector<int> values;
    for (int i = from; i < argc; i++) {
        std::string arg = argv[i];
        std::size_t dots = arg.find("..");
        if (dots != std::string::npos && dots > 0) {
            int lo = number(arg.substr(0, dots));
            int hi = number(arg.substr(dots + 2));
            for (int v = lo; v <= hi; v++) values.push_back(v);
        } else {
            values.push_back(number(arg));
        }
    }
    return values;
}

// Builds the list with your build(), before the log starts.
Node* make_list(const std::vector<int>& values) {
    Node* head = build(values, 0);
    for (const Node* p = head; p != nullptr; p = p->next) trace::id_of(p);
    return head;
}

void start(const std::string& title) {
    std::cout << "title " << title << '\n';
    trace::calls = 0;
    trace::out = &std::cout;
    trace::after_return = after_return;
}

void finish(const std::string& result) {
    trace::out = nullptr;
    std::cout << "result " << result << '\n';
    std::cout << "calls " << trace::calls << '\n';
    std::cerr << result << "   (" << trace::calls << (trace::calls == 1 ? " call)\n" : " calls)\n");
}

[[maybe_unused]] std::string text_of(double v) {
    std::ostringstream text;
    text << v;
    return text.str();
}

} // namespace

void* operator new(std::size_t n) {
    void* p = std::malloc(n == 0 ? 1 : n);
    if (p == nullptr) throw std::bad_alloc();
    return p;
}

void operator delete(void* p) noexcept {
    if (p != nullptr && num_freed < MAX_FREED) freed[num_freed++] = p;
}

void operator delete(void* p, std::size_t) noexcept { operator delete(p); }

int main(int argc, char** argv) {
    if (argc < 2) usage();
    std::string f = argv[1];

    if (f == "size") {
        watched = make_list(numbers(argc, argv, 2));
        start(f + " of the list");
        log_list();
        int r = size(watched);
        finish(f + " = " + std::to_string(r));
    } else if (f == "print" || f == "print_back") {
#if TASK >= 2
        watched = make_list(numbers(argc, argv, 2));
        OutBuf buf;
        std::ostream out(&buf);
        start(f + " of the list");
        log_list();
        if (f == "print") {
            print(watched, out);
        } else {
            print_back(watched, out);
        }
        finish(f + " done");
#else
        needs(2);
#endif
    } else if (f == "remove_first" || f == "clear" || f == "insert_sorted") {
#if TASK >= 3
        if (f == "clear") {
            watched = make_list(numbers(argc, argv, 2));
            start("clear the list");
            log_list();
            clear(watched);
            finish("clear done");
        } else {
            if (argc < 3) usage();
            int value = number(argv[2]);
            watched = make_list(numbers(argc, argv, 3));
            start(f + " " + std::to_string(value));
            log_list();
#if TASK >= 5
            Node* head = nullptr;
            if (f == "remove_first") {
                head = remove_first(watched, value);
            } else {
                head = insert_sorted(watched, value);
            }
#else
            if (f == "insert_sorted") needs(5);
            Node* head = remove_first(watched, value);
#endif
            trace::out = nullptr;
            watched = head; // the list now starts where the call said
            for (const Node* p = head; p != nullptr; p = p->next) trace::id_of(p);
            std::cout << "final ";
            log_list();
            finish(f + " done");
        }
#else
        needs(3);
#endif
    } else if (f == "power" || f == "power_2") {
#if TASK >= 4
        if (argc != 4) usage();
        double b = number(argv[2]);
        int n = number(argv[3]);
        if (n < 0) usage();
        start(f + "(" + argv[2] + ", " + argv[3] + ")");
        double r = f == "power" ? power(b, n) : power_2(b, n);
        finish(f + " = " + text_of(r));
#else
        needs(4);
#endif
    } else if (f == "max_slow" || f == "max_fast") {
#if TASK >= 4
        std::vector<int> values = numbers(argc, argv, 2);
        if (values.empty()) {
            std::cerr << "the maximum of an empty list is not defined\n";
            return 1;
        }
        watched = make_list(values);
        start(f + " of the list");
        log_list();
        int r = f == "max_slow" ? max_slow(watched) : max_fast(watched);
        finish(f + " = " + std::to_string(r));
#else
        needs(4);
#endif
    } else {
        usage();
    }
    return 0;
}
