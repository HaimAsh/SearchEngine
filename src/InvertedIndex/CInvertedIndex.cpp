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

const std::vector<uint32_t>& CInvertedIndex::GetIDsOfToken(const absl::string_view token) const
{
    auto it = m_invertedIndex.find(token);
    if (it != m_invertedIndex.end())
    {
        return it->second;
    }

    return m_emptyVector;
}

std::vector<uint32_t> CInvertedIndex::Search(absl::string_view searchString) const
{
    std::vector<uint32_t> result;
    std::vector<uint32_t> temp1;
    std::vector<uint32_t> temp2;
    std::vector<std::string> tokens;
    tokens.reserve(32);

    CTokenizer tokenizer(searchString);

    while (tokenizer.HasNext())
    {
        absl::string_view t = tokenizer.Next();
        if (!t.empty()) { // ONLY add non-empty tokens!
            tokens.emplace_back(t);
        }
    }

    if (tokens.empty())
    {
        return m_emptyVector;
    }

    const auto& firstList = GetIDsOfToken(tokens.back());
    result.assign(firstList.begin(), firstList.end());
    tokens.pop_back();

    while (false == tokens.empty())
    {

        if (result.empty())
        {
            return m_emptyVector;
        }

        temp2 = GetIDsOfToken(tokens.back());
        tokens.pop_back();

        Intersect(result, temp2, temp1);
        std::swap(result, temp1);
        temp1.clear();
    }

    return result;

}

size_t CInvertedIndex::GetUniqueWordCount() const
{
    return m_invertedIndex.size();
}

void CInvertedIndex::AddTitle(uint32_t ID, absl::string_view title)
{
    if (ID >= m_docTitles.size()) {
        m_docTitles.resize(ID + 1);
    }
    m_docTitles[ID] = std::string(title);
}

const std::string& CInvertedIndex::GetTitle(uint32_t ID) const
{
    return m_docTitles[ID];
}

void CInvertedIndex::Intersect(const std::vector<uint32_t> &v1,
    const std::vector<uint32_t> &v2, std::vector<uint32_t>& result)
{
    auto v1Iter = v1.begin();
    auto v2Iter = v2.begin();

    while (v1Iter != v1.end() && v2Iter != v2.end())
    {
        if (*v1Iter == *v2Iter)
        {
            result.push_back(*v1Iter);
            ++v1Iter;
            ++v2Iter;
        }
        else if (*v1Iter < *v2Iter)
        {
            ++v1Iter;
        }
        else
        {
            ++v2Iter;
        }
    }
}
