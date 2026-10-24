// ===========================================================================
// digits.cpp -- runs your classifier.  Written for you.  Do not change it.
//
// Build it with your knn.cpp.  TASK says how far you are, so that it only
// calls what you have written:
//
//     $ g++ -std=c++17 -O2 -DTASK=4 knn.cpp digits.cpp -o digits
//
// Leave out -DTASK once you have finished Task 6.
//
//     $ ./digits show N                 the N-th training digit, as text   (Task 1)
//     $ ./digits classify N K           read the N-th test digit            (Task 4)
//     $ ./digits serve [TRAIN] [PORT]   the web page, with TRAIN digits     (Task 4)
//     $ ./digits accuracy TRAIN TEST K  read TEST test digits, and time it  (Task 5)
//
// TRAIN is how many training digits to load (at most 60000, the default).
// ===========================================================================

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "knn.h"

#ifndef TASK
#define TASK 6
#endif

#if TASK >= 4
#include "server.h"
#endif

namespace {

const char* TRAIN_IMAGES = "train-images.idx";
const char* TRAIN_LABELS = "train-labels.idx";
[[maybe_unused]] const char* TEST_IMAGES = "test-images.idx";
[[maybe_unused]] const char* TEST_LABELS = "test-labels.idx";

// The n-th digit of a pair of MNIST files.
Image nth(const char* images, const char* labels, int n) {
    MnistReader reader(images, labels);
    if (n < 0 || n >= reader.count()) {
        std::cerr << "there are only " << reader.count() << " digits\n";
        std::exit(1);
    }
    unsigned char pixels[PIXELS];
    int label = 0;
    for (int i = 0; i <= n; i++) {
        reader.next(pixels, label);
    }
    return Image(pixels, label);
}

// An image as text: darker pixels get denser characters.
void print(const Image& img) {
    const char* shades = " .:-=+*#%@";
    for (int row = 0; row < SIDE; row++) {
        std::string line;
        for (int col = 0; col < SIDE; col++) {
            char c = shades[img.at(row, col) * 9 / 255];
            line += c;
            line += c;
        }
        std::cout << line << '\n';
    }
}

[[maybe_unused]] double seconds_since(std::chrono::steady_clock::time_point start) {
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}

[[maybe_unused]] std::string pixels_json(const Image& img) {
    std::string out = "[";
    for (int row = 0; row < SIDE; row++) {
        for (int col = 0; col < SIDE; col++) {
            out += (row || col ? "," : "") + std::to_string(img.at(row, col));
        }
    }
    return out + "]";
}

void usage() {
    std::cerr <<
        "usage:\n"
        "  ./digits show N\n"
        "  ./digits classify N K\n"
        "  ./digits serve [TRAIN] [PORT]\n"
        "  ./digits accuracy TRAIN TEST K\n"
        "K, the number of neighbors, is at least 1.\n";
    std::exit(1);
}

[[maybe_unused]] [[noreturn]] void needs(int task) {
    std::cerr << "this needs Task " << task << ". Build with -DTASK=" << task
              << " or higher once it passes its tests.\n";
    std::exit(1);
}

#if TASK >= 4
Response handle(const Request& req, const Classifier& clf, const std::string& page) {
    Response res;
    if (req.path == "/" || req.path == "/index.html") {
        res.type = "text/html; charset=utf-8";
        res.body = page;
    } else if (req.path == "/stats") {
        res.body = "{\"train\":" + std::to_string(clf.size())
                 + ",\"task\":" + std::to_string(TASK) + "}";
    } else if (req.path == "/classify" && req.method == "POST") {
        // The body is the drawing: PIXELS numbers from 0 to 255, separated by commas.
        unsigned char pixels[PIXELS] = {0};
        std::stringstream in(req.body);
        std::string value;
        for (int i = 0; i < PIXELS && std::getline(in, value, ','); i++) {
            int v = std::atoi(value.c_str());
            pixels[i] = static_cast<unsigned char>(v < 0 ? 0 : v > 255 ? 255 : v);
        }
        Image query(pixels, -1);
        int k = std::atoi(req.param("k", "3").c_str());
        NeighborList list(k < 1 ? 1 : k);
        auto start = std::chrono::steady_clock::now();
        int guess = clf.classify(query, list);
        double ms = 1000 * seconds_since(start);

        int votes[10] = {0};
        for (int i = 0; i < list.size(); i++) {
            votes[list.label_at(i)]++;
        }
        res.body = "{\"guess\":" + std::to_string(guess)
                 + ",\"ms\":" + std::to_string(ms)
                 + ",\"pixels\":" + pixels_json(query) + ",\"votes\":[";
        for (int d = 0; d < 10; d++) {
            res.body += (d ? "," : "") + std::to_string(votes[d]);
        }
        res.body += "],\"neighbors\":[";
        for (int i = 0; i < list.size(); i++) {
            res.body += std::string(i ? "," : "")
                      + "{\"index\":" + std::to_string(list.index_at(i))
                      + ",\"label\":" + std::to_string(list.label_at(i))
                      + ",\"distance\":" + std::to_string(list.distance_at(i))
                      + ",\"pixels\":" + pixels_json(clf.image(list.index_at(i))) + "}";
        }
        res.body += "]}";
    } else if (req.path == "/mistakes") {
        // Reads the first `check` test digits and returns the ones it gets wrong.
        int check = std::atoi(req.param("check", "300").c_str());
        int k = std::atoi(req.param("k", "3").c_str());
        MnistReader reader(TEST_IMAGES, TEST_LABELS);
        NeighborList list(k < 1 ? 1 : k);
        unsigned char pixels[PIXELS];
        int label = 0;
        int seen = 0;
        res.body = "{\"wrong\":[";
        bool first = true;
        while (seen < check && reader.next(pixels, label)) {
            Image query(pixels, label);
            int guess = clf.classify(query, list);
            if (guess != label) {
                res.body += std::string(first ? "" : ",")
                          + "{\"index\":" + std::to_string(seen)
                          + ",\"label\":" + std::to_string(label)
                          + ",\"guess\":" + std::to_string(guess)
                          + ",\"pixels\":" + pixels_json(query) + "}";
                first = false;
            }
            seen++;
        }
        res.body += "],\"checked\":" + std::to_string(seen) + "}";
    } else {
        res.status = 404;
        res.type = "text/plain";
        res.body = "not found";
    }
    return res;
}
#endif

} // namespace

int main(int argc, char** argv) {
    if (argc < 2) usage();
    std::string mode = argv[1];

    if (mode == "show") {
        if (argc < 3) usage();
        Image img = nth(TRAIN_IMAGES, TRAIN_LABELS, std::atoi(argv[2]));
        print(img);
        std::cout << "label: " << img.label() << '\n';
    } else if (mode == "classify") {
#if TASK >= 4
        if (argc < 4 || std::atoi(argv[3]) < 1) usage();
        Image query = nth(TEST_IMAGES, TEST_LABELS, std::atoi(argv[2]));
        Classifier clf(TRAIN_IMAGES, TRAIN_LABELS, 60000);
        NeighborList list(std::atoi(argv[3]));
        auto start = std::chrono::steady_clock::now();
        int guess = clf.classify(query, list);
        double secs = seconds_since(start);
        print(query);
        std::cout << "label: " << query.label() << "   guess: " << guess
                  << "   (" << 1000 * secs << " ms)\n\nnearest:\n";
        for (int i = 0; i < list.size(); i++) {
            std::cout << "  training digit " << list.index_at(i) << ", a "
                      << list.label_at(i) << ", distance " << list.distance_at(i) << '\n';
        }
        if (list.size() > 0) {
            std::cout << '\n';
            print(clf.image(list.index_at(0)));
        }
#else
        needs(4);
#endif
    } else if (mode == "serve") {
#if TASK >= 4
        int train = argc >= 3 ? std::atoi(argv[2]) : 60000;
        int port = argc >= 4 ? std::atoi(argv[3]) : 8080;
        std::ifstream file("index.html");
        std::stringstream page;
        page << file.rdbuf();
        auto start = std::chrono::steady_clock::now();
        Classifier clf(TRAIN_IMAGES, TRAIN_LABELS, train);
        std::cout << "loaded " << clf.size() << " training digits in "
                  << seconds_since(start) << " s\n";
        return serve(port, [&](const Request& req) {
            return handle(req, clf, page.str());
        });
#else
        needs(4);
#endif
    } else if (mode == "accuracy") {
#if TASK >= 5
        if (argc < 5) usage();
        int train = std::atoi(argv[2]);
        int test = std::atoi(argv[3]);
        int k = std::atoi(argv[4]);
        if (k < 1) usage();
        Classifier clf(TRAIN_IMAGES, TRAIN_LABELS, train);
        auto start = std::chrono::steady_clock::now();
        double acc = clf.accuracy(TEST_IMAGES, TEST_LABELS, test, k);
        double secs = seconds_since(start);
        std::cout << "train " << clf.size() << ", test " << test << ", k " << k
                  << ":  accuracy " << 100 * acc << "%,  " << secs << " s,  "
                  << 1000 * secs / (test > 0 ? test : 1) << " ms per digit\n";
#else
        needs(5);
#endif
    } else {
        usage();
    }
    return 0;
}
