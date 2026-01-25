//
// Created by haimash on 22/01/2026.
//

#ifndef SEARCHENGINE_CXMLPARSER_H
#define SEARCHENGINE_CXMLPARSER_H

#include <functional>
#include <string>

#include "CDocument.h"

// 1. Forward declare the struct
struct _xmlTextReader;

// 2. Define the pointer type matching the libxml2 typedef
typedef struct _xmlTextReader *xmlTextReaderPtr;

class CWikiMediaParser
{
public:

    using PageCallback = std::function<void(CDocument)>;

    bool ParseFile(const std::string& filePath);

    std::string ReadElementText(xmlTextReaderPtr reader);

    explicit CWikiMediaParser(PageCallback callback) : m_pageCallback(std::move(callback))
    {}

    ~CWikiMediaParser() = default;

    CWikiMediaParser(CWikiMediaParser const&) = delete;
    CWikiMediaParser& operator=(CWikiMediaParser const&) = delete;

private:
    PageCallback m_pageCallback;

    enum class LIB_NODE_TYPES
    {
        START_NODE = 1,
        END_NODE = 15
    };

    enum class WIKIMEDIA_NS
    {
        MAIN_ARTICLE = 0,
        TALK_ARTICLE = 1,
        USER = 2,
        USER_TALK = 3,
    };

    enum class LIBXML_READER_RETURN_VALUES
    {
        SUCCESS = 1,
        END = 0,
        FAILURE = -1
    };

    static constexpr std::string_view m_page = "page";
};


#endif //SEARCHENGINE_CXMLPARSER_H