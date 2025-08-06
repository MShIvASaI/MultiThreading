#pragma once
#include <unordered_map>
#include <list>
#include <mutex>
#include <shared_mutex>

// LRUCache class implements a Least Recently Used (LRU) cache with thread-safety using modern C++ features.
// It uses a combination of a doubly-linked list and an unordered_map for efficient O(1) operations.
template <typename Key, typename Value>
class LRUCache {
public:
    // Constructor to initialize cache capacity.
    explicit LRUCache(size_t capacity)
        : capacity_(capacity) {}

    // Get value associated with key.
    // Returns true and sets 'value' if found, false if not in cache.
    bool get(const Key& key, Value& value) {
        std::unique_lock lock(mutex_);

        auto it = cacheMap.find(key);
        if (it == cacheMap.end()) {
            return false; // Key not found
        }
        // Move the accessed node to the front (most recently used)
        cache_items_.splice(cache_items_.begin(), cache_items_, it->second);
        value = it->second->second;
        return true;
    }

    // Insert or update (key, value) in the cache.
    void put(const Key& key, const Value& value) {
        std::unique_lock lock(mutex_);

        auto it = cacheMap.find(key);
        if (it != cacheMap.end()) {
            // Key exists, update value and move to front
            it->second->second = value;
            cache_items_.splice(cache_items_.begin(), cache_items_, it->second);
            return;
        }

        // Insert new item at the front
        cache_items_.emplace_front(key, value);
        cacheMap[key] = cache_items_.begin();

        // If over capacity, remove the least recently used (from back)
        if (cacheMap.size() > capacity_) {
            auto lru = cache_items_.end();
            --lru;
            cacheMap.erase(lru->first);
            cache_items_.pop_back();
        }
    }

    // Remove a key from the cache.
    void remove(const Key& key) {
        std::unique_lock lock(mutex_);
        auto it = cacheMap.find(key);
        if (it != cacheMap.end()) {
            cache_items_.erase(it->second);
            cacheMap.erase(it);
        }
    }

    // Get current size of the cache.
    size_t size() const {
        std::shared_lock lock(mutex_);
        return cacheMap.size();
    }

private:
    mutable std::shared_mutex mutex_; // For thread-safe access (read/write lock)
    size_t capacity_; // Maximum capacity of cache

    // Doubly linked list to store cache keys and values.
    // Front of the list is most recently used.
    std::list<std::pair<Key, Value>> cache_items_;

    // Map from key to list iterator, allows O(1) access and removal.
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> cacheMap;
};
