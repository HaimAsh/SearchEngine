//
// Created by haimash on 22/01/2026.
//

#include "CSearchEngine.h"

bool CSearchEngine::Init(const std::string &filePath)
{
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

        parser.ParseFile(filePath);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

void CSearchEngine::Search(const std::string &query, std::vector<std::string>& hits) const
{
    std::vector<std::pair<uint32_t, uint32_t>> res = m_invertedIndex.Search(query);
    hits.clear();
    hits.reserve(res.size());

    for (auto& iter : res)
    {
        hits.push_back(m_invertedIndex.GetTitle(iter.first));
    }
}