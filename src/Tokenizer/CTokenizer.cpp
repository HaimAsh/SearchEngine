//
// Created by haimash on 25/01/2026.
//

#include "CTokenizer.h"
#include "absl/strings/ascii.h"
#include "absl/strings/match.h"

constexpr uint32_t NORMALIZE_WORD_SIZE = 120;

const absl::flat_hash_set<absl::string_view> CTokenizer::m_stopWords = {
    "a", "an", "and", "are", "as", "at", "be", "but", "by", "for",
    "if", "in", "into", "is", "it", "no", "not", "of", "on", "or",
    "such", "that", "the", "their", "then", "there", "these",
    "they", "this", "to", "was", "will", "with", "http", "https", "www"
};

CTokenizer::CTokenizer(std::string_view text) noexcept : m_text(text)
{
    FindWord();

    m_normalizeWord.reserve(NORMALIZE_WORD_SIZE);
}

bool CTokenizer::HasNext() const
{
    return false == m_currentToken.empty();
}

std::string_view CTokenizer::Next()
{
    auto endOfStopWord = m_stopWords.end();
    absl::string_view lookUpView;

    while (HasNext())
    {
        // --- THE GARBAGE FILTER ---
        // Skip tokens that are too short, too long, or don't start with a letter.
        if (m_currentToken.size() < 2 ||
            m_currentToken.size() > 30 ||
            !absl::ascii_isalpha(m_currentToken[0]))
        {
            FindWord();
            continue;
        }

        // 1. Check length immediately.
        // If it's > 5, it CAN'T be a stop word. Skip the hash lookup entirely!
        if (m_currentToken.size() > 5)
        {
            m_normalizeWord.assign(m_currentToken.data(), m_currentToken.size());
            absl::AsciiStrToLower(&m_normalizeWord);
            FindWord();
            return m_normalizeWord;
        }

        // 2. If it's small, it MIGHT be a stop word.
        // We must normalize to check the set (since our set is lowercase).
        m_normalizeWord.assign(m_currentToken.data(), m_currentToken.size());
        absl::AsciiStrToLower(&m_normalizeWord);

        // Save the raw token's state before moving the pointer
        FindWord();

        lookUpView = m_normalizeWord;
        // 3. Perform the O(1) Hash Lookup
        if (m_stopWords.find(lookUpView) == m_stopWords.end())
        {
            return m_normalizeWord;
        }

        // If it was a stop word, the loop continues...
    }

    return "";
}

void CTokenizer::FindWord()
{
    size_t wordStart = m_text.find_first_not_of(m_delimiters, m_textIndex);

    if (wordStart != absl::string_view::npos)
    {
        size_t wordEnd = m_text.find_first_of(m_delimiters, wordStart);
        m_currentToken = m_text.substr(wordStart, wordEnd - wordStart);

        // Advance index
        m_textIndex = (wordEnd == absl::string_view::npos) ? m_text.size() : wordEnd;
    }
    else
    {
        m_currentToken = "";
    }
}
