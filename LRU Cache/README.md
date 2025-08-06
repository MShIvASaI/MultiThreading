# MultiThreading LRU Cache

This repository provides a thread-safe Least Recently Used (LRU) Cache implementation in modern C++.  
It demonstrates the use of C++17 (or later) features and multithreading for high-performance concurrent access.

## Features

- **Thread-safe**: Uses `std::shared_mutex` for efficient concurrent access.
- **Modern C++**: Written using C++17 and above constructs.
- **Efficient O(1) Operations**: Uses a combination of `std::list` and `std::unordered_map` for fast access, insertion, and deletion.
- **Simple API**: Supports `get`, `put`, `remove`, and `size` operations.

## Usage

### 1. Include the Header

```cpp
#include "LRUCache.h"
```

### 2. Create an LRU Cache

```cpp
// Create a cache that can hold up to 10 items
LRUCache<int, std::string> cache(10);
```

### 3. Multi-threaded Example

```cpp
#include <thread>
#include <vector>
#include "LRUCache.h"

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
    LRUCache<int, std::string> cache(10);
    std::vector<std::thread> threads;

    // Start writer threads
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(writer, std::ref(cache), i);
    }
    for (auto& t : threads) t.join();

    threads.clear();

    // Start reader threads
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(reader, std::ref(cache), i);
    }
    for (auto& t : threads) t.join();

    return 0;
}
```

## How It Works

- The cache maintains a doubly linked list of key-value pairs.  
  The front of the list is the most recently used item.
- An unordered_map allows fast lookup of list nodes by key.
- All write operations (`get`, `put`, `remove`) are protected by a `std::shared_mutex` for thread safety.
- The cache automatically evicts the least recently used item when the capacity is exceeded.

## Requirements

- C++17 or higher
- Standard C++ libraries

## Contributing

Feel free to open issues or pull requests for improvements, bug fixes, or new features.

## License

This project is provided under the MIT License.