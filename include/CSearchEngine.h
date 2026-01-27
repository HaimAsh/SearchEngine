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

    [[nodiscard]] bool Init(const std::string& filePath, IRanker* ranker);

    void Search(const std::string& query, std::vector<std::string>& hits) const;

    [[nodiscard]] size_t GetWordCount() const { return m_invertedIndex.GetUniqueWordCount(); }

private:
    CInvertedIndex m_invertedIndex;
    IRanker* m_ranker = nullptr;
};

#endif //SEARCHENGINE_CSEARCHENGINE_H
