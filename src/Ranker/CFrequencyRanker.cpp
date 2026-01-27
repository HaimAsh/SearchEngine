//
// Created by haimash on 26/01/2026.
//

#include "CFrequencyRanker.h"

std::vector<uint32_t> CFrequencyRanker::Rank(
        const std::vector<std::pair<uint32_t, uint32_t>>& matches,
        size_t totalDocs) const
{
        std::vector<uint32_t> result;
        result.reserve(matches.size());

        std::vector<std::pair<uint32_t, uint32_t>> sortedMatches = matches;

        std::sort(sortedMatches.begin(), sortedMatches.end(), [](const auto&a, const auto&b)
        {
                return a.second > b.second;
        });

        for (const auto& it : sortedMatches)
        {
                result.push_back(it.first);
        }

        return result;
}