//
// Created by haimash on 22/01/2026.
//

#include "CSearchEngine.h"

bool CSearchEngine::Init(const std::string &filePath)
{
    bool res = false;
    try
    {
        CWikiMediaParser parser([&](const CDocument& doc)
        {
            m_invertedIndex.AddTitle(doc.id, doc.title);

            CTokenizer tokenizer(doc.text);

            absl::string_view token;

            while (tokenizer.HasNext())
            {
                token = tokenizer.Next();

                if (false == token.empty())
                {
                    m_invertedIndex.AddToken(token, doc.id);
                }
            }
        });

        res = parser.ParseFile(filePath);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return res;
}

void CSearchEngine::Search(const std::string &query, std::vector<std::string>& hits) const
{
    std::vector<TermPostings> queryResults;
    CTokenizer tokenizer(query);

    while (tokenizer.HasNext())
    {
        std::string_view termView = tokenizer.Next();

        // 1. Find the word in the map
        auto it = m_invertedIndex.GetIterator(termView);

        if (m_invertedIndex.End() != it)
        {
            // 2. Use the map's internal string as the source for our string_view.
            // This ensures the view is valid for the life of the engine.
            queryResults.push_back(
            {
                std::string_view(it->first), // The stable key in the map
                it->second                   // The vector of {ID, Freq}
            });
        }
    }

    hits.clear();
    hits.reserve(queryResults.size());

    std::vector<uint32_t> sortedRes;
    m_ranker->Rank(queryResults, m_invertedIndex.GetDocWordCounts(), m_invertedIndex.GetDocsTitles(), sortedRes);

    for (auto& iter : sortedRes)
    {
        hits.push_back(m_invertedIndex.GetTitle(iter));
    }
}