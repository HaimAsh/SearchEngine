//
// Created by haimash on 26/01/2026.
//
/// @file CInvertedIndex.h
/// @brief Core data structure for the search engine, mapping terms to document occurrences.

#ifndef SEARCHENGINE_CINVERTEDINDEX_H
#define SEARCHENGINE_CINVERTEDINDEX_H

#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>
#include "absl/container/flat_hash_map.h"
#include "CTokenizer.h"

using MapIterator = absl::flat_hash_map<std::string, std::vector<std::pair<uint32_t, uint32_t>>>::const_iterator;

/// @class CInvertedIndex
/// @brief Efficiently manages word-to-document mappings and metadata.
///
/// This class uses absl::flat_hash_map for O(1) term lookups and stores
/// document statistics (titles and word counts) required for ranking algorithms.
class CInvertedIndex
{
public:
    /// Adds a token occurrence to the index. If the token exists for the given ID, increments frequency.
    /// @param token the word to add
    /// @param ID the ID of the page the word was found in
    void AddToken(absl::string_view token, uint32_t ID);

    /// Retrieves all document IDs associated with a specific token.
    /// @param token The search term
    /// @return A vector of document IDs containing the term
    [[nodiscard]] std::vector<uint32_t> GetIDsOfToken(const absl::string_view token) const;

    /// Returns the total count of unique words (keys) stored in the index.
    [[nodiscard]] size_t GetUniqueWordCount() const;

    /// Maps a document ID to its original title string
    /// @param ID The document identifier
    /// @param title The title text to associate
    void AddTitle(uint32_t ID, absl::string_view title);

    /// Retrieves the title for a specific document
    /// @param ID The document identifier
    /// @return The title string
    [[nodiscard]] const std::string &GetTitle(uint32_t ID) const;

    [[nodiscard]] size_t GetNumOfDocs() const { return m_docTitles.size(); }

    [[nodiscard]] MapIterator GetIterator(std::string_view query) const
    {
        return m_invertedIndex.find(query);
    }

    [[nodiscard]] MapIterator End() const
    {
        return m_invertedIndex.end();
    }

    const std::vector<uint32_t> &GetDocWordCounts() const
    {
        return m_docWordCounts;
    }

    const std::vector<std::string>& GetDocsTitles() const
    {
        return m_docTitles;
    }

    void MergeLocalIndex(
    absl::flat_hash_map<std::string, std::vector<std::pair<uint32_t, uint32_t>>>&& localMap,
    absl::flat_hash_map<uint32_t, uint32_t>&& localCounts);

    void Finalize();

    /// this function saves the inverted index to a binary file
    /// @param filename the path to the file
    void Save(const std::string& filename) const;

    /// this function loads the inverted index from a binary file
    /// @param filename the path to the file
    /// @return true for success, otherwise false
    [[nodiscard]] bool Load(const std::string& filename);

private:

    /// mapping from word to vector of pairs of {document ID, frequency of word in document}
    absl::flat_hash_map<std::string, std::vector<std::pair<uint32_t, uint32_t>>> m_invertedIndex;

    std::vector<uint32_t> m_emptyVector;

    std::vector<std::string> m_docTitles;

    std::vector<std::pair<uint32_t, uint32_t>> m_emptyPairsVector;

    std::vector<uint32_t> m_docWordCounts;

    std::shared_mutex m_titlesMutex;
    std::mutex m_mapMutex;
};

#endif //SEARCHENGINE_CINVERTEDINDEX_H
