# Wikipedia Search Engine (C++20)

A high-performance search engine built from scratch to index and rank Wikipedia XML dumps. This project implements advanced Information Retrieval (IR) concepts including **parallel indexing**, **binary persistence**, and **BM25 ranking**.

## Key Technical Features

* **Multi-Threaded Indexing:** Processes large XML dumps using a **Producer-Consumer model** with `std::jthread` and a custom thread-safe queue. XML parsing is single-threaded, while document analysis (tokenization and term-frequency computation) is parallelized.
* **Instant Binary Persistence:** Custom binary serialization layer (`Save`/`Load`) allows the engine to bypass XML parsing on subsequent launches, reaching a "Search Ready" state in **0 seconds**.
* **Parallel Document Analysis:** Worker threads tokenize documents and compute term frequencies in parallel, inserting results into a synchronized global inverted index.
* **Memory Efficiency:** Utilizes **absl::flat_hash_map** for **O(1)** lookups. `std::string_view` is used transiently during parsing and tokenization, while the inverted index stores owning strings to ensure safe lifetimes.
* **Unit Testing:** Comprehensive test suite using **GoogleTest (GTest)** to verify tokenizer accuracy, index integrity, and ranking mathematical correctness.
* **Advanced Ranking:** Implements multiple ranking strategies via the **Factory Pattern**:
    * **BM25 (Best Matching 25):** State-of-the-art probabilistic relevance model.
    * **TF-IDF:** Traditional statistical importance ranking.
    * **Frequency:** Baseline raw count ranking.
* **Title-Field Boosting:** A custom heuristic that identifies query matches within document titles to prioritize primary entity articles.

---

## Project Architecture

The engine is divided into three primary layers, now optimized for high-concurrency:

1. **Ingestion Layer:** A main thread reads XML data while multiple worker threads tokenize content and build thread-local indices to ensure maximum CPU utilization.
2. **Storage Layer:** A thread-safe **CInvertedIndex** protected by `std::shared_mutex` for high-concurrency writes and optimized binary I/O for persistent storage.
3. **Ranking Layer:** An extensible **IRanker** interface that processes query results and sorts them by relevance based on the selected algorithm.

---

## Performance Benchmarks (Large Wiki Dataset)

| Metric | XML Cold Build | Binary Warm Load |
| :--- | :--- | :--- |
| **Total Time** | **15 - 16 seconds** | **~0 seconds** |
| **RAM Usage** | **~1.4 GB** | **~516 MB** |
| **Words Stored** | 1,579,339 | 1,579,339 |
| **Persistence Size** | N/A | **370 MB (.bin)** |

---

## Reliability & Concurrency Testing

To ensure stability in a multi-threaded environment, the engine was validated using:
* **ThreadSanitizer (TSan):** Used to detect and eliminate data races and heap-use-after-free errors during parallel execution.
* **GoogleTest (GTest):** Used for stress testing the `CThreadSafeQueue` and ensuring indexing logic remains consistent across threads.

---

## Lessons Learned: The "Israel" Problem

A significant challenge during development was the "Ranking Paradox." Initially, the query "israel" returned short biographical stubs or election tables at #1 because they had high keyword density.

**The Solution:**
I implemented **Multi-Field Scoring**. By passing document titles to the ranking layer and applying a **Whole-Word Title Boost**, the engine was able to differentiate between an article *about* Israel and an article that merely *mentions* it. This successfully moved the "State of Israel" article to the #1 spot.

---

## Data Source
This engine is designed to parse **Wikimedia XML Dumps**. 
1. Download a dump (e.g., `enwiki-latest-pages-articles.xml`) from the [Official Wikimedia Dumps](https://dumps.wikimedia.org/).
2. For testing, it is recommended to use a smaller subset or a "multistream" file.

## Build & Run

### Prerequisites
* C++20 compatible compiler (GCC 11+ / Clang 13+)
* CMake 3.15+
* **Abseil-cpp** library - added as submodule
* **GoogleTest** library - added as submodule
* **Libxml2**

### Steps
```bash
# Clone the repository and submodules
git clone --recursive [https://github.com/HaimAsh/SearchEngine.git](https://github.com/HaimAsh/SearchEngine.git)
cd SearchEngine

# Build
mkdir build && cd build
cmake ..
make

# Run
./SearchEngine /path/to/your/wiki_dump.xml
