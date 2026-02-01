//
// Created by haimash on 26/01/2026.
//

#include "CInvertedIndex.h"

void CInvertedIndex::AddToken(absl::string_view token, uint32_t ID)
{
    std::unique_lock unique_lock(m_mapMutex);
    // 1. Ensure the vector is large enough to hold this ID.
    if (ID >= m_docWordCounts.size()) {
        m_docWordCounts.resize(ID + 500, 0);
    }

    std::vector<std::pair<uint32_t, uint32_t>>& IDs = m_invertedIndex[token];

    // Assumes tokens are added in increasing document ID order
    if (IDs.empty() || IDs.back().first != ID)
    {
        IDs.emplace_back(ID, 1);
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
    /// we open another scope so the shared lock will be destroyed if resize is needed
    {
        /// since ID qre unique if threads will try to write to the vector, each thread will write to different
        /// index, that is thread-safe
        std::shared_lock lock(m_titlesMutex);
        if (ID < m_docTitles.size())
        {
            m_docTitles[ID] = std::string(title);
            return;
        }
    }

    /// if we reach this part of the code then resize is needed so we lock the possabilty to write to the vector
    std::unique_lock unique_lock(m_titlesMutex);
    m_docTitles.resize(ID + 1000);
    m_docTitles[ID] = std::string(title);
}

const std::string& CInvertedIndex::GetTitle(uint32_t ID) const
{
    return m_docTitles[ID];
}

void CInvertedIndex::MergeLocalIndex(
    absl::flat_hash_map<std::string, std::vector<std::pair<uint32_t, uint32_t>>> &&localMap,
    absl::flat_hash_map<uint32_t, uint32_t> &&localCounts)
{
    {
        // 1. Merge Word Counts
        {
            std::unique_lock lock(m_titlesMutex);
            for (const auto& [id, count] : localCounts) {
                if (id >= m_docWordCounts.size()) {
                    m_docWordCounts.resize(id + 500, 0);
                }
                m_docWordCounts[id] = count;
            }
        }

        // 2. Merge Map
        {
            std::unique_lock lock(m_mapMutex);
            for (auto& [token, localPostings] : localMap) {
                auto& globalPostings = m_invertedIndex[token];

                // Efficiently move the vector contents
                globalPostings.insert(
                    globalPostings.end(),
                    std::make_move_iterator(localPostings.begin()),
                    std::make_move_iterator(localPostings.end())
                );
            }
        }
    }
}

void CInvertedIndex::Finalize()
{
    // Parallel sort is best if you have many unique words!
    for (auto& [token, postings] : m_invertedIndex) {
        std::sort(postings.begin(), postings.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });
    }
}
