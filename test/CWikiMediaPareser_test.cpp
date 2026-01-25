#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include "CWikiMediaParser.h"

class WikiMediaParserTest : public ::testing::Test {
protected:
    // Helper to create a temporary small XML file
    void CreateTestXml(const std::string& filename, const std::string& content) {
        std::ofstream out(filename);
        out << content;
        out.close();
    }
};

TEST_F(WikiMediaParserTest, ParsesValidArticleCorrectly) {
    std::string xmlContent = 
        "<mediawiki><page>"
        "<title>Test Page</title>"
        "<ns>0</ns>"
        "<id>100</id>"
        "<revision><id>200</id><text>Hello World</text></revision>"
        "</page></mediawiki>";
    
    CreateTestXml("test_simple.xml", xmlContent);

    bool called = false;
    CWikiMediaParser parser([&](CDocument doc) {
        called = true;
        EXPECT_EQ(doc.title, "Test Page");
        EXPECT_EQ(doc.id, 100);
        EXPECT_EQ(doc.text, "Hello World");
    });

    bool success = parser.ParseFile("test_simple.xml");
    
    EXPECT_TRUE(success);
    EXPECT_TRUE(called);
}

TEST_F(WikiMediaParserTest, FiltersOutNonMainNamespace) {
    std::string xmlContent = 
        "<mediawiki><page>"
        "<title>Talk:Test</title>"
        "<ns>1</ns>" // Not Main Namespace
        "<id>101</id>"
        "<revision><text>Should be ignored</text></revision>"
        "</page></mediawiki>";
    
    CreateTestXml("test_ns.xml", xmlContent);

    bool called = false;
    CWikiMediaParser parser([&](CDocument doc) {
        called = true;
    });

    parser.ParseFile("test_ns.xml");
    
    EXPECT_FALSE(called); // Callback should never be fired
}