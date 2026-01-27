//
// Created by haimash on 26/01/2026.
//

#include "CInvertedIndex.h"

void CInvertedIndex::AddToken(absl::string_view token, uint32_t ID)
{
    // 1. Ensure the vector is large enough to hold this ID.
    // If ID is 0, size must be 1. If ID is 100, size must be 101.
    if (ID >= m_docWordCounts.size()) {
        m_docWordCounts.resize(ID + 1, 0);
    }

    std::vector<std::pair<uint32_t, uint32_t>>& IDs = m_invertedIndex[token];

    if (IDs.empty() || IDs.back().first != ID)
    {
        IDs.emplace_back(ID, 0);
    }
    else
    {
        IDs.back().second++;
    }
    m_docWordCounts[ID]++;
}

std::vector<uint32_t> CInvertedIndex::GetIDsOfToken(const absl::string_view token) const
{
    auto it = m_invertedIndex.find(token);
    std::vector<uint32_t> result;

    if (it != m_invertedIndex.end())
    {
        std::vector<std::pair<uint32_t, uint32_t>> current = it->second;
        result.reserve(current.size());

        for (auto& curr : current)
        {
            result.emplace_back(curr.first);
        }

        return result;
    }

    return m_emptyVector;
}

size_t CInvertedIndex::GetUniqueWordCount() const
{
    return m_invertedIndex.size();
}

void CInvertedIndex::AddTitle(uint32_t ID, absl::string_view title)
{
    if (ID >= m_docTitles.size()) {
        m_docTitles.resize(ID + 1);
        m_numOfDocs++;
    }
    m_docTitles[ID] = std::string(title);
}

const std::string& CInvertedIndex::GetTitle(uint32_t ID) const
{
    return m_docTitles[ID];
}