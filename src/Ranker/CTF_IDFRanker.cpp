//
// Created by haimash on 26/01/2026.
//

#include "CTF_IDFRanker.h"

#include <cmath>
#include <algorithm>

#include "absl/container/flat_hash_map.h"

void CTF_IDFRanker::Rank(const std::vector<TermPostings>& queryPosting,
                         const std::vector<uint32_t>& docLengths,
                         const std::vector<std::string>& docTitles,
                         std::vector<uint32_t>& outSortedIds) const
{
    outSortedIds.clear();

    if (queryPosting.empty() || docLengths.empty())
    {
        return;
    }

    absl::flat_hash_map<uint32_t, double> docScores;
    size_t totalDocs = docLengths.size();

    for (const auto& term : queryPosting)
    {
        if (term.postings.empty())
        {
            continue;
        }

        // 1. IDF: How rare is this word in the entire library?
        // If a word is in every document, IDF will be near 0.
        // If it's in only 10 docs, IDF will be very high.
        double df = static_cast<double>(term.postings.size());
        double idf = std::log10(static_cast<double>(totalDocs) / (1.0 + df));

        for (const auto& [docId, freq] : term.postings)
        {
            // 2. TF: Logarithmic scaling for frequency
            // This prevents a document that says 'israel' 1000 times
            // from being 100x better than one that says it 10 times.
            double tf = 1.0 + std::log10(static_cast<double>(freq));

            // 3. Accumulate: TF * IDF
            docScores[docId] += (tf * idf);
        }
    }

    std::vector<std::pair<uint32_t, double>> scoreVec(docScores.begin(), docScores.end());
    outSortedIds.reserve(scoreVec.size());

    std::sort(scoreVec.begin(), scoreVec.end(), [](const auto& a, const auto& b)
    {
        return a.second > b.second;
    });

    for (const auto& match: scoreVec)
    {
        outSortedIds.emplace_back(match.first);
    }
}
