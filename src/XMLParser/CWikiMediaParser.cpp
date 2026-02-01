//
// Created by haimash on 22/01/2026.
//

#include <libxml/xmlreader.h>
#include "CWikiMediaParser.h"

#include "CTokenizer.h"

bool CWikiMediaParser::ParseFile(const std::string& filePath)
{
    // Define a unique_ptr that calls the libxml2 closer automatically
    std::unique_ptr<_xmlTextReader, void(*)(xmlTextReaderPtr)> reader(
        xmlNewTextReaderFilename(filePath.c_str()),
        [](xmlTextReaderPtr r) { if(r) xmlFreeTextReader(r); }
    );

    if (reader == nullptr)
    {
        return false;
    }

    // Start worker threads
    uint32_t numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0)
    {
        numThreads = 4; // Default to 4 if unable to detect
    }

    m_threads.reserve(numThreads);

    for (uint32_t i = 0; i < numThreads; ++i)
    {
        m_threads.emplace_back(&CWikiMediaParser::ThreadWorkingFunction, this);
    }


    LIBXML_READER_RETURN_VALUES ret = static_cast<LIBXML_READER_RETURN_VALUES>(xmlTextReaderRead(reader.get()));

    while (LIBXML_READER_RETURN_VALUES::SUCCESS == ret)
    {
        if (0 == xmlTextReaderDepth(reader.get()))
        {
            ret = static_cast<LIBXML_READER_RETURN_VALUES>(xmlTextReaderRead(reader.get()));
            continue;
        }

        if (static_cast<LIB_NODE_TYPES> (xmlTextReaderNodeType(reader.get())) == LIB_NODE_TYPES::START_NODE &&
            xmlStrEqual(xmlTextReaderConstName(reader.get()), BAD_CAST "page"))
        {
            LIBXML_READER_RETURN_VALUES innerRet = static_cast<LIBXML_READER_RETURN_VALUES>(xmlTextReaderRead(reader.get()));

            CDocument currentDoc{0, "", ""};
            bool isMainNs = false;
            bool nsFound = false;
            bool idFound = false;

            while (innerRet == LIBXML_READER_RETURN_VALUES::SUCCESS)
            {
                auto innerType = static_cast<LIB_NODE_TYPES>(xmlTextReaderNodeType(reader.get()));
                const xmlChar* innerName = xmlTextReaderConstName(reader.get());

                if (innerType == LIB_NODE_TYPES::START_NODE)
                {
                    if (xmlStrEqual(innerName, BAD_CAST "title"))
                    {
                        currentDoc.title = ReadElementText(reader.get());
                    }

                    else if (false == idFound && xmlStrEqual(innerName, BAD_CAST "id"))
                    {
                        std::string idStr = ReadElementText(reader.get());
                        currentDoc.id = static_cast<uint32_t>(std::stoul(idStr));
                        idFound = true;
                    }

                    else if (xmlStrEqual(innerName, BAD_CAST "ns"))
                    {
                        std::string nsVal = ReadElementText(reader.get());
                        if (nsVal == "0")
                        {
                            isMainNs = true;
                        }
                        else
                        {
                            isMainNs = false;
                            break;
                        }
                        nsFound = true;
                    }
                    else if (xmlStrEqual(innerName, BAD_CAST "text"))
                    {
                        currentDoc.text = ReadElementText(reader.get());
                    }
                }

                // 3. Exit inner loop when we hit </page>
                if (innerType == LIB_NODE_TYPES::END_NODE && xmlStrEqual(innerName, BAD_CAST "page")) {
                    break;
                }
                innerRet = static_cast<LIBXML_READER_RETURN_VALUES>(xmlTextReaderRead(reader.get()));
            }

            if (isMainNs && nsFound)
            {
                m_documentQueue.Push(std::move(currentDoc));
            }
        }

        ret = static_cast<LIBXML_READER_RETURN_VALUES>(xmlTextReaderRead(reader.get()));
    }

    m_documentQueue.SignalFinished();

    // NEW: Wait for all workers to finish their loops and call the merge callback
    for (auto& thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    return true;
}

void CWikiMediaParser::ThreadWorkingFunction(void *arg)
{
    auto parser = static_cast<CWikiMediaParser*>(arg);
    CDocument doc = {0, "", ""};

    // Every thread gets its own private index
    absl::flat_hash_map<std::string, std::vector<std::pair<uint32_t, uint32_t>>> localMap;
    // We also need local word counts to avoid locking m_titlesMutex constantly
    absl::flat_hash_map<uint32_t, uint32_t> localWordCounts;

    while (parser->m_documentQueue.Pop(doc))
    {
        // 1. Add title to global index
        // because titles are unique per ID, this is fast.
        parser->m_pageCallbackForTitle(doc.id, doc.title);

        // 2. Tokenize into the LOCAL map
        CTokenizer tokenizer(doc.text);
        while (tokenizer.HasNext())
        {
            auto token = tokenizer.Next();
            if (!token.empty())
            {
                auto& postings = localMap[std::string(token)];
                if (postings.empty() || postings.back().first != doc.id) {
                    postings.emplace_back(doc.id, 1);
                } else {
                    postings.back().second++;
                }
                localWordCounts[doc.id]++;
            }
        }
    }

    // 3. FINAL MERGE: Now we call a new function in CInvertedIndex
    // to merge this localMap into the global one.
    parser->m_mergeCallback(std::move(localMap), std::move(localWordCounts));
}

std::string CWikiMediaParser::ReadElementText(xmlTextReaderPtr reader)
{
    xmlChar* text = xmlTextReaderReadString(reader);
    std::string result;
    if (text)
    {
        result = reinterpret_cast<const char*>(text);
        xmlFree(text); // libxml2 requires manual free
    }
    return result;
}
