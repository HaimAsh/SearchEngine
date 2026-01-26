//
// Created by haimash on 25/01/2026.
//

#ifndef SEARCHENGINE_CTOKENIZER_H
#define SEARCHENGINE_CTOKENIZER_H

#include <string>
#include "absl/strings/string_view.h"

class CTokenizer
{
public:

    explicit CTokenizer(std::string_view text) noexcept;


    [[nodiscard]] bool HasNext() const;
    std::string_view Next();

private:

    void FindWord();

    std::string_view m_text;
    size_t m_textIndex = 0;
    absl::string_view m_delimiters = " \n\t\r.,;:!?\"'()[]{}<>-_/\\|`~@#$%^&*+=\0";
    absl::string_view m_currentToken;

    std::string m_normalizeWord;
};

#endif //SEARCHENGINE_CTOKENIZER_H