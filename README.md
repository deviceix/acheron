# acheron

![Build & Unit Tests](https://github.com/deviceix/acheron/actions/workflows/build.yml/badge.svg)

This repository contains the source code for Acheron; a modern, type-safe and efficient
standard library subset in C++20.

Acheron provides optimized algorithms and containers designed to reduce runtime computation costs
while maintaining portability and API compatibility with the C++ standard library. It is designed to be
lightweight, performant, and suitable for both embedded and systems programming.

## Getting Started

To build Acheron, you need to have CMake 3.30+ and a C++20 compatible compiler, preferably LLVM Clang installed.

```shell
git clone https://github.com/deviceix/acheron.git
cd acheron
mkdir build && cd build
cmake -B build                      \
  -DCMAKE_BUILD_TYPE=Release        \
  -DACHERON_BUILD_TESTS=ON          \
  -DACHERON_BUILD_EXAMPLES=OFF      \
  -DACHERON_BUILD_BENCHMARKS=OFF
cmake --build build
```

### Available Build Options

| Option                     | Description      | Default |
|----------------------------|------------------|---------|
| `ACHERON_BUILD_BENCHMARKS` | Build benchmarks | `OFF`   |
| `ACHERON_BUILD_EXAMPLES`   | Build examples   | `OFF`   |
| `ACHERON_BUILD_TESTS`      | Build tests      | `ON`    |

## Quick Start

```c++
#include <acheron/vector>
#include <acheron/string>
#include <acheron/unordered_map>

int main()
{
    ach::vector<int> numbers = { 1, 2, 3, 4, 5 };
    numbers.push_back(6);
    
    /* optimized string */
    ach::string text = "Hello, World!";
    text.append(" From Acheron");
    
    /* Robin Hood hash map */
    ach::unordered_map<int, ach::string> map;
    map[1] = "one";
    map[2] = "two";
    
    return 0;
}
```

## Status

The project is actively being developed. The API aims for standard library compatibility where applicable.

| Component             | Status   | Notes                                  |
|-----------------------|----------|----------------------------------------|
| Core Containers       | Complete | vector, list, string                   |
| Atomic Operations     | Complete | Memory ordering, thread safety         |
| Hash Containers       | Complete | unordered_map with Robin Hood hashing  |
| Dynamic Containers    | Complete | deque, dynamic_bitset                  |
| Ordered Containers    | Planned  | map, set                               |
| Stack/Queue Adapters  | Complete | stack, queue                           |
| Algorithms            | Planned  | Sorting, searching, transformations    |
| Concurrent Containers | Planned  | Thread-safe variants of all containers |

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE.txt) file for details.

## Author

- Al (@alpluspluss)

## Contributing

Currently not accepting contributions. If you want to contribute, please open an issue or a pull request.

## Acknowledgements

- The C++ standards committee for their excellent work on the standard library specification papers.
- [cppreference](https://en.cppreference.com/) for providing a comprehensive reference for C++ standards.
