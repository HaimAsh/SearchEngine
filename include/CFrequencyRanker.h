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
        size_t totalDocs,
        std::vector<uint32_t>& outSortedIds) const override;
};

#endif //SEARCHENGINE_CFREQUENCYRANKER_H