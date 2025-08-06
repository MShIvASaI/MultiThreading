#include <iostream>
#include <thread>
#include <vector>
#include "LRUCache.h"

// Example usage of thread-safe LRUCache in multi-threaded environment

void writer(LRUCache<int, std::string>& cache, int thread_id) {
    for (int i = 0; i < 10; ++i) {
        cache.put(thread_id * 100 + i, "Value_" + std::to_string(thread_id) + "_" + std::to_string(i));
    }
}

void reader(LRUCache<int, std::string>& cache, int thread_id) {
    for (int i = 0; i < 10; ++i) {
        std::string value;
        int key = thread_id * 100 + i;
        if (cache.get(key, value)) {
            std::cout << "Thread " << thread_id << " read: key=" << key << ", value=" << value << std::endl;
        }
    }
}

int main() {
    // Create LRU cache of capacity 10
    LRUCache<int, std::string> cache(10);

    std::vector<std::thread> threads;

    // Launch a few writer threads
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(writer, std::ref(cache), i);
    }

    // Wait for writers to finish
    for (auto& t : threads) t.join();

    threads.clear();

    // Launch reader threads
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(reader, std::ref(cache), i);
    }

    // Wait for readers to finish
    for (auto& t : threads) t.join();

    return 0;
}