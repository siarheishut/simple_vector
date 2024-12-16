# Custom Vector Implementation

## Overview
This project contains a custom implementation of a dynamic array class template `Vector` in C++, which mimics the behavior of `std::vector`. It is designed to manage memory dynamically and supports typical vector operations, including element insertion, deletion, and resizing, with efficient memory handling.

The implementation also includes a `RawMemory` utility class to handle low-level memory allocation and deallocation.

---

## Features
1. **Dynamic Memory Management**
    - Memory is allocated and deallocated dynamically as needed.
    - Supports `Reserve` to preallocate memory for better performance.

2. **Construction and Destruction**
    - Supports value construction, copy construction, and move construction of elements.
    - Provides methods for explicit destruction of elements.

3. **Basic Operations**
    - Push and pop elements with `PushBack`, `PopBack`.
    - Resize the container with `Resize`.
    - Insert and erase elements at specific positions.
    - Emplace elements with perfect forwarding using `EmplaceBack` and `Emplace`.

4. **Iterators**
    - Provides `begin`, `end`, `cbegin`, `cend` iterators for range-based loops and STL compatibility.

5. **Efficiency**
    - Uses move semantics and uninitialized memory optimizations for better performance.

6. **Safety**
    - Prevents memory leaks with proper destruction and deallocation.
    - Deletes copy constructors and assignment operators for raw memory to ensure correctness.

---

## Example Usage
Below is a sample code snippet demonstrating how to use the `Vector` class:

```cpp
#include "my_vector.h"
#include <iostream>

int main() {
    Vector<int> v;

    // Add elements to the vector
    v.PushBack(1);
    v.PushBack(2);
    v.PushBack(3);

    std::cout << "Vector elements: ";
    for (const auto& elem : v) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    // Insert an element
    v.Insert(v.begin() + 1, 10);
    
    std::cout << "After insertion: ";
    for (const auto& elem : v) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    // Erase an element
    v.Erase(v.begin() + 2);
    
    std::cout << "After erasure: ";
    for (const auto& elem : v) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

### Output:
```
Vector elements: 1 2 3
After insertion: 1 10 2 3
After erasure: 1 10 3
```
