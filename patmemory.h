#ifndef PATMEMORY_H
#define PATMEMORY_H

#include <vector>
#include <string>
#include <cstdint>
#include "pattern.h"

template<typename Value>
class Memory {
private:
    struct Entry {
        std::string key;
        Value value;
        bool used;
        Entry() : key(""), value(), used(false) {}
    };

    std::vector<Entry> table;  // Declared first...
    size_t capacity;           // ...then capacity

public:
    // Reorder initializer list to match declaration order
    Memory(size_t cap) : table(cap), capacity(cap) {}

    // Insert (returns true on success, false if full)
    bool put(const std::string &key, const Value &value) {
        uint64_t hash = hash_function_64(key.c_str(), key.size());
        size_t index = hash % capacity;
        size_t start = index;
        // Use linear probing to resolve collisions.
        do {
            if (!table[index].used || table[index].key == key) {
                table[index].key = key;
                table[index].value = value;
                table[index].used = true;
                return true;
            }
            index = (index + 1) % capacity;
        } while (index != start);
        return false; // Table is full.
    }

    // Retrieve value; returns true if found.
    bool get(const std::string &key, Value &value_out) {
        uint64_t hash = hash_function_64(key.c_str(), key.size());
        size_t index = hash % capacity;
        size_t start = index;
        while (table[index].used) {
            if (table[index].key == key) {
                value_out = table[index].value;
                return true;
            }
            index = (index + 1) % capacity;
            if (index == start)
                break;
        }
        return false;
    }
};

#endif // PATMEMORY_H
