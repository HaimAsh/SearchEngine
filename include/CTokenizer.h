//
// Created by haimash on 25/01/2026.
//

#ifndef SEARCHENGINE_CTOKENIZER_H
#define SEARCHENGINE_CTOKENIZER_H

#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "absl/container/flat_hash_set.h"

class CTokenizer
{
public:

    explicit CTokenizer(std::string_view text) noexcept;

    /// checks if there are words left in the text
    /// @return true if words exist, false otherwise
    [[nodiscard]] bool HasNext() const;

    /// searches for the next valid word
    /// since this function is designed to be used frequently, it returns string_view to avoid returning string and
    /// allocating extra memory, so after calling this function, copy the word, otherwise it will be invalid
    /// @return returns string_view of the word
    std::string_view Next();

private:

    void FindWord();

    std::string_view m_text;
    size_t m_textIndex = 0;
    absl::string_view m_delimiters = " \n\t\r.,;:!?\"'()[]{}<>-_/\\|`~@#$%^&*+=\0";
    absl::string_view m_currentToken;

    std::string m_normalizeWord;

    static const absl::flat_hash_set<absl::string_view> m_stopWords;
};

#endif //SEARCHENGINE_CTOKENIZER_H