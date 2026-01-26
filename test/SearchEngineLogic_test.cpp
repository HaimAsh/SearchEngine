//
// Created by haimash on 26/01/2026.
//
#include <gtest/gtest.h>
#include "CTokenizer.h"
#include "CInvertedIndex.h"

TEST(CTokenizerTest, HandlesGarbageAndNormalization) {
    // Input contains: stop-word ("The"), garbage number ("12345"),
    // too short ("a"), and valid tokens ("Science", "Space").
    std::string text = "The 12345 Science Space a";
    CTokenizer tokenizer(text);

    // "The" is a stop word -> Skip
    // "12345" starts with digit -> Skip
    // "Science" -> Expect "science"
    EXPECT_EQ(tokenizer.Next(), "science");

    // "Space" -> Expect "space"
    EXPECT_EQ(tokenizer.Next(), "space");

    // "a" is too short -> Skip
    EXPECT_EQ(tokenizer.Next(), "");
}

// 1. Inherit from ::testing::Test
class InvertedIndexTest : public ::testing::Test {
protected:
    CInvertedIndex index;

    // This runs before every TEST_F
    void SetUp() override {
        index.AddToken("apple", 1);
        index.AddToken("banana", 1);
        index.AddToken("apple", 2);
        index.AddToken("cherry", 2);
    }
};

// 2. Use TEST_F (Fixture) instead of TEST
TEST_F(InvertedIndexTest, BasicIntersection) {
    auto results = index.Search("apple");
    EXPECT_EQ(results.size(), 2);
}

TEST_F(InvertedIndexTest, NoMatchReturnEmpty) {
    auto results = index.Search("xylophone");
    EXPECT_TRUE(results.empty());
}

TEST_F(InvertedIndexTest, HandlesEmptyOrJunkQueries) {
    // 1. Completely empty string
    auto res1 = index.Search("");
    EXPECT_TRUE(res1.empty());

    // 2. String with only a stop-word (assuming "the" is filtered)
    auto res2 = index.Search("the");
    EXPECT_TRUE(res2.empty());

    // 3. String with only garbage (starts with number)
    auto res3 = index.Search("12345");
    EXPECT_TRUE(res3.empty());
}

TEST_F(InvertedIndexTest, WordNotFoundInIndex) {
    // Search for a validly formatted word that simply isn't in our dictionary
    auto res = index.Search("thorium");
    EXPECT_TRUE(res.empty());
}

TEST_F(InvertedIndexTest, HandlesUnbalancedLists) {
    // Add 'common' to many docs
    for(uint32_t i = 1; i <= 100; ++i) {
        index.AddToken("common", i);
    }
    // Add 'rare' to only the last doc
    index.AddToken("rare", 100);

    auto res = index.Search("common rare");
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0], 100);
}

TEST_F(InvertedIndexTest, SearchIsCaseInsensitive) {
    index.AddToken("physics", 50);

    auto resUpper = index.Search("PHYSICS");
    auto resLower = index.Search("physics");

    ASSERT_EQ(resUpper.size(), 1);
    EXPECT_EQ(resUpper[0], 50);
    EXPECT_EQ(resUpper, resLower);
}

TEST_F(InvertedIndexTest, UniqueWordCountAccuracy) {
    size_t initialCount = index.GetUniqueWordCount();

    // Add a new word
    index.AddToken("brandnewword", 999);
    EXPECT_EQ(index.GetUniqueWordCount(), initialCount + 1);

    // Add an existing word to a new doc (count should NOT increase)
    index.AddToken("apple", 999);
    EXPECT_EQ(index.GetUniqueWordCount(), initialCount + 1);
}

TEST_F(InvertedIndexTest, PreventsDuplicatePostings) {
    // Add same token to same ID twice
    index.AddToken("science", 10);
    index.AddToken("science", 10);

    const auto& postings = index.GetIDsOfToken("science");

    // Should only have '10' once
    EXPECT_EQ(postings.size(), 1);
}