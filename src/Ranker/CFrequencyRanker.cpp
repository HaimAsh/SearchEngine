//
// Created by haimash on 26/01/2026.
//

#include "CFrequencyRanker.h"

#include "absl/container/flat_hash_map.h"

void CFrequencyRanker::Rank(
        const std::vector<TermPostings>& queryPosting,
        const std::vector<uint32_t>& docLengths,
        std::vector<uint32_t>& outSortedIds) const
{
        outSortedIds.clear();
        outSortedIds.reserve(queryPosting.size());

        if (queryPosting.empty())
        {
                return;
        }

        absl::flat_hash_map<uint32_t, uint32_t> intersectedRes;

        for (const auto& term : queryPosting)
        {
                for (const auto& [docId, sum] : term.postings)
                {
                        intersectedRes[docId] += sum;
                }
        }

        std::vector<std::pair<uint32_t, uint32_t>> sortedMatches;
        sortedMatches.reserve(intersectedRes.size());

        for (const auto& term : intersectedRes)
        {
                sortedMatches.emplace_back(term);
        }

        std::sort(sortedMatches.begin(), sortedMatches.end(), [](const auto&a, const auto&b)
        {
                return a.second > b.second;
        });

        for (const auto& it : sortedMatches)
        {
                outSortedIds.push_back(it.first);
        }
}
