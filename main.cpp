//
// Created by haimash on 21/01/2026.
//

#include <iostream>
#include <ostream>
#include <chrono>
#include <fstream>

#include "CSearchEngine.h"
#include "CFrequencyRanker.h"
#include "CTF_IDFRanker.h"
#include "CBM25Ranker.h"
#include "CRankerFactory.h"

size_t GetMemoryUsage() {
    std::ifstream file("/proc/self/status");
    std::string line;
    while (std::getline(file, line)) {
        if (line.compare(0, 6, "VmRSS:") == 0) {
            return std::stoul(line.substr(7)); // Returns KB
        }
    }
    return 0;
}

std::unique_ptr<IRanker> CreateRanker(int choice) {
    switch (choice) {
        case 1:
            return std::make_unique<CBM25Ranker>();
        case 2:
            return std::make_unique<CTF_IDFRanker>();
        case 3:
            return std::make_unique<CFrequencyRanker>();
        default:
            std::cout << "Invalid choice, defaulting to BM25\n";
            return std::make_unique<CBM25Ranker>();
    }
}

int main()
{
    CSearchEngine searchEngine;

    const std::string filePath = "../data/simplewiki-latest-pages-articles.xml";

    std::cout << "Starting to build index..." << std::endl;

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    if (false == searchEngine.Init(filePath))
    {
        std::cerr << "Failed to initialize" << std::endl;
        return EXIT_FAILURE;
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    size_t ramUsed = GetMemoryUsage();

    std::cout << "-----------------------------------" << std::endl;
    std::cout << "Total Time:   " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " s" << std::endl;
    std::cout << "RAM Usage:    " << ramUsed / 1024 << " MB" << std::endl;

    std::string query;
    std::cout << "\nIndex Built! " << searchEngine.GetWordCount() << " words stored." << std::endl;

    std::cout << "Choose Ranker:\n1. Frequency\n2. TF-IDF\n3. BM25 (Title Boosted)\nSelection: ";
    int choice;
    std::cin >> choice;

    RankerType type = RankerType::BM25; // Default
    if (choice == 1)
    {
        type = RankerType::FREQUENCY;
    }
    else if (choice == 2)
    {
        type = RankerType::TF_IDF;
    }

    searchEngine.SetRanker(RankerFactory::CreateRanker(type));

    while (true) {
        std::cout << "\nEnter search query (or 'exit'): ";
        std::getline(std::cin, query);
        if (query == "exit") break;

        start = std::chrono::high_resolution_clock::now();
        std::vector<std::string> hits;
        searchEngine.Search(query, hits);
        end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duration = end - start;

        std::cout << "Found " << hits.size() << " results in " << duration.count() << "ms" << std::endl;

        // Print the first 10 results
        for (size_t i = 0; i < std::min<size_t>(10, hits.size()); ++i) {
            std::cout << (i + 1) << ". " << hits[i] << std::endl;
        }
    }

    return 0;
}