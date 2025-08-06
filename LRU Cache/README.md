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

In C++ multithreading, especially with `std::shared_mutex` (or `std::shared_timed_mutex`), you have two main lock types:

### 1. `std::shared_lock`
- **Purpose:** Allows multiple threads to acquire the lock for reading (shared access) simultaneously.
- **When to use:** Use `shared_lock` when you only need to **read** data and not modify it. Multiple threads can hold a `shared_lock` at the same time.

### 2. `std::unique_lock`
- **Purpose:** Allows only one thread to acquire the lock for writing (exclusive access).
- **When to use:** Use `unique_lock` when you need to **modify** (write/update) the shared data. Only one thread can hold a `unique_lock` at a time, and no thread can hold a `shared_lock` while a `unique_lock` is held.


**In this LRUCache.h, `put` uses `unique_lock` because it writes to the cache, and you want to ensure only one thread can modify the data at a time to avoid issues.**  
Use `shared_lock` for read-only operations, and `unique_lock` for write operations.

## Why `shared_mutex` is declared as `mutable` ?
Declaring a `shared_mutex` as `mutable` is important when you want to use it for thread synchronization inside a class that provides const member functions.

**Explanation:**

- In C++, a `const` member function promises not to modify the logical state of the object.
- However, you may still want to allow multiple threads to read from the object at the same time, using a `shared_mutex`.
- Acquiring a lock (even for reading) requires modifying the internal state of the mutex (e.g., locking counters).
- If your `shared_mutex` is not declared as `mutable`, you cannot lock or unlock it in a `const` member function, because that would violate the `const` promise.
- Declaring the `shared_mutex` as `mutable` allows you to lock/unlock the mutex even in `const` member functions, because `mutable` permits modification of that member even in `const` contexts.

**Example:**
```cpp
class Example {
    mutable std::shared_mutex mutex;
    int data;
public:
    int get() const {
        std::shared_lock lock(mutex); // OK, because mutex is mutable
        return data;
    }
};
```
Without `mutable`, the above would not compile, because `mutex` would be considered immutable in `const` functions.

**Summary:**  
Declare `shared_mutex` as `mutable` if you need to lock it in `const` member functions, enabling thread-safe reads without breaking the `const` promise of your API.

## Requirements

- C++17 or higher
- Standard C++ libraries

## Contributing

Feel free to open issues or pull requests for improvements, bug fixes, or new features.

## License

This project is provided under the MIT License.
