//
// Created by haimash on 27/01/2026.
//
/// @file CBM25Ranker.h
/// @brief Implementation of the BM25 (Best Matching 25) ranking algorithm.

#ifndef SEARCHENGINE_CBM25RANKER_H
#define SEARCHENGINE_CBM25RANKER_H

#include "IRanker.h"


/// @class CBM25Ranker
/// @brief A sophisticated ranker that uses probabilistic relevance modeling.
///
/// This ranker improves upon TF-IDF by adding term frequency saturation
/// and document length normalization using the average document length.
class CBM25Ranker : public IRanker
{
public:

    /// @brief Ranks documents using the BM25 formula with title-field boosting.
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

    bool IsWholeWordMatch(std::string_view title, std::string_view term) const;

    /// Function to find the needle in the haystack as case-insensitive
    /// @param haystack the string to find in
    /// @param needle what is the string to find
    /// @param pos position start in the haystack
    /// @return if found returns the beginning of the needle in the haystack, otherwise return npos
    size_t CaseInsensitiveFind(std::string_view haystack, std::string_view needle, size_t pos = 0) const;

    struct ScoredDoc
    {
        uint32_t ID;
        double Score;
    };
};
#endif //SEARCHENGINE_CBM25RANKER_H