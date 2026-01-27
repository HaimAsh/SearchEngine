//
// Created by haimash on 27/01/2026.
//

#ifndef SEARCHENGINE_CRANKERFACTORY_H
#define SEARCHENGINE_CRANKERFACTORY_H

#include "IRanker.h"
#include "CFrequencyRanker.h"
#include "CTF_IDFRanker.h"
#include "CBM25Ranker.h"
#include <memory>

enum class RankerType {
    FREQUENCY,
    TF_IDF,
    BM25
};

class RankerFactory {
public:
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