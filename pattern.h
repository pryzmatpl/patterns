#ifndef _PATTERN_
#define _PATTERN_

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

#endif