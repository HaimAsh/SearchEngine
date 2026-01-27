//
// Created by haimash on 26/01/2026.
//

#ifndef SEARCHENGINE_IRANKER_H
#define SEARCHENGINE_IRANKER_H

#include <vector>
#include <cstdint>
#include <string>

// This struct holds the data for one specific word from the query
struct TermPostings {
    std::string_view term;
    std::vector<std::pair<uint32_t, uint32_t>> postings;
};

class IRanker {
public:
    virtual ~IRanker() = default;

    // Returns a sorted list of Document IDs
    virtual void Rank(
        const std::vector<TermPostings>& queryPosting,
        const std::vector<uint32_t>& docLengths,
        const std::vector<std::string>& docTitles,
        std::vector<uint32_t>& outSortedIds) const = 0;
};

#endif //SEARCHENGINE_IRANKER_H