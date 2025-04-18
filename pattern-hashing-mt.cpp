#include <cstdint>
#include <iostream>
#include <cstring>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>
#include <thread>
#include <future>
#include "pattern.h"

int main() {
    const size_t max_length = 1000000;   // Maximum string length: 1 million characters
    const size_t num_iterations = 100000; // 100k iterations for averaging
    std::vector<size_t> lengths = {100, 1000, 10000, 100000, max_length};

    for (size_t len : lengths) {
        std::string test_str = generate_random_string(len);
        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 1;

        // --- Test 32-bit hash function ---
        size_t iters_per_thread = num_iterations / num_threads;
        size_t remainder = num_iterations % num_threads;
        std::vector<std::future<double>> futures_32;

        for (unsigned int t = 0; t < num_threads; ++t) {
            size_t local_iters = iters_per_thread + (t < remainder ? 1 : 0);
            futures_32.push_back(std::async(std::launch::async, [&, local_iters]() -> double {
                double local_total = 0.0;
                for (size_t i = 0; i < local_iters; ++i) {
                    auto start = std::chrono::high_resolution_clock::now();
                    volatile uint32_t local_hash = hash_function_32(test_str.c_str(), len);
                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double, std::micro> diff = end - start;
                    local_total += diff.count();
                }
                return local_total;
            }));
        }
        double total_time_32 = 0.0;
        for (auto &f : futures_32) {
            total_time_32 += f.get();
        }
        double average_time_32 = total_time_32 / num_iterations;
        uint32_t hash_32 = hash_function_32(test_str.c_str(), len);
        std::cout << "32-bit Hash - Length " << len << ": Average time = " 
                  << average_time_32 << " microseconds, Hash = " << hash_32 << std::endl;
        std::cout << "Hash (hex): 0x" 
                  << std::hex << std::setw(8) << std::setfill('0') << hash_32 << std::endl;

        // --- Test 64-bit hash function ---
        iters_per_thread = num_iterations / num_threads;
        remainder = num_iterations % num_threads;
        std::vector<std::future<double>> futures_64;

        for (unsigned int t = 0; t < num_threads; ++t) {
            size_t local_iters = iters_per_thread + (t < remainder ? 1 : 0);
            futures_64.push_back(std::async(std::launch::async, [&, local_iters]() -> double {
                double local_total = 0.0;
                for (size_t i = 0; i < local_iters; ++i) {
                    auto start = std::chrono::high_resolution_clock::now();
                    volatile uint64_t local_hash = hash_function_64(test_str.c_str(), len);
                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double, std::micro> diff = end - start;
                    local_total += diff.count();
                }
                return local_total;
            }));
        }
        double total_time_64 = 0.0;
        for (auto &f : futures_64) {
            total_time_64 += f.get();
        }
        double average_time_64 = total_time_64 / num_iterations;
        uint64_t hash_64 = hash_function_64(test_str.c_str(), len);
        std::cout << "64-bit Hash - Length " << len << ": Average time = " 
                  << average_time_64 << " microseconds, Hash = " << hash_64 << std::endl;
        std::cout << "Hash (hex): 0x" 
                  << std::hex << std::setw(16) << std::setfill('0') << hash_64 << std::endl;
    }

    return 0;
}
