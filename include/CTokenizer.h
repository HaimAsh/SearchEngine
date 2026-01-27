//
// Created by haimash on 25/01/2026.
//
/// @file CTokenizer.h
/// @brief A memory-efficient text tokenizer for extracting normalized words

#ifndef SEARCHENGINE_CTOKENIZER_H
#define SEARCHENGINE_CTOKENIZER_H

#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "absl/container/flat_hash_set.h"

/// @class CTokenizer
/// @brief Iterates over a string to produce clean, individual tokens.
///
/// This class handles delimiter stripping, stop-word filtering, and normalization.
/// It uses string_view to minimize heap allocations during the scanning process.
class CTokenizer
{
public:

    /// @brief Initializes the tokenizer with a source text.
    /// @param text The raw string to be tokenized.
    /// Note: The source text must outlive the tokenizer.
    explicit CTokenizer(std::string_view text) noexcept;

    /// @brief Checks if there are more tokens available in the text.
    /// @return true if another valid token exists.
    [[nodiscard]] bool HasNext() const;

    /// searches for the next valid word
    /// since this function is designed to be used frequently, it returns string_view to avoid returning string and
    /// allocating extra memory, so after calling this function, copy the word, otherwise it will be invalid
    /// @return returns string_view of the word
    std::string_view Next();

private:

    /// @brief Internal helper to advance m_textIndex to the start of the next valid word.
    void FindWord();

    std::string_view m_text;
    size_t m_textIndex = 0;
    static constexpr absl::string_view m_delimiters = " \n\t\r.,;:!?\"'()[]{}<>-_/\\|`~@#$%^&*+=\0";
    absl::string_view m_currentToken;

    std::string m_normalizeWord;

    /// Common words (the, a, is, etc.) to be ignored by the tokenizer.
    static const absl::flat_hash_set<absl::string_view> m_stopWords;
};

#endif //SEARCHENGINE_CTOKENIZER_H