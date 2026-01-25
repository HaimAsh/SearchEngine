//
// Created by haimash on 22/01/2026.
//

#include <libxml/xmlreader.h>
#include "CWikiMediaParser.h"

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
                m_pageCallback(std::move(currentDoc));
            }
        }

        ret = static_cast<LIBXML_READER_RETURN_VALUES>(xmlTextReaderRead(reader.get()));
    }

    return true;
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