// ===========================================================================
// vector_tokens.h -- the same tokens kept in a std::vector instead of a
// linked list.  Written for you, for bench.cpp.
//
// merge() is written the obvious way: when it finds the pair, it overwrites
// the left token and erases the right one with erase().  It gives the same
// tokens as your TokenList::merge.
// ===========================================================================

#ifndef VECTOR_TOKENS_H
#define VECTOR_TOKENS_H

#include <cstddef>
#include <string>
#include <vector>

class TokenVector {
    private:
        std::vector<int> m_tokens;

    public:
        explicit TokenVector(const std::string& text) {
            for (char c : text) {
                m_tokens.push_back(static_cast<unsigned char>(c));
            }
        }

        size_t size() const { return m_tokens.size(); }

        std::vector<int> tokens() const { return m_tokens; }

        size_t merge(int left, int right, int new_token) {
            size_t removed = 0;
            size_t i = 0;
            while (i + 1 < m_tokens.size()) {
                if (m_tokens[i] == left && m_tokens[i + 1] == right) {
                    m_tokens[i] = new_token;
                    // every token after i + 1 moves one slot to the left
                    m_tokens.erase(m_tokens.begin() + i + 1);
                    removed++;
                }
                i++;
            }
            return removed;
        }
};

#endif  // VECTOR_TOKENS_H
