// ===========================================================================
// test_network.cpp -- your local tests, on facebook-small.txt.
//
// These are the same kinds of checks the autograder runs, on the 10-user
// dataset instead of the 4,039-user one. Get all of these passing before you
// go near the big file.
//
// Build and run:
//     $ g++ -std=c++17 -Wall -Wextra -Werror -g network.cpp test_network.cpp -o test_network
//     $ ./test_network
//
// The autograder runs different data and a few extra cases, so passing here
// is not a guarantee. It is a good start.
// ===========================================================================

#include "checks.h"
#include "network.h"

#include <stdexcept>
#include <string>

// The friendships in facebook-small.txt:
//
//     0: 1 3 7 8        5: 2 4 6 9
//     1: 0 2 4 6        6: 1 3 5 7 8
//     2: 1 5 9          7: 0 4 6 9
//     3: 0 4 6 9        8: 0 4 6 9
//     4: 1 3 5 7 8      9: 2 3 5 7 8

static void test_constructor() {
    checks::Case c("constructor");
    Network net("facebook-small.txt");
    CHECK(net.get_num_users() == 10);
    CHECK(net.get_num_connections() == 21);   // as in the file header, not 42
}

// Every method that only reads the network is declared const. This case does
// not test an answer, it tests that your declarations say so: if it fails to
// compile, you left `const` off one of them.
static void test_const_correctness() {
    checks::Case c("const correctness");
    const Network net("facebook-small.txt");
    CHECK(net.get_num_users() == 10);
    CHECK(net.get_num_connections() == 21);
    CHECK(net.is_friend(0, 1) == true);
    CHECK(net.num_friends(4) == 5);
    CHECK(net.num_friends_of_friends(0) == 8);
    CHECK(net.num_mutual_friends(0, 4) == 4);
    CHECK(net.most_popular_user() == 4);
}

static void test_missing_file() {
    checks::Case c("missing file");
    CHECK_THROWS_AS(Network("no-such-file.txt"), std::runtime_error);
}

static void test_is_friend() {
    checks::Case c("is_friend");
    Network net("facebook-small.txt");

    CHECK(net.is_friend(0, 1) == true);
    CHECK(net.is_friend(0, 8) == true);
    CHECK(net.is_friend(2, 9) == true);
    CHECK(net.is_friend(4, 5) == true);
    CHECK(net.is_friend(8, 9) == true);

    // Friendship goes both ways. If only one direction works, you stored the
    // edges once and never looked for the reverse.
    CHECK(net.is_friend(1, 0) == true);
    CHECK(net.is_friend(9, 2) == true);
    CHECK(net.is_friend(9, 8) == true);

    CHECK(net.is_friend(0, 2) == false);
    CHECK(net.is_friend(0, 4) == false);
    CHECK(net.is_friend(1, 3) == false);
    CHECK(net.is_friend(2, 3) == false);

    // Nobody is their own friend.
    for (int i = 0; i < 10; ++i) CHECK(net.is_friend(i, i) == false);
}

static void test_exceptions() {
    checks::Case c("exceptions");
    Network net("facebook-small.txt");

    CHECK_THROWS_AS(net.is_friend(-1, 0), std::out_of_range);
    CHECK_THROWS_AS(net.is_friend(0, 10), std::out_of_range);
    CHECK_THROWS_AS(net.num_friends(-5), std::out_of_range);
    CHECK_THROWS_AS(net.num_friends(10), std::out_of_range);
    CHECK_THROWS_AS(net.num_friends_of_friends(10), std::out_of_range);
    CHECK_THROWS_AS(net.num_mutual_friends(0, 99), std::out_of_range);
    CHECK_THROWS_AS(net.num_mutual_friends(99, 0), std::out_of_range);

    // 0 and 9 are the first and last valid ids.
    CHECK_NOTHROW(net.is_friend(0, 9));
    CHECK_NOTHROW(net.num_friends(9));
    CHECK_NOTHROW(net.num_friends_of_friends(0));
}

static void test_num_friends() {
    checks::Case c("num_friends");
    Network net("facebook-small.txt");
    const int expected[10] = {4, 4, 3, 4, 5, 4, 5, 4, 4, 5};
    for (int u = 0; u < 10; ++u) CHECK(net.num_friends(u) == expected[u]);
}

static void test_num_friends_of_friends() {
    checks::Case c("num_friends_of_friends");
    Network net("facebook-small.txt");
    // Distance 1 or 2, each user counted once, not counting the user.
    // User 0 reaches 1, 3, 7, 8 directly and 2, 4, 6, 9 through them: 8 total.
    // User 0 cannot reach 5 in two steps, so 5 is the only one missing.
    const int expected[10] = {8, 9, 9, 9, 9, 8, 9, 9, 9, 9};
    for (int u = 0; u < 10; ++u) CHECK(net.num_friends_of_friends(u) == expected[u]);
}

static void test_num_mutual_friends() {
    checks::Case c("num_mutual_friends");
    Network net("facebook-small.txt");

    CHECK(net.num_mutual_friends(0, 1) == 0);
    CHECK(net.num_mutual_friends(0, 3) == 0);
    CHECK(net.num_mutual_friends(0, 4) == 4);
    CHECK(net.num_mutual_friends(0, 9) == 3);
    CHECK(net.num_mutual_friends(1, 3) == 3);
    CHECK(net.num_mutual_friends(2, 7) == 1);
    CHECK(net.num_mutual_friends(4, 6) == 5);
    CHECK(net.num_mutual_friends(7, 8) == 4);

    // Order must not matter.
    CHECK(net.num_mutual_friends(9, 0) == 3);
    CHECK(net.num_mutual_friends(3, 1) == 3);

    // Same user twice: all of that user's friends are mutual.
    CHECK(net.num_mutual_friends(0, 0) == 4);
    CHECK(net.num_mutual_friends(2, 2) == 3);
    CHECK(net.num_mutual_friends(5, 5) == 4);
}

static void test_most_popular_user() {
    checks::Case c("most_popular_user");
    Network net("facebook-small.txt");
    // Users 4, 6 and 9 all have 5 friends. The smallest id wins.
    CHECK(net.most_popular_user() == 4);
}

// ---------------------------------------------------------------------------
// The rule of three.
//
// Your class holds raw pointers. If you do not write a copy constructor and a
// copy assignment operator, C++ writes them for you, and the ones it writes
// copy the pointers instead of what they point at. Two objects then share one
// array, and the second destructor frees memory that was already freed.
//
// The first two cases below check that a copy is a real copy. The third checks
// that the copy still works after the original is gone, which is where a
// shallow copy usually crashes.
// ---------------------------------------------------------------------------

static void test_copy_constructor() {
    checks::Case c("copy constructor");
    Network original("facebook-small.txt");
    Network copy(original);

    CHECK(copy.get_num_users() == 10);
    CHECK(copy.get_num_connections() == 21);
    CHECK(copy.is_friend(0, 1) == true);
    CHECK(copy.is_friend(0, 2) == false);
    CHECK(copy.num_friends(4) == 5);
    CHECK(copy.num_friends_of_friends(0) == 8);
    CHECK(copy.num_mutual_friends(0, 4) == 4);
    CHECK(copy.most_popular_user() == 4);
}

static void test_copy_assignment() {
    checks::Case c("copy assignment");
    Network a("facebook-small.txt");
    Network b("facebook-small.txt");
    b = a;

    CHECK(b.get_num_users() == 10);
    CHECK(b.is_friend(2, 9) == true);
    CHECK(b.num_friends(6) == 5);

    // Assigning an object to itself must not destroy it. A version that frees
    // its arrays before copying will fail right here. (We go through a pointer
    // because the compiler warns about writing `b = b` directly.)
    Network *alias = &b;
    b = *alias;
    CHECK(b.get_num_users() == 10);
    CHECK(b.is_friend(2, 9) == true);
    CHECK(b.num_friends(6) == 5);
}

static void test_copy_outlives_original() {
    checks::Case c("copy outlives original");
    Network *survivor = nullptr;
    {
        Network original("facebook-small.txt");
        survivor = new Network(original);
    }   // original is destroyed here, and frees its arrays

    CHECK(survivor->get_num_users() == 10);
    CHECK(survivor->is_friend(0, 1) == true);
    CHECK(survivor->num_friends(4) == 5);
    CHECK(survivor->most_popular_user() == 4);
    delete survivor;
}

// Nothing above this point notices a missing `delete[]`: a leaking Network
// gives every right answer until the program runs out of memory. This case
// notices, by counting. `checks::arrays_in_use()` goes up on every `new[]` and
// down on every `delete[]` anywhere in your program, so if it does not come
// back to where it started once the objects are gone, something was not freed.
static void test_no_leaks() {
    checks::Case c("no leaks");
    const long long before = checks::arrays_in_use();

    {   // the destructor gives back what the constructor took
        Network net("facebook-small.txt");
        CHECK(checks::arrays_in_use() > before);   // it really allocated
    }
    CHECK_ALL_FREED(before);

    {   // a copy allocates arrays of its own, and frees them
        Network original("facebook-small.txt");
        Network copy(original);
        Network assigned("facebook-small.txt");
        assigned = copy;
    }
    CHECK_ALL_FREED(before);

    for (int i = 0; i < 100; ++i) {
        Network *net = new Network("facebook-small.txt");
        delete net;
    }
    CHECK_ALL_FREED(before);
}

// ---------------------------------------------------------------------------
// YOUR TESTS -- add at least two.
// One of them must be a case you actually got wrong, or expected to get wrong.
// A test that has never failed has never told you anything.
// ---------------------------------------------------------------------------
static void test_your_cases() {
    checks::Case c("your cases");
    // TODO
}

int main(int argc, char **argv) {
    checks::configure(argc, argv);
    std::cout << "running tests\n\n";

    test_constructor();
    test_const_correctness();
    test_missing_file();
    test_is_friend();
    test_exceptions();
    test_num_friends();
    test_num_friends_of_friends();
    test_num_mutual_friends();
    test_most_popular_user();
    test_copy_constructor();
    test_copy_assignment();
    test_copy_outlives_original();
    test_no_leaks();
    test_your_cases();

    return checks::summary();
}
