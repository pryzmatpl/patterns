#include <cstdint>
#include <iostream>
#include <cstring>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>
#include <thread>
#include <future>

// 32-bit hash function using inline assembly
uint32_t hash_function_32(const char* str, size_t len) {
    uint32_t hash;
    asm volatile (
        "xorq %%rax, %%rax\n"             // Clear 64-bit accumulator (rax)
        "xorq %%rdx, %%rdx\n"             // Clear 64-bit index counter (rdx)
        "mov %[str], %%rsi\n"             // Load string pointer into rsi
        "mov %[len], %%rdi\n"             // Load length into rdi
        ".loop_32:\n"
        "cmpq %%rdi, %%rdx\n"             // Compare index (rdx) with length (rdi)
        "jge .done_32\n"
        "movzbl (%%rsi, %%rdx,1), %%ecx\n" // Load byte from string into ecx (zero-extended)
        // Inline length value logic
        "cmpb $'1', %%cl\n"
        "je .one_32\n"
        "cmpb $'&', %%cl\n"
        "je .and_32\n"
        "cmpb $'*', %%cl\n"
        "je .star_32\n"
        "movl $1, %%ecx\n"                // Default value: 1
        "jmp .after_len_32\n"
        ".one_32:\n"
        "movl $1, %%ecx\n"                // '1' -> 1
        "jmp .after_len_32\n"
        ".and_32:\n"
        "movl $2, %%ecx\n"                // '&' -> 2
        "jmp .after_len_32\n"
        ".star_32:\n"
        "movl $0xFFFFFFFF, %%ecx\n"       // '*' -> max 32-bit value
        ".after_len_32:\n"
        "movq %%rdx, %%rbx\n"             // Copy index (rdx) to rbx
        "incq %%rbx\n"                    // rbx = index + 1 (position)
        "imulq %%rcx, %%rbx\n"            // Multiply length value by position
        "addq %%rbx, %%rax\n"             // Add to accumulator (rax)
        "incq %%rdx\n"                    // Increment index
        "jmp .loop_32\n"
        ".done_32:\n"
        "movl %%eax, %[hash]\n"           // Store lower 32 bits of rax into hash
        : [hash] "=r" (hash)
        : [str] "r" (str), [len] "r" (len)
        : "rax", "rbx", "rcx", "rdx", "rsi", "rdi"
    );
    return hash;
}

// 64-bit hash function using inline assembly
uint64_t hash_function_64(const char* str, size_t len) {
    uint64_t hash;
    asm volatile (
        "xorq %%rax, %%rax\n"             // Clear 64-bit accumulator (rax)
        "xorq %%rdx, %%rdx\n"             // Clear 64-bit index counter (rdx)
        "mov %[str], %%rsi\n"             // Load string pointer into rsi
        "mov %[len], %%rdi\n"             // Load length into rdi
        ".loop_64:\n"
        "cmpq %%rdi, %%rdx\n"             // Compare index (rdx) with length (rdi)
        "jge .done_64\n"
        "movzbl (%%rsi, %%rdx,1), %%ecx\n" // Load byte from string into ecx (zero-extended)
        // Inline length value logic
        "cmpb $'1', %%cl\n"
        "je .one_64\n"
        "cmpb $'&', %%cl\n"
        "je .and_64\n"
        "cmpb $'*', %%cl\n"
        "je .star_64\n"
        "movq $1, %%rcx\n"                // Default value: 1 (64-bit)
        "jmp .after_len_64\n"
        ".one_64:\n"
        "movq $1, %%rcx\n"                // '1' -> 1
        "jmp .after_len_64\n"
        ".and_64:\n"
        "movq $2, %%rcx\n"                // '&' -> 2
        "jmp .after_len_64\n"
        ".star_64:\n"
        "movq $0xFFFFFFFFFFFFFFFF, %%rcx\n" // '*' -> max 64-bit value
        ".after_len_64:\n"
        "movq %%rdx, %%rbx\n"             // Copy index (rdx) to rbx
        "incq %%rbx\n"                    // rbx = index + 1 (position)
        "imulq %%rcx, %%rbx\n"            // Multiply length value by position
        "addq %%rbx, %%rax\n"             // Add to accumulator (rax)
        "incq %%rdx\n"                    // Increment index
        "jmp .loop_64\n"
        ".done_64:\n"
        "movq %%rax, %[hash]\n"           // Store full 64-bit rax into hash
        : [hash] "=r" (hash)
        : [str] "r" (str), [len] "r" (len)
        : "rax", "rbx", "rcx", "rdx", "rsi", "rdi"
    );
    return hash;
}

std::string generate_random_string(size_t length) {
    const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()";
    std::string result;
    result.reserve(length);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, charset.size() - 1);
    for (size_t i = 0; i < length; ++i) {
        result += charset[dis(gen)];
    }
    return result;
}

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
