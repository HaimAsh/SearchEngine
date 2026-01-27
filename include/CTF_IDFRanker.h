//
// Created by haimash on 26/01/2026.
//
/// @file CTF_IDFRanker.h
/// @brief Implementation of the TF-IDF ranking algorithm.
#ifndef SEARCHENGINE_CTF_IDFRANKER_H
#define SEARCHENGINE_CTF_IDFRANKER_H

#include "IRanker.h"

class CTF_IDFRanker : public IRanker
{
public:

    /// @brief Ranks documents using the TF-IDF formula.
    /// @param queryPosting Data retrieved from the inverted index for the query.
    /// @param docLengths Vector of total word counts per document.
    /// @param docTitles Vector of document titles for field-specific weighting.
    /// @param outSortedIds [out] Result vector sorted from most to least relevant.
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
