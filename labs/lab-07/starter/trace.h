// ===========================================================================
// trace.h -- lets you watch your recursion.  Written for you.  Do not change
// it.
//
// Put one line at the top of every recursive function you write:
//
//     int size(Node* p) {
//         WATCH(p);
//         ...
//     }
//
// WATCH counts every call in trace::calls.  In your tests that is all it
// does.  In watch.cpp it also writes every call and every return to a log,
// which viewer/index.html plays back step by step.
// ===========================================================================

#ifndef TRACE_H
#define TRACE_H

#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>

namespace trace {

inline long long calls = 0; // every WATCH adds 1
inline std::ostream* out = nullptr; // the log; nullptr: no log
inline void (*after_return)() = nullptr; // called after each logged return
inline std::map<const void*, int> ids; // a short id for each node: #1, #2, ...

inline int id_of(const void* p) {
    int& id = ids[p];
    if (id == 0) id = static_cast<int>(ids.size());
    return id;
}

// How a call's argument appears in the log.
template <class T>
std::string describe(const T* p) {
    if (p == nullptr) return "null";
    return "#" + std::to_string(id_of(p));
}

template <class T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type describe(T v) {
    std::ostringstream text;
    text << v;
    return text.str();
}

// One call: created when the function starts, destroyed when it returns.
struct Frame {
    const char* name;

    Frame(const char* function, const std::string& arg) : name(function) {
        ++calls;
        if (out != nullptr) *out << "call " << name << ' ' << arg << '\n';
    }

    ~Frame() {
        if (out != nullptr) {
            *out << "return " << name << '\n';
            if (after_return != nullptr) after_return();
        }
    }
};

} // namespace trace

#define WATCH(arg) \
    trace::Frame watch_frame_(__func__, trace::out != nullptr ? trace::describe(arg) : std::string())

#endif // TRACE_H
