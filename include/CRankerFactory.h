//
// Created by haimash on 27/01/2026.
//
/// @file CRankerFactory.h
/// implements a factory design for creating ranking algorithm
#ifndef SEARCHENGINE_CRANKERFACTORY_H
#define SEARCHENGINE_CRANKERFACTORY_H

#include "IRanker.h"
#include "CFrequencyRanker.h"
#include "CTF_IDFRanker.h"
#include "CBM25Ranker.h"
#include <memory>

/// enum to define the types of algorithms
/// to add place the new algorithm at the end of the enum
enum class RankerType {
    FREQUENCY = 0,
    TF_IDF = 1,
    BM25 = 2
};

/// @class CRankerFactory
/// @brief this class is responsible to create the algorithm
class CRankerFactory {
public:
    /// this function creates a unique_ptr of the algorithm
    /// @param type the type of the algorithm to create
    /// @return returns a unique ptr for the algorithm class, if type is invalid, BM25 will be the default
    static std::unique_ptr<IRanker> CreateRanker(RankerType type) {
        switch (type) {
            case RankerType::FREQUENCY:
                return std::make_unique<CFrequencyRanker>();
            case RankerType::TF_IDF:
                return std::make_unique<CTF_IDFRanker>();
            case RankerType::BM25:
                return std::make_unique<CBM25Ranker>();
            default:
                return std::make_unique<CBM25Ranker>();
        }
    }
};

#endif //SEARCHENGINE_CRANKERFACTORY_H