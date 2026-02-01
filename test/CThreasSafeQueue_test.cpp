//
// Created by haimash on 29/01/2026.
//

#include <thread>
#include <vector>
#include <numeric>
#include <atomic>
#include <gtest/gtest.h>
#include "CThreadSafeQueue.h"
#include <string>

TEST(CThreadSafeQueueTest, BasicPushPop) {
    CThreadSafeQueue<std::string> queue;
    queue.Push("TestItem");

    std::string result;
    bool success = queue.Pop(result);

    EXPECT_TRUE(success);
    EXPECT_EQ(result, "TestItem");
}

TEST(CThreadSafeQueueTest, SignalFinishedReturnsFalseWhenEmpty) {
    CThreadSafeQueue<int> queue;
    queue.SignalFinished();

    int result;
    // Pop should return false immediately because it's finished and empty
    EXPECT_FALSE(queue.Pop(result));
}

TEST(CThreadSafeQueueTest, MultipleProducersAndConsumers) {
    CThreadSafeQueue<int> queue;
    const int numProducers = 4;
    const int numConsumers = 4;
    const int itemsPerProducer = 1000;

    std::atomic<int> totalPoppedSum{0};
    int expectedSum = 0;

    // Calculate expected sum for verification
    for(int i = 0; i < numProducers * itemsPerProducer; ++i) {
        expectedSum += i;
    }

    // 1. Launch Consumers
    std::vector<std::jthread> consumers;
    for (int i = 0; i < numConsumers; ++i) {
        consumers.emplace_back([&queue, &totalPoppedSum]() {
            int val;
            while (queue.Pop(val)) {
                totalPoppedSum += val;
            }
        });
    }

    // 2. Launch Producers
    std::vector<std::jthread> producers;
    for (int i = 0; i < numProducers; ++i) {
        producers.emplace_back([&queue, i, itemsPerProducer]() {
            for (int j = 0; j < itemsPerProducer; ++j) {
                // Push unique values to verify sum later
                queue.Push(i * itemsPerProducer + j);
            }
        });
    }

    // 3. Wait for producers to finish
    for (auto& p : producers) p.join();

    // 4. Signal that no more items are coming
    queue.SignalFinished();

    // 5. Wait for consumers to drain the queue and exit
    for (auto& c : consumers) c.join();

    EXPECT_EQ(totalPoppedSum.load(), expectedSum);
}

TEST(CThreadSafeQueueTest, ConsumerBlocksUntilPush) {
    CThreadSafeQueue<int> queue;
    int result = 0;
    std::atomic<bool> popped{false};

    std::jthread consumer([&]() {
        if (queue.Pop(result)) {
            popped = true;
        }
    });

    // Give the consumer a moment to start and block
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_FALSE(popped.load()); // Should still be waiting

    queue.Push(42);

    // Wait for consumer to finish
    consumer.join();

    EXPECT_TRUE(popped.load());
    EXPECT_EQ(result, 42);
}