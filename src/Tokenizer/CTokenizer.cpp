//
// Created by haimash on 25/01/2026.
//

#include "CTokenizer.h"
#include "absl/strings/ascii.h"

constexpr uint32_t NORMALIZE_WORD_SIZE = 120;

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
    m_normalizeWord.assign(m_currentToken.data(), m_currentToken.size());

    absl::AsciiStrToLower(&m_normalizeWord); // Use the pointer version to do it in-place

    FindWord();

    return m_normalizeWord;
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
