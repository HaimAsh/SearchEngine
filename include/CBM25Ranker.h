//
// Created by haimash on 27/01/2026.
//

#ifndef SEARCHENGINE_CBM25RANKER_H
#define SEARCHENGINE_CBM25RANKER_H

#include "IRanker.h"

class CBM25Ranker : public IRanker
{
public:
    void Rank(
        const std::vector<TermPostings>& queryPosting,
        const std::vector<uint32_t>& docLengths,
        const std::vector<std::string>& docTitles,
        std::vector<uint32_t>& outSortedIds) const override;

private:

    bool IsWholeWordMatch(std::string_view title, std::string_view term) const;
    size_t CaseInsensitiveFind(std::string_view haystack, std::string_view needle, size_t pos = 0) const;

    struct ScoredDoc
    {
        uint32_t ID;
        double Score;
    };
};
#endif //SEARCHENGINE_CBM25RANKER_H