#ifndef PATMEMORY_MT_H
#define PATMEMORY_MT_H

#include <vector>
#include <string>
#include <cstdint>
#include "pattern.h"
#include <shared_mutex>

template<typename Value>
class MemoryMT {
private:
    struct Entry {
        std::string key;
        Value value;
        bool used;
        Entry() : key(""), value(), used(false) {}
    };

    std::vector<Entry> table;  // declared first...
    size_t capacity;           // ...then capacity
    mutable std::shared_mutex mutex; // for multithreaded access

public:
    // Constructor: note the initializer order now matches the declaration order.
    MemoryMT(size_t cap) : table(cap), capacity(cap) {}

    // Insert (returns true on success, false if full)
    bool put(const std::string &key, const Value &value) {
        std::unique_lock lock(mutex); // acquire exclusive lock for writing
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
    bool get(const std::string &key, Value &value_out) const {
        std::shared_lock lock(mutex); // acquire shared lock for reading
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

#endif // PATMEMORY_MT_H
