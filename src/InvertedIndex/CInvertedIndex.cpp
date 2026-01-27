//
// Created by haimash on 26/01/2026.
//

#include "CInvertedIndex.h"

void CInvertedIndex::AddToken(absl::string_view token, uint32_t ID)
{
    std::vector<std::pair<uint32_t, uint32_t>>& IDs = m_invertedIndex[token];

    if (IDs.empty() || IDs.back().first != ID)
    {
        IDs.emplace_back(ID, 0);
    }
    else
    {
        IDs.back().second++;
    }
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

const std::vector<std::pair<uint32_t, uint32_t>>&
CInvertedIndex::GetPairsOfToken(const absl::string_view token) const
{
    auto it = m_invertedIndex.find(token);
    if (it != m_invertedIndex.end())
    {
        return it->second; // Return the reference directly
    }

    // We need an empty vector of pairs as a static member to return if not found
    return m_emptyPairsVector;
}

void CInvertedIndex::Intersect(const std::vector<std::pair<uint32_t, uint32_t>> &v1,
    const std::vector<std::pair<uint32_t, uint32_t>> &v2, std::vector<std::pair<uint32_t, uint32_t>>& result)
{
    auto v1Iter = v1.begin();
    auto v2Iter = v2.begin();

    while (v1Iter != v1.end() && v2Iter != v2.end())
    {
        if (v1Iter->first == v2Iter->first)
        {
            result.emplace_back(v1Iter->first, v1Iter->second + v2Iter->second);
            ++v1Iter;
            ++v2Iter;
        }
        else if (v1Iter->first < v2Iter->first)
        {
            ++v1Iter;
        }
        else
        {
            ++v2Iter;
        }
    }
}
