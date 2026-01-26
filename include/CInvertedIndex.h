//
// Created by haimash on 26/01/2026.
//

#ifndef SEARCHENGINE_CINVERTEDINDEX_H
#define SEARCHENGINE_CINVERTEDINDEX_H

#include <string>
#include <vector>
#include "absl/container/flat_hash_map.h"
#include "CTokenizer.h"

class CInvertedIndex
{
public:
    void AddToken(absl::string_view token, uint32_t ID);
    const std::vector<uint32_t>& GetIDsOfToken(const absl::string_view token) const;

    [[nodiscard]] std::vector<uint32_t> Search(absl::string_view searchString) const;

    [[nodiscard]] size_t GetUniqueWordCount() const;

    void AddTitle(uint32_t ID, absl::string_view title);

    const std::string &GetTitle(uint32_t ID) const;

private:

    static void Intersect(const std::vector<uint32_t>& v1,
                   const std::vector<uint32_t>& v2,
                   std::vector<uint32_t>& result);

    absl::flat_hash_map<std::string, std::vector<uint32_t>> m_invertedIndex;

    std::vector<uint32_t> m_emptyVector;

    std::vector<std::string> m_docTitles;
};

#endif //SEARCHENGINE_CINVERTEDINDEX_H
