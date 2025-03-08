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
#include "patmemory.h"

int main() {
    // --- Testing Memory Data Structure ---
    std::cout << "\nTesting Memory Data Structure:" << std::endl;

    // 1. Basic Functionality Test:
    std::cout << "\n1. Basic Functionality Test:" << std::endl;
    Memory<std::string> mem(1024);
    mem.put("apple", "red");
    mem.put("banana", "yellow");
    mem.put("grape", "purple");

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
    Memory<int> smallMem(3);
    std::cout << "Adding 4 items to capacity 3:" << std::endl;
    std::cout << "Add 1: " << (smallMem.put("one", 1) ? "success" : "failed") << std::endl;
    std::cout << "Add 2: " << (smallMem.put("two", 2) ? "success" : "failed") << std::endl;
    std::cout << "Add 3: " << (smallMem.put("three", 3) ? "success" : "failed") << std::endl;
    std::cout << "Add 4: " << (smallMem.put("four", 4) ? "success" : "failed") << std::endl;

    // 3. Performance Test with Large Dataset:
    std::cout << "\n3. Performance Test with Large Dataset:" << std::endl;
    const size_t TEST_SIZES[] = {1000000, 5000000, 20000000}; // 1M, 5M, 20M records
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);

    for (size_t testSize : TEST_SIZES) {
        std::cout << "\nTesting with " << testSize << " records:" << std::endl;
        Memory<int> largeMem(testSize);

        // Insert performance
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < testSize; i++) {
            std::string key = "key" + std::to_string(i);
            largeMem.put(key, dis(gen));
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Insert time: " << duration.count() << " ms" << std::endl;
        std::cout << "Insert rate: " << (testSize * 1000.0 / duration.count()) << " records/sec" << std::endl;

        // Lookup performance
        size_t lookupCount = 10000;
        start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < lookupCount; i++) {
            std::string key = "key" + std::to_string(dis(gen) % testSize);
            int val;
            largeMem.get(key, val);
        }
        end = std::chrono::high_resolution_clock::now();
        auto lookup_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Lookup time (" << lookupCount << " random lookups): " 
                  << lookup_duration.count() << " μs" << std::endl;
        std::cout << "Average lookup time: " << (lookup_duration.count() / lookupCount) 
                  << " μs/lookup" << std::endl;

        // Memory usage estimate (approximate)
        size_t approxMemUsage = (sizeof(std::string) + sizeof(int)) * testSize + 
                              sizeof(std::unordered_map<std::string, int>);
        std::cout << "Approximate memory usage: " << (approxMemUsage / 1024.0 / 1024.0) 
                  << " MB" << std::endl;
    }

    // 4. Edge Cases:
    std::cout << "\n4. Edge Cases:" << std::endl;
    Memory<std::string> edgeMem(1);
    std::cout << "Empty key test: " << (edgeMem.put("", "empty") ? "success" : "failed") << std::endl;
    
    if (edgeMem.get("", value))
        std::cout << "Retrieved empty key value: " << value << std::endl;

    return 0;
}
