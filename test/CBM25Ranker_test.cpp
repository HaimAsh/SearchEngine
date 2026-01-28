//
// Created by haimash on 28/01/2026.
//

#include <gtest/gtest.h>

#include "CInvertedIndex.h"
#include "CBM25Ranker.h"

static std::vector<TermPostings>
BuildQueryPostings(const CInvertedIndex& index,
                   const std::vector<std::string>& queryTerms)
{
    std::vector<TermPostings> result;

    for (const auto& term : queryTerms)
    {
        auto it = index.GetIterator(term);
        if (it == index.End())
            continue;

        TermPostings tp;
        tp.term = term;
        tp.postings = it->second;
        result.push_back(std::move(tp));
    }

    return result;
}

TEST(BM25Ranker, HigherTermFrequencyRanksHigher)
{
    CInvertedIndex index;

    // doc 0: "apple"
    index.AddTitle(0, "doc0");
    index.AddToken("apple", 0);

    // doc 1: "apple apple apple"
    index.AddTitle(1, "doc1");
    index.AddToken("apple", 1);
    index.AddToken("apple", 1);
    index.AddToken("apple", 1);

    CBM25Ranker ranker;

    auto queryPosting = BuildQueryPostings(index, {"apple"});
    std::vector<uint32_t> result;

    ranker.Rank(queryPosting,
                index.GetDocWordCounts(),
                index.GetDocsTitles(),
                result);

    ASSERT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], 1u); // doc 1 should win
}

TEST(BM25Ranker, RareTermOutweighsCommonTerm)
{
    CInvertedIndex index;

    // doc 0: common + rare
    index.AddTitle(0, "doc0");
    index.AddToken("common", 0);
    index.AddToken("rare", 0);

    // doc 1: common common common
    index.AddTitle(1, "doc1");
    index.AddToken("common", 1);
    index.AddToken("common", 1);
    index.AddToken("common", 1);

    CBM25Ranker ranker;

    auto queryPosting = BuildQueryPostings(index, {"rare", "common"});
    std::vector<uint32_t> result;

    ranker.Rank(queryPosting,
                index.GetDocWordCounts(),
                index.GetDocsTitles(),
                result);

    ASSERT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], 0u); // rare term dominates
}

TEST(BM25Ranker, DuplicateQueryTermsDoNotDoubleCount)
{
    CInvertedIndex index;

    index.AddTitle(0, "doc0");
    index.AddToken("engine", 0);

    index.AddTitle(1, "doc1");
    index.AddToken("engine", 1);
    index.AddToken("engine", 1);

    CBM25Ranker ranker;

    auto single = BuildQueryPostings(index, {"engine"});
    auto duplicate = BuildQueryPostings(index, {"engine", "engine"});

    std::vector<uint32_t> r1;
    std::vector<uint32_t> r2;

    ranker.Rank(single,
                index.GetDocWordCounts(),
                index.GetDocsTitles(),
                r1);

    ranker.Rank(duplicate,
                index.GetDocWordCounts(),
                index.GetDocsTitles(),
                r2);

    ASSERT_EQ(r1.size(), r2.size());
    EXPECT_EQ(r1[0], r2[0]);
}

TEST(BM25Ranker, TitleMatchBoostsRanking)
{
    CInvertedIndex index;

    index.AddTitle(0, "apple pie");
    index.AddToken("apple", 0);
    index.AddToken("pie", 0);

    index.AddTitle(1, "banana");
    index.AddToken("pie", 1);
    index.AddToken("pie", 1);
    index.AddToken("pie", 1);

    CBM25Ranker ranker;

    auto queryPosting = BuildQueryPostings(index, {"apple"});
    std::vector<uint32_t> result;

    ranker.Rank(queryPosting,
                index.GetDocWordCounts(),
                index.GetDocsTitles(),
                result);

    ASSERT_FALSE(result.empty());
    EXPECT_EQ(result[0], 0u); // title match wins
}