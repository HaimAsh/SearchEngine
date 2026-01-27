//
// Created by haimash on 22/01/2026.
//

#ifndef SEARCHENGINE_CSEARCHENGINE_H
#define SEARCHENGINE_CSEARCHENGINE_H

#include "CWikiMediaParser.h"
#include "CTokenizer.h"
#include "CInvertedIndex.h"
#include "IRanker.h"

class CSearchEngine
{
public:
    CSearchEngine() = default;
    ~CSearchEngine() = default;

    CSearchEngine& operator=(const CSearchEngine &) = delete;
    CSearchEngine(const CSearchEngine &) = delete;

    /// Initialize the search engine with the file to search in
    /// @param filePath the path to the file
    /// @return true for success, false otherwise
    [[nodiscard]] bool Init(const std::string& filePath);

    /// search for the word in the file
    /// @param query the word to search
    /// @param hits Reference to vector of strings, the titles of matching pages will be stored in
    void Search(const std::string& query, std::vector<std::string>& hits) const;

    /// get the number of unique words found in the file
    /// @return The number
    [[nodiscard]] size_t GetWordCount() const { return m_invertedIndex.GetUniqueWordCount(); }

    /// set the ranking algorithm
    /// @param newRanker pointer the ranking algorithm instance
    void SetRanker(std::unique_ptr<IRanker> newRanker)
    {
        m_ranker = std::move(newRanker);
    }

private:
    CInvertedIndex m_invertedIndex;
    std::unique_ptr<IRanker> m_ranker = nullptr;
};

#endif //SEARCHENGINE_CSEARCHENGINE_H
