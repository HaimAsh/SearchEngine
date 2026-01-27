//
// Created by haimash on 26/01/2026.
//

#ifndef SEARCHENGINE_CFREQUENCYRANKER_H
#define SEARCHENGINE_CFREQUENCYRANKER_H

#include "IRanker.h"

class CFrequencyRanker : public IRanker
{
    [[nodiscard]] std::vector<uint32_t> Rank(
        const std::vector<std::pair<uint32_t, uint32_t>>& matches,
        size_t totalDocs) const override;
};

#endif //SEARCHENGINE_CFREQUENCYRANKER_H