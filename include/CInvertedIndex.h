//
// Created by haimash on 26/01/2026.
//

#ifndef SEARCHENGINE_CINVERTEDINDEX_H
#define SEARCHENGINE_CINVERTEDINDEX_H

#include <string>
#include <vector>
#include "absl/container/flat_hash_map.h"
#include "CTokenizer.h"

using MapIterator = absl::flat_hash_map<std::string, std::vector<std::pair<uint32_t, uint32_t>>>::const_iterator;

class CInvertedIndex
{
public:
    void AddToken(absl::string_view token, uint32_t ID);
    [[nodiscard]] std::vector<uint32_t> GetIDsOfToken(const absl::string_view token) const;

    [[nodiscard]] size_t GetUniqueWordCount() const;

    void AddTitle(uint32_t ID, absl::string_view title);

    [[nodiscard]] const std::string &GetTitle(uint32_t ID) const;

    [[nodiscard]] const std::vector<std::pair<uint32_t, uint32_t>> &GetPairsOfToken(absl::string_view token) const;

    [[nodiscard]] size_t GetNumOfDocs() const { return m_numOfDocs; }

    MapIterator GetIterator(std::string_view query) const
    {
        return m_invertedIndex.find(query);
    }

    MapIterator End() const
    {
        return m_invertedIndex.end();
    }

    const std::vector<uint32_t> &GetDocWordCounts() const
    {
        return m_docWordCounts;
    }

    const std::vector<std::string>& GetDocsTitles() const
    {
        return m_docTitles;
    }

private:

    absl::flat_hash_map<std::string, std::vector<std::pair<uint32_t, uint32_t>>> m_invertedIndex;

    std::vector<uint32_t> m_emptyVector;

    std::vector<std::string> m_docTitles;

    size_t m_numOfDocs = 0;

    std::vector<std::pair<uint32_t, uint32_t>> m_emptyPairsVector;

    std::vector<uint32_t> m_docWordCounts;
};

#endif //SEARCHENGINE_CINVERTEDINDEX_H
