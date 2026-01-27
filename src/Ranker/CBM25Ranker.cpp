//
// Created by haimash on 26/01/2026.
//

#include "CBM25Ranker.h"

#include <cmath>
#include <algorithm>

#include "absl/container/flat_hash_map.h"

size_t CBM25Ranker::CaseInsensitiveFind(std::string_view haystack, std::string_view needle, size_t pos) const
{
    if (needle.empty()) return 0;
    if (pos >= haystack.size()) return std::string::npos;

    auto it = std::search(
        haystack.begin() + pos, haystack.end(),
        needle.begin(), needle.end(),
        [](char h, char n) {
            return std::tolower(static_cast<unsigned char>(h)) ==
                   std::tolower(static_cast<unsigned char>(n));
        }
    );

    if (it != haystack.end()) {
        return std::distance(haystack.begin(), it);
    }
    return std::string::npos;
}

bool CBM25Ranker::IsWholeWordMatch(std::string_view title, std::string_view term) const
{
    size_t pos = 0;
    while ((pos = CaseInsensitiveFind(title, term, pos)) != std::string::npos)
        {
        // Check if character before is non-alphanumeric
        bool startMatch = (pos == 0 || !std::isalnum(title[pos - 1]));
        // Check if character after is non-alphanumeric
        bool endMatch = (pos + term.length() == title.length() || !std::isalnum(title[pos + term.length()]));

        if (startMatch && endMatch)
        {
            return true;
        }

        pos += 1;
    }

    return false;
}

void CBM25Ranker::Rank(const std::vector<TermPostings>& queryPosting,
                         const std::vector<uint32_t>& docLengths,
                         const std::vector<std::string>& docTitles,
                         std::vector<uint32_t>& outSortedIds) const
{
    const double k1 = 1.2;
    const double b = 0.75;

    outSortedIds.clear();

    if (queryPosting.empty() || docLengths.empty())
    {
        return;
    }

    double sumLengths = 0;
    for(auto len : docLengths)
    {
        sumLengths += len;
    }
    double avgDocLength = sumLengths / docLengths.size();

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

        for (const auto& [docId, freq] : term.postings) {
            double tf = static_cast<double>(freq);
            double docLen = static_cast<double>(docLengths[docId]);

            // Check if the term is in the title
            const std::string& title = docTitles[docId];
            if (IsWholeWordMatch(title, term.term))
            {
                tf += 100.0; // Boost it even more for an exact word match!
            }

            // This is the BM25 'Soft' Saturation formula
            double numerator = tf * (k1 + 1);
            double denominator = tf + k1 * (1 - b + b * (docLen / avgDocLength));

            docScores[docId] += idf * (numerator / denominator);
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
