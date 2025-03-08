#include <cstdint>
#include <iostream>
#include <cstring>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>
#include <thread>
#include <future>
#include <string>
#include <memory>
#include <unordered_map>
#include "pattern.h"
#include "patmemory-mt.h"

int main() {
    std::cout << "\nTesting Memory Data Structure (Multithreaded):" << std::endl;
    
    // 1. Basic Functionality Test:
    std::cout << "\n1. Basic Functionality Test:" << std::endl;
    MemoryMT<std::string> mem(1024);
    // Launch threads to concurrently insert items.
    std::vector<std::thread> threads;
    threads.emplace_back([&mem](){ mem.put("apple", "red"); });
    threads.emplace_back([&mem](){ mem.put("banana", "yellow"); });
    threads.emplace_back([&mem](){ mem.put("grape", "purple"); });
    for (auto &t : threads)
        t.join();

    std::string value;
    if (mem.get("apple", value))
        std::cout << "apple: " << value << std::endl;
    if (mem.get("banana", value))
        std::cout << "banana: " << value << std::endl;
    if (mem.get("grape", value))
        std::cout << "grape: " << value << std::endl;
    if (!mem.get("orange", value))
        std::cout << "orange not found" << std::endl;
    
    // 2. Capacity Limit Test:
    std::cout << "\n2. Capacity Limit Test:" << std::endl;
    MemoryMT<int> smallMem(3);
    std::cout << "Adding 4 items to capacity 3 concurrently:" << std::endl;
    auto put_wrapper = [&smallMem](const std::string &key, int val) -> bool {
        return smallMem.put(key, val);
    };
    std::future<bool> f1 = std::async(std::launch::async, put_wrapper, "one", 1);
    std::future<bool> f2 = std::async(std::launch::async, put_wrapper, "two", 2);
    std::future<bool> f3 = std::async(std::launch::async, put_wrapper, "three", 3);
    std::future<bool> f4 = std::async(std::launch::async, put_wrapper, "four", 4);
    bool r1 = f1.get(), r2 = f2.get(), r3 = f3.get(), r4 = f4.get();
    std::cout << "Add 1: " << (r1 ? "success" : "failed") << std::endl;
    std::cout << "Add 2: " << (r2 ? "success" : "failed") << std::endl;
    std::cout << "Add 3: " << (r3 ? "success" : "failed") << std::endl;
    std::cout << "Add 4: " << (r4 ? "success" : "failed") << std::endl;

    // 3. Performance Test with Large Dataset:
    std::cout << "\n3. Performance Test with Large Dataset:" << std::endl;
    const size_t TEST_SIZE = 1000000; // 1M records for demonstration
    MemoryMT<int> largeMem(TEST_SIZE);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);
    
    // Concurrent insertion using multiple threads.
    size_t threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 4;
    std::vector<std::thread> insertThreads;
    size_t itemsPerThread = TEST_SIZE / threadCount;
    auto start_time = std::chrono::high_resolution_clock::now();
    for (size_t t = 0; t < threadCount; ++t) {
        size_t start = t * itemsPerThread;
        size_t end = (t == threadCount - 1) ? TEST_SIZE : start + itemsPerThread;
        insertThreads.emplace_back([&largeMem, start, end, &dis, &gen](){
            for (size_t i = start; i < end; ++i) {
                std::string key = "key" + std::to_string(i);
                largeMem.put(key, dis(gen));
            }
        });
    }
    for (auto &t : insertThreads)
        t.join();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Concurrent insert time: " << insert_duration.count() << " ms" << std::endl;
    std::cout << "Insert rate: " << (TEST_SIZE * 1000.0 / insert_duration.count()) << " records/sec" << std::endl;

    // Concurrent lookup performance test:
    size_t lookupCount = 10000;
    start_time = std::chrono::high_resolution_clock::now();
    std::vector<std::future<int>> lookupFutures;
    for (size_t i = 0; i < lookupCount; ++i) {
        lookupFutures.push_back(std::async(std::launch::async, [&largeMem, &dis, &gen, TEST_SIZE](){
            std::string key = "key" + std::to_string(dis(gen) % TEST_SIZE);
            int value;
            largeMem.get(key, value);
            return value;
        }));
    }
    for (auto &f : lookupFutures)
        f.get();
    end_time = std::chrono::high_resolution_clock::now();
    auto lookup_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Concurrent lookup time (" << lookupCount << " random lookups): " 
              << lookup_duration.count() << " μs" << std::endl;
    std::cout << "Average lookup time: " << (lookup_duration.count() / lookupCount) 
              << " μs/lookup" << std::endl;
    
    // 4. Edge Cases:
    std::cout << "\n4. Edge Cases:" << std::endl;
    MemoryMT<std::string> edgeMem(1);
    std::cout << "Empty key test: " << (edgeMem.put("", "empty") ? "success" : "failed") << std::endl;
    if (edgeMem.get("", value))
        std::cout << "Retrieved empty key value: " << value << std::endl;
    
    return 0;
}
