//
// Created by haimash on 26/01/2026.
//

#include "CInvertedIndex.h"

#include <fstream>

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
            for (const auto& [id, count] : localCounts)
            {
                if (id >= m_docWordCounts.size()) {
                    m_docWordCounts.resize(id + 500, 0);
                }
                m_docWordCounts[id] = count;
            }
        }

        // 2. Merge Map
        {
            std::unique_lock lock(m_mapMutex);
            for (auto& [token, localPostings] : localMap)
            {
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
    for (auto& [token, postings] : m_invertedIndex)
        {
        std::sort(postings.begin(), postings.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });
    }
}

void CInvertedIndex::Save(const std::string &filename) const
{
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs)
    {
        return;
    }

    // 1. Header: Magic Number and Version
    uint32_t magic = 0xBADCAFFE;
    uint32_t version = 1;
    ofs.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
    ofs.write(reinterpret_cast<const char*>(&version), sizeof(version));

    // 2. Titles Vector
    uint32_t titleCount = m_docTitles.size();
    ofs.write(reinterpret_cast<const char*>(&titleCount), sizeof(titleCount));
    for (const auto& title : m_docTitles)
    {
        uint32_t len = title.size();
        ofs.write(reinterpret_cast<const char*>(&len), sizeof(len));
        ofs.write(title.data(), len);
    }

    // 3. Word Counts Vector (Bulk Write)
    uint32_t wordCountSize = m_docWordCounts.size();
    ofs.write(reinterpret_cast<const char*>(&wordCountSize), sizeof(wordCountSize));
    ofs.write(reinterpret_cast<const char*>(m_docWordCounts.data()),
              wordCountSize * sizeof(uint32_t));

    // 4. Inverted Index Map
    uint32_t uniqueWords = m_invertedIndex.size();
    ofs.write(reinterpret_cast<const char*>(&uniqueWords), sizeof(uniqueWords));
    for (const auto& [word, postings] : m_invertedIndex)
    {
        // Write word
        uint32_t wordLen = word.size();
        ofs.write(reinterpret_cast<const char*>(&wordLen), sizeof(wordLen));
        ofs.write(word.data(), wordLen);

        // Write postings (Bulk Write)
        uint32_t postCount = postings.size();
        ofs.write(reinterpret_cast<const char*>(&postCount), sizeof(postCount));
        ofs.write(reinterpret_cast<const char*>(postings.data()),
                  postCount * sizeof(std::pair<uint32_t, uint32_t>));
    }
}

bool CInvertedIndex::Load(const std::string &filename)
 {
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs)
        {
            return false;
        }

        // 1. Check Header
        uint32_t magic, version;
        ifs.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        ifs.read(reinterpret_cast<char*>(&version), sizeof(version));

        if (magic != 0xBADCAFFE)
        {
            return false;
        }

        // 2. Load Titles
        uint32_t titleCount;
        ifs.read(reinterpret_cast<char*>(&titleCount), sizeof(titleCount));
        m_docTitles.clear();
        m_docTitles.reserve(titleCount);
        for (uint32_t i = 0; i < titleCount; ++i)
        {
            uint32_t len;
            ifs.read(reinterpret_cast<char*>(&len), sizeof(len));
            std::string title(len, ' ');
            ifs.read(title.data(), len);
            m_docTitles.push_back(std::move(title));
        }

        // 3. Load Word Counts (Bulk Read)
        uint32_t wordCountSize;
        ifs.read(reinterpret_cast<char*>(&wordCountSize), sizeof(wordCountSize));
        m_docWordCounts.resize(wordCountSize);
        ifs.read(reinterpret_cast<char*>(m_docWordCounts.data()),
                 wordCountSize * sizeof(uint32_t));

        // 4. Load Inverted Index
        uint32_t uniqueWords;
        ifs.read(reinterpret_cast<char*>(&uniqueWords), sizeof(uniqueWords));
        m_invertedIndex.clear();
        m_invertedIndex.reserve(uniqueWords);
        for (uint32_t i = 0; i < uniqueWords; ++i)
        {
            // Read word
            uint32_t wordLen;
            ifs.read(reinterpret_cast<char*>(&wordLen), sizeof(wordLen));
            std::string word(wordLen, ' ');
            ifs.read(word.data(), wordLen);

            // Read postings (Bulk Read)
            uint32_t postCount;
            ifs.read(reinterpret_cast<char*>(&postCount), sizeof(postCount));
            std::vector<std::pair<uint32_t, uint32_t>> postings(postCount);
            ifs.read(reinterpret_cast<char*>(postings.data()),
                     postCount * sizeof(std::pair<uint32_t, uint32_t>));

            m_invertedIndex[std::move(word)] = std::move(postings);
        }

        return true;
    }