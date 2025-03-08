#include <cstdint>
#include <iostream>
#include <cstring>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>   // For std::hex, std::setw, and std::setfill

#include "pattern.h"

int main() {
    const size_t max_length = 1000000; // Maximum hash length: 1 million characters
    const size_t num_iterations = 100000; // 100k loops for time-averaging
    std::vector<size_t> lengths = {100, 1000, 10000, 100000, max_length};

    for (size_t len : lengths) {
        std::string test_str = generate_random_string(len);

        // Test 32-bit hash
        double total_time_32 = 0.0;
        uint32_t hash_32 = 0;
        for (size_t i = 0; i < num_iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            hash_32 = hash_function_32(test_str.c_str(), len);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> duration = end - start;
            total_time_32 += duration.count();
        }
        double average_time_32 = total_time_32 / num_iterations;
        std::cout << "32-bit Hash - Length " << len << ": Average time = " 
                  << average_time_32 << " microseconds, Hash = " << hash_32 << std::endl;
        std::cout << "Hash (hex): 0x"       // Prefix for clarity
              << std::hex               // Use hexadecimal format
              << std::setw(8)           // Set width to 8 characters
              << std::setfill('0')      // Pad with zeros if needed
              << hash_32 << std::endl;     // Print the hash

        // Test 64-bit hash
        double total_time_64 = 0.0;
        uint64_t hash_64 = 0;
        for (size_t i = 0; i < num_iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            hash_64 = hash_function_64(test_str.c_str(), len);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> duration = end - start;
            total_time_64 += duration.count();
        }
        double average_time_64 = total_time_64 / num_iterations;
        std::cout << "64-bit Hash - Length " << len << ": Average time = " 
                  << average_time_64 << " microseconds, Hash = " << hash_64 << std::endl;
        std::cout << "Hash (hex): 0x"       // Prefix for clarity
              << std::hex               // Use hexadecimal format
              << std::setw(8)           // Set width to 8 characters
              << std::setfill('0')      // Pad with zeros if needed
              << hash_64 << std::endl;     // Print the hash
    }

    return 0;
}