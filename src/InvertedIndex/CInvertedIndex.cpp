//
// Created by haimash on 26/01/2026.
//

#include "CInvertedIndex.h"

void CInvertedIndex::AddToken(absl::string_view token, uint32_t ID)
{
    std::vector<uint32_t>& IDs = m_invertedIndex[token];

    if (IDs.empty() || IDs.back() != ID)
    {
        IDs.push_back(ID);
    }
}

const std::vector<uint32_t>& CInvertedIndex::GetIDsOfToken(const absl::string_view token)
{
    auto it = m_invertedIndex.find(token);
    if (it != m_invertedIndex.end())
    {
        return it->second;
    }

    return m_emptyVector;
}

size_t CInvertedIndex::GetUniqueWordCount() const
{
    return m_invertedIndex.size();
}