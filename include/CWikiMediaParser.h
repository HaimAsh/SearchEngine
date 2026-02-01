//
// Created by haimash on 22/01/2026.
//
/// @file CWikiMediaParser.h
/// @brief Pareses wikimedia files
#ifndef SEARCHENGINE_CXMLPARSER_H
#define SEARCHENGINE_CXMLPARSER_H

#include <functional>
#include <string>
#include <thread>

#include "CThreadSafeQueue.h"
#include "CDocument.h"
#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"

// 1. Forward declare the struct
struct _xmlTextReader;

// 2. Define the pointer type matching the libxml2 typedef
typedef struct _xmlTextReader *xmlTextReaderPtr;

class CWikiMediaParser
{
public:

    using PageCallback = std::function<void(CDocument)>;
    // Callback 1: For titles (called immediately by workers)
    using TitleCallback = std::function<void(uint32_t, absl::string_view)>;

    // Callback 2: For merging (called once per thread at the end)
    using MergeCallback = std::function<void(
        absl::flat_hash_map<std::string, std::vector<std::pair<uint32_t, uint32_t>>>&&,
        absl::flat_hash_map<uint32_t, uint32_t>&&)>;

    explicit CWikiMediaParser(TitleCallback tCb, MergeCallback mCb)
        : m_pageCallbackForTitle(std::move(tCb)), m_mergeCallback(std::move(mCb)) {}

    /// the function parses the wikimedia file and for each page found uses the callback function passed in the ctor
    /// @param filePath the path to the wikimedia file
    /// @return true for success, otherwise false
    [[nodiscard]] bool ParseFile(const std::string& filePath);

    ~CWikiMediaParser() = default;

    CWikiMediaParser(CWikiMediaParser const&) = delete;
    CWikiMediaParser& operator=(CWikiMediaParser const&) = delete;

    static void ThreadWorkingFunction(void *arg);

private:

    /// reads an xml string, stores it in a string and frees it after finishes
    /// @param reader pointer for the xml reader
    /// @return the string that was read by the xml reader
    std::string ReadElementText(xmlTextReaderPtr reader);

    PageCallback m_pageCallback;
    TitleCallback m_pageCallbackForTitle;
    MergeCallback m_mergeCallback;

    CThreadSafeQueue<CDocument> m_documentQueue;
    std::vector<std::jthread> m_threads;

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