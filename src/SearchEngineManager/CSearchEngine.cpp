//
// Created by haimash on 22/01/2026.
//

#include "CSearchEngine.h"

std::string GetBinaryPath(const std::string& xmlPath)
{
    std::filesystem::path p(xmlPath);
    return p.replace_extension(".bin").string();
}

bool CSearchEngine::Init(const std::string &filePath)
{
    bool res = false;

    std::string binPath = GetBinaryPath(filePath);
    if (std::filesystem::exists(binPath))
    {
        res = m_invertedIndex.Load(binPath);
        if (res)
        {
            return true;
        }
        // If Load fails (corrupt file), we log it and continue to the XML parser
        std::cerr << "Warning: Binary index corrupt. Rebuilding from XML..." << std::endl;
    }

    try
    {
        CWikiMediaParser parser([&](uint32_t id, absl::string_view title) {
        m_invertedIndex.AddTitle(id, title);
        }, [&](auto&& localMap, auto&& localCounts) {
            m_invertedIndex.MergeLocalIndex(std::move(localMap), std::move(localCounts));
        });

        res = parser.ParseFile(filePath);

        m_invertedIndex.Finalize();
        m_invertedIndex.Save(binPath);
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