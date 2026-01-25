//
// Created by haimash on 22/01/2026.
//

#ifndef SEARCHENGINE_CDOCUMENT_H
#define SEARCHENGINE_CDOCUMENT_H

#include <cstdint>
#include <string>

struct CDocument
{
    uint32_t id;
    std::string title;
    std::string text;

    CDocument(uint32_t id, const std::string& title, const std::string& text): id(id), title(title), text(text)
    {}
};

#endif //SEARCHENGINE_CDOCUMENT_H