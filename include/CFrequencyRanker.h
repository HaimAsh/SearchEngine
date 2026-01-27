//
// Created by haimash on 26/01/2026.
//
/// @file CFrequencyRanker.h
/// @brief Implementation of the frequency ranking algorithm.
#ifndef SEARCHENGINE_CFREQUENCYRANKER_H
#define SEARCHENGINE_CFREQUENCYRANKER_H

#include "IRanker.h"

class CFrequencyRanker : public IRanker
{
public:
        /// @brief Ranks documents solely on the frequency of the word in the document
        /// @param queryPosting Data retrieved from the inverted index for the query.
        /// @param docLengths Vector of total word counts per document.
        /// @param docTitles Vector of document titles for field-specific weighting.
        /// @param outSortedIds [out] Result vector sorted from most to least relevant.
        void Rank(
        const std::vector<TermPostings>& queryPosting,
        const std::vector<uint32_t>& docLengths,
        const std::vector<std::string>& docTitles,
        std::vector<uint32_t>& outSortedIds) const override;
};

#endif //SEARCHENGINE_CFREQUENCYRANKER_H