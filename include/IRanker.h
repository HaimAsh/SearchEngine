//
// Created by haimash on 26/01/2026.
//

#ifndef SEARCHENGINE_IRANKER_H
#define SEARCHENGINE_IRANKER_H

#include <vector>
#include <cstdint>

class IRanker {
public:
    virtual ~IRanker() = default;

    // Returns a sorted list of Document IDs
    virtual void Rank(
        const std::vector<std::pair<uint32_t, uint32_t>>& matches,
        size_t totalDocs,
        std::vector<uint32_t>& outSortedIds) const = 0;
};

#endif //SEARCHENGINE_IRANKER_H