//
// Created by haimash on 26/01/2026.
//

#ifndef SEARCHENGINE_CTF_IDFRANKER_H
#define SEARCHENGINE_CTF_IDFRANKER_H

#include "IRanker.h"

class CTF_IDFRanker : public IRanker
{
public:
    void Rank(
        const std::vector<TermPostings>& queryPosting,
        const std::vector<uint32_t>& docLengths,
        const std::vector<std::string>& docTitles,
        std::vector<uint32_t>& outSortedIds) const override;

private:
    struct ScoredDoc
    {
        uint32_t ID;
        double Score;
    };
};
#endif //SEARCHENGINE_CTF_IDFRANKER_H
