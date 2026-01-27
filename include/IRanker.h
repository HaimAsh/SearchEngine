//
// Created by haimash on 26/01/2026.
//
/// @file IRanker.h
/// @brief Abstract base class for search relevance scoring algorithms.

#ifndef SEARCHENGINE_IRANKER_H
#define SEARCHENGINE_IRANKER_H

#include <vector>
#include <cstdint>
#include <string>

/// @struct TermPostings
/// @brief Encapsulates a search term and its associated document occurrences
struct TermPostings {
    std::string_view term;
    std::vector<std::pair<uint32_t, uint32_t>> postings;
};

/// @class IRanker
/// @brief Interface for ranking strategies.
/// Subclasses must implement the Rank method to assign scores to documents
/// based on term statistics and metadata.
class IRanker {
public:
    virtual ~IRanker() = default;

    /// ranks the results
    /// @param queryPosting Pre-filtered list of documents containing the query terms.
    /// @param docLengths Pre-calculated word counts for every document in the index.
    /// @param docTitles Titles used for field-specific boosting (e.g., title matching).
    /// @param outSortedIds [out] Vector to be Cleared and filled with document IDs sorted by relevance.
    virtual void Rank(
        const std::vector<TermPostings>& queryPosting,
        const std::vector<uint32_t>& docLengths,
        const std::vector<std::string>& docTitles,
        std::vector<uint32_t>& outSortedIds) const = 0;
};

#endif //SEARCHENGINE_IRANKER_H