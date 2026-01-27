//
// Created by haimash on 26/01/2026.
//

#ifndef SEARCHENGINE_CFREQUENCYRANKER_H
#define SEARCHENGINE_CFREQUENCYRANKER_H

#include "IRanker.h"

class CFrequencyRanker : public IRanker
{
public:
        void Rank(
        const std::vector<TermPostings>& queryPosting,
        const std::vector<uint32_t>& docLengths,
        const std::vector<std::string>& docTitles,
        std::vector<uint32_t>& outSortedIds) const override;
};

#endif //SEARCHENGINE_CFREQUENCYRANKER_H