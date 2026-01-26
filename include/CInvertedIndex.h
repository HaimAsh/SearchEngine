//
// Created by haimash on 26/01/2026.
//

#ifndef SEARCHENGINE_CINVERTEDINDEX_H
#define SEARCHENGINE_CINVERTEDINDEX_H

#include "absl/container/flat_hash_map.h"
#include <vector>
#include <string>

class CInvertedIndex
{
public:
    void AddToken(absl::string_view token, uint32_t ID);
    const std::vector<uint32_t>& GetIDsOfToken(const absl::string_view token);

    [[nodiscard]] size_t GetUniqueWordCount() const;
private:
    absl::flat_hash_map<std::string, std::vector<uint32_t>> m_invertedIndex;

    std::vector<uint32_t> m_emptyVector;
};

#endif //SEARCHENGINE_CINVERTEDINDEX_H
