// ===========================================================================
// checks.h -- the test harness for CSC 212.
//
// Same harness as Lab 01, plus named cases and a timer. You do not need to
// change anything here, but read it. It is short.
//
//   * CHECK(expr) uses the # operator to turn the text of your expression into
//     a string, so a failure can name the check that broke.
//
//   * A Case starts a timer when it is created and prints its result when it
//     is destroyed. The object's lifetime is the test case. Same idea you are
//     about to implement in Network: the constructor takes a resource, the
//     destructor gives it back.
//
//   * Each case prints as soon as it finishes. If your program crashes
//     halfway, you still see which cases passed before it died.
// ===========================================================================
#ifndef CHECKS_H
#define CHECKS_H

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <new>
#include <string>

#ifndef _WIN32
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace checks {

// Seconds a single case is allowed to take. Only enforced under --report, so
// it never interferes with you running the tests yourself.
static const unsigned CASE_TIME_LIMIT = 45;

static int  total_run    = 0;      // every check, across every case
static int  total_failed = 0;
static int  case_run     = 0;      // just the case running now
static int  case_failed  = 0;
static bool report_mode  = false;  // --report adds machine-readable lines
static std::string case_name;

// The autograder passes --report. You never need to.
inline void configure(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--report") report_mode = true;
    }
}

#ifndef _WIN32
// A case that runs too long is stopped here rather than by an outside timeout,
// so the cases that already finished keep their results. Only write() and
// _exit() are safe to call from a signal handler, so that is all this does.
static char timed_out_name[128];

inline void on_alarm(int) {
    const char *prefix = "\nTIMEOUT|";
    ssize_t ignored = write(1, prefix, std::strlen(prefix));
    ignored = write(1, timed_out_name, std::strlen(timed_out_name));
    ignored = write(1, "\n", 1);
    (void)ignored;
    _exit(2);
}

inline void start_alarm(const std::string& name) {
    if (!report_mode) return;
    std::strncpy(timed_out_name, name.c_str(), sizeof(timed_out_name) - 1);
    timed_out_name[sizeof(timed_out_name) - 1] = '\0';
    std::signal(SIGALRM, on_alarm);
    alarm(CASE_TIME_LIMIT);
}

inline void stop_alarm() {
    if (report_mode) alarm(0);
}
#else
inline void start_alarm(const std::string&) {}
inline void stop_alarm() {}
#endif

inline void record(bool ok, const std::string& expr, int line) {
    ++total_run;
    ++case_run;
    if (!ok) {
        ++total_failed;
        ++case_failed;
        std::cout << "\n    FAIL  line " << line << ":  " << expr;
        if (report_mode) {
            std::cout << "\nFAILURE|" << case_name << "|" << expr << "|" << line;
        }
    }
}

// ---------------------------------------------------------------------------
// Counting array allocations.
//
// The two functions at the bottom of this file replace the global new[] and
// delete[], so every `new int[n]` and every `delete[] p` in the program moves
// this counter. std::vector, std::string and std::set allocate through the
// single-object form, so they never appear here. What the counter measures is
// exactly the raw arrays you allocated yourself.
//
// A Network that has been destroyed should leave the counter where it found
// it. If it is higher, something was never freed.
// ---------------------------------------------------------------------------

static long long live_arrays = 0;

inline long long arrays_in_use() { return live_arrays; }

inline void check_balanced(long long before, int line) {
    const long long now = arrays_in_use();
    record(now == before,
           "every array was freed (still allocated: "
               + std::to_string(now - before) + ")",
           line);
}

class Case {
  public:
    explicit Case(const std::string& name) : start_(clock_::now()) {
        case_name   = name;
        case_run    = 0;
        case_failed = 0;
        // No newline yet: any FAIL lines should appear under this name.
        std::cout << "  " << std::left << std::setw(24) << name << std::flush;
        start_alarm(name);
    }

    ~Case() {
        stop_alarm();
        const double secs = std::chrono::duration<double>(clock_::now() - start_).count();
        if (case_failed > 0) std::cout << "\n  " << std::string(24, ' ');
        std::cout << std::right << std::setw(4) << case_run << " checks"
                  << std::setw(9) << std::fixed << std::setprecision(3) << secs << " s   "
                  << (case_failed == 0 ? "ok" : "FAILED") << '\n' << std::flush;
        if (report_mode) {
            std::cout << "RESULT|" << case_name << "|"
                      << (case_failed == 0 ? "pass" : "fail") << "|"
                      << case_run << "|" << case_failed << "|"
                      << std::fixed << std::setprecision(6) << secs << '\n' << std::flush;
        }
    }

    Case(const Case&) = delete;
    Case& operator=(const Case&) = delete;

  private:
    using clock_ = std::chrono::steady_clock;
    clock_::time_point start_;
};

// Runs one test case.
//
// When you run the tests yourself this just calls the function. Under
// --report the autograder runs each case in a child process, so that a case
// that crashes or runs too long costs only its own task and the rest still
// run. Results are printed by the child as it goes, which is why they survive.
#ifndef _WIN32
inline void run(void (*case_function)()) {
    if (!report_mode) { case_function(); return; }
    std::cout.flush();
    const pid_t pid = fork();
    if (pid == 0) {
        case_function();
        std::cout.flush();
        _exit(0);
    }
    if (pid < 0) { case_function(); return; }   // no fork: run it here
    int status = 0;
    waitpid(pid, &status, 0);
}
#else
inline void run(void (*case_function)()) { case_function(); }
#endif

inline int summary() {
    std::cout << '\n'
              << total_run - total_failed << " / " << total_run << " checks passed\n";
    if (total_failed == 0) {
        std::cout << "ALL TESTS PASSED\n";
    } else {
        std::cout << total_failed << " CHECK(S) FAILED\n";
    }
    return total_failed == 0 ? 0 : 1;
}

}  // namespace checks

#define CHECK(expr) ::checks::record((expr), #expr, __LINE__)

// Records the counter, runs nothing, and later checks it came back:
//     const long long before = ::checks::arrays_in_use();
//     { Network net("facebook-small.txt"); }
//     CHECK_ALL_FREED(before);
#define CHECK_ALL_FREED(before) ::checks::check_balanced((before), __LINE__)

#define CHECK_THROWS_AS(expr, exception_type)                               \
    do {                                                                    \
        bool threw = false;                                                 \
        try { (void)(expr); }                                               \
        catch (const exception_type&) { threw = true; }                     \
        catch (...) { threw = false; }                                      \
        ::checks::record(threw, "throws " #exception_type ":  " #expr,      \
                         __LINE__);                                         \
    } while (0)

#define CHECK_NOTHROW(expr)                                                 \
    do {                                                                    \
        bool quiet = true;                                                  \
        try { (void)(expr); }                                               \
        catch (...) { quiet = false; }                                      \
        ::checks::record(quiet, "does not throw:  " #expr, __LINE__);       \
    } while (0)

// These replace the ones the standard library provides. Only this file
// defines them, and only the test program includes this file.
void* operator new[](std::size_t size) {
    void* p = std::malloc(size == 0 ? 1 : size);
    if (p == nullptr) throw std::bad_alloc();
    ++::checks::live_arrays;
    return p;
}

void operator delete[](void* p) noexcept {
    if (p != nullptr) { --::checks::live_arrays; std::free(p); }
}

void operator delete[](void* p, std::size_t) noexcept {
    ::operator delete[](p);
}

#endif  // CHECKS_H
